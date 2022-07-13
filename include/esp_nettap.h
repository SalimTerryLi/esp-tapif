/*
 * SPDX-FileCopyrightText: 2021 SalimTerryLi <lhf2613@gmail.com>
 * SPDX-FileType: SOURCE
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <esp_netif.h>
#include "esp_netif_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef esp_err_t (*tap_transmit_f)(void* buffer, size_t len);
typedef esp_err_t (*tap_free_buffer_f)(void* buffer);

/**
 * @brief Forward declaration of TAP interface handle
 */
typedef struct tap_netif_driver_s* tap_netif_driver_t;

/**
 * @brief Creates tap driver instance to be used with esp-netif
 *
 * @param fn_tx callback function when there is data to be passed from network layer to link layer
 * @param fn_free callback function when low layer data is consumed and corresponding buffer can be freed
 *
 * @return
 *  - pointer to wifi interface handle on success
 *  - NULL otherwise
 */
tap_netif_driver_t esp_tap_create_if_driver(tap_transmit_f fn_tx, tap_free_buffer_f fn_free);

/**
 * @brief Destroys tap driver instance
 *
 * @param h pointer to tap interface handle
 *
 */
void esp_tap_destroy_if_driver(tap_netif_driver_t h);

/**
 * @brief Pass packets from link layer to network layer
 *
 * @param h pointer to tap interface handle
 * @param buffer data buffer
 * @param len data size
 *
 */
esp_err_t esp_tap_process_incoming_packet(tap_netif_driver_t h, void *buffer, size_t len);

#ifdef __cplusplus
}
#endif
