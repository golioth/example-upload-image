/*
 * Copyright (c) 2024 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(golioth_camera, LOG_LEVEL_DBG);

#include <Arducam/ArducamCamera.h>
#include <golioth/stream.h>

static ArducamCamera camera_mod;
static ArducamCamera *cam_p;

/* The library requires a param value but it is not used by the SPI HAL in this project so this
 * value may be any int */
#define ARDUCAM_CS_PIN 42

void camera_init(void)
{
    camera_mod = createArducamCamera(ARDUCAM_CS_PIN);
    cam_p = &camera_mod;
    begin(cam_p);
    setImageQuality(cam_p, LOW_QUALITY);
}

void camera_capture_image(void)
{
    if (!cam_p)
    {
        LOG_ERR("Camera pointer is NULL, was it initialized?");
        return;
    }

    LOG_INF("Taking photo");
    takePicture(cam_p, CAM_IMAGE_MODE_VGA, CAM_IMAGE_PIX_FMT_JPG);
}

enum golioth_status camera_get_next_block(uint32_t block_idx,
                                          uint8_t *block_buffer,
                                          size_t *block_size,
                                          bool *is_last,
                                          void *arg)
{
    size_t max_block_size = *block_size;

    if (!cam_p)
    {
        LOG_ERR("Camera pointer is NULL, was it initialized?");
        goto camera_chunk_error;
    }

    if (cam_p->receivedLength == 0)
    {
        LOG_WRN("No bytes remaininig in image");
        goto camera_chunk_error;
    }

    *block_size = readBuff(cam_p, block_buffer, max_block_size);
    *is_last = (cam_p->receivedLength == 0) ? true : false;

    LOG_DBG("Uploading block_id: %u block_size: %u is_last: %d Bytes remaining: %u",
            block_idx,
            *block_size,
            *is_last,
            cam_p->receivedLength);

    return GOLIOTH_OK;

camera_chunk_error:
    *block_size = 0;
    *is_last = true;

    return GOLIOTH_ERR_NO_MORE_DATA;
}
