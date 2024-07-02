/*
 * Copyright (c) 2024 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "camera.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(golioth_camera, LOG_LEVEL_DBG);

#include <Arducam/ArducamCamera.h>

struct camera_module
{
    ArducamCamera camera;
};

/* The library requires a param value but it is not used by the SPI HAL in this project so this
 * value may be any int */
#define ARDUCAM_CS_PIN 42

struct camera_module *camera_init(void)
{
    struct camera_module *cam_mod = malloc(sizeof(struct camera_module));
    if (!cam_mod) {
        LOG_ERR("Init failed: unable to allocate memory");
        return NULL;
    }

    cam_mod->camera = createArducamCamera(ARDUCAM_CS_PIN);
    begin(&cam_mod->camera);
    setImageQuality(&cam_mod->camera, LOW_QUALITY);
    return cam_mod;
}

int camera_deinit(struct camera_module *cam_mod)
{
    if (!cam_mod) {
        LOG_ERR("Deinit failed: camera_module is NULL");
        return -EINVAL;
    }

    free(cam_mod);
    return 0;
}

int camera_capture_image(struct camera_module *cam_mod)
{
    if (!cam_mod)
    {
        LOG_ERR("Image capture failed: camera pointer is NULL");
        return -EINVAL;
    }

    LOG_INF("Taking photo");
    takePicture(&cam_mod->camera, CAM_IMAGE_MODE_VGA, CAM_IMAGE_PIX_FMT_JPG);
    return 0;
}

int camera_get_next_block(struct camera_module *cam_mod,
                          uint8_t *block_buffer,
                          size_t *block_size,
                          bool *is_last,
                          size_t *bytes_remaining)
{
    size_t max_block_size = *block_size;

    if (!cam_mod)
    {
        LOG_ERR("Cannot get image block: camera pointer is NULL");
        return -EINVAL;
    }

    if (cam_mod->camera.receivedLength == 0)
    {
        LOG_WRN("No bytes remaining in image");
        return -ENODATA;
    }

    *block_size = readBuff(&cam_mod->camera, block_buffer, max_block_size);
    *bytes_remaining = cam_mod->camera.receivedLength;
    *is_last = (*bytes_remaining == 0) ? true : false;

    return 0;
}
