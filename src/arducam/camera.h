/*
 * Copyright (c) 2024 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __CAMERA_H
#define __CAMERA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct camera_module;

struct camera_module *camera_init(void);
int camera_deinit(struct camera_module *cam_mod);
int camera_capture_image(struct camera_module *cam_mod);
int camera_get_next_block(struct camera_module *cam_mod,
                          uint8_t *block_buffer,
                          size_t *block_size,
                          bool *is_last,
                          size_t *bytes_remaining);

#endif /* __CAMERA_H */
