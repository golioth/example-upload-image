/*
 * Copyright (c) 2024 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __CAMERA_H
#define __CAMERA_H

#include <stdint.h>
#include <golioth/client.h>

void camera_init(void);
void camera_capture_image(void);
enum golioth_status camera_get_next_block(uint32_t block_idx,
                                          uint8_t *block_buffer,
                                          size_t *block_size,
                                          bool *is_last,
                                          void *arg);

#endif /* __CAMERA_H */
