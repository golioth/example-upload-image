/*
 * Copyright (c) 2022-2023 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "golioth/golioth_status.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(example_upload_image, LOG_LEVEL_DBG);

#include <golioth/client.h>
#include <golioth/fw_update.h>
#include <golioth/settings.h>
#include <golioth/stream.h>
#include <samples/common/net_connect.h>
#include <samples/common/sample_credentials.h>
#include <zephyr/kernel.h>

#include "fables.h"

/* Current firmware version; update in prj.conf or via build argument */
static const char *_current_version = CONFIG_MCUBOOT_IMGTOOL_SIGN_VERSION;

static struct golioth_client *client;
K_SEM_DEFINE(connected, 0, 1);

static k_tid_t _system_thread = 0;

static int32_t _loop_delay_s = 5;
#define LOOP_DELAY_S_MAX 43200
#define LOOP_DELAY_S_MIN 0

struct block_upload_source
{
    uint8_t *buf;
    size_t len;
};

static void wake_system_thread(void)
{
    k_wakeup(_system_thread);
}

static void on_client_event(struct golioth_client *client,
                            enum golioth_client_event event,
                            void *arg)
{
    bool is_connected = (event == GOLIOTH_CLIENT_EVENT_CONNECTED);

    if (is_connected)
    {
        k_sem_give(&connected);
    }
    LOG_INF("Golioth client %s", is_connected ? "connected" : "disconnected");
}

static enum golioth_settings_status on_loop_delay_setting(int32_t new_value, void *arg)
{
    _loop_delay_s = new_value;
    LOG_INF("Set loop delay to %i seconds", new_value);
    wake_system_thread();
    return GOLIOTH_SETTINGS_SUCCESS;
}

static int app_settings_register(struct golioth_client *client)
{
    struct golioth_settings *settings = golioth_settings_init(client);

    int err = golioth_settings_register_int_with_range(settings,
                                                       "LOOP_DELAY_S",
                                                       LOOP_DELAY_S_MIN,
                                                       LOOP_DELAY_S_MAX,
                                                       on_loop_delay_setting,
                                                       NULL);

    if (err)
    {
        LOG_ERR("Failed to register settings callback: %d", err);
    }

    return err;
}

/* Callback invoked by the Golioth SDK to read the next block of data
   data to send to Golioth */
static enum golioth_status block_upload_read_chunk(uint32_t block_idx,
                                                   uint8_t *block_buffer,
                                                   size_t *block_size,
                                                   bool *is_last,
                                                   void *arg)
{
    size_t bu_max_block_size = *block_size;
    const struct block_upload_source *bu_source = arg;
    size_t bu_offset = block_idx * bu_max_block_size;
    size_t bu_size = bu_source->len - bu_offset;

    LOG_DBG("block-idx: %u bu_offset: %u bytes_remaining: %u", block_idx, bu_offset, bu_size);

    if (bu_offset >= bu_source->len)
    {
        LOG_ERR("Calculated offset is past end of buffer: %d", bu_offset);
        goto bu_error;
    }

    if (bu_size < 0)
    {
        LOG_ERR("Calculated size for next block is < 0: %d", bu_size);
        goto bu_error;
    }

    if (bu_size <= bu_max_block_size)
    {
        *block_size = bu_size;
        *is_last = true;
    }
    else
    {
        *block_size = bu_max_block_size;
        *is_last = false;
    }

    memcpy(block_buffer, bu_source->buf + bu_offset, *block_size);
    return GOLIOTH_OK;

bu_error:
    *block_size = 0;
    *is_last = true;

    return GOLIOTH_ERR_NO_MORE_DATA;
}

static int upload_txt_file(void)
{
    /* Test function that uses block upload with a .txt file */
    struct block_upload_source bu_ctx = {.buf = (uint8_t *) &fables, .len = fables_len};

    int err = golioth_stream_set_blockwise_sync(client,
                                                "blockupload",
                                                GOLIOTH_CONTENT_TYPE_OCTET_STREAM,
                                                block_upload_read_chunk,
                                                &bu_ctx);

    return err;
}

int main(void)
{
    LOG_DBG("Start Golioth example_upload_image");
    LOG_INF("Firmware version: %s", CONFIG_MCUBOOT_IMGTOOL_SIGN_VERSION);

    /* Get system thread id so loop delay change event can wake main */
    _system_thread = k_current_get();

    /* Start the network connection */
    net_connect();

    /* Get the client configuration from auto-loaded settings */
    const struct golioth_client_config *client_config = golioth_sample_credentials_get();

    /* Create and start a Golioth Client */
    client = golioth_client_create(client_config);

    /* Register Golioth event callback */
    golioth_client_register_event_callback(client, on_client_event, NULL);

    /* Initialize Golioth OTA firmware update service */
    golioth_fw_update_init(client, _current_version);

    /* Register Golioth Settings service */
    app_settings_register(client);

    /* Block until connected to Golioth */
    k_sem_take(&connected, K_FOREVER);

    int counter = 0;
    int err;

    while (true)
    {
        LOG_INF("Golioth hello! %d", counter);

        char buf[32];
        snprintk(buf, sizeof(buf), "{\"counter\":%d}", counter);

        if (counter == 4)
        {
            err = upload_txt_file();
            if (err)
            {
                LOG_ERR("Error during block upload: %d", err);
            }
            else
            {
                LOG_INF("Block upload successful!");
            }
        }

        ++counter;
        k_sleep(K_SECONDS(_loop_delay_s));
    }
}
