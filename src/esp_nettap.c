/*
 * SPDX-FileCopyrightText: 2021 SalimTerryLi <lhf2613@gmail.com>
 * SPDX-FileType: SOURCE
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_log.h>
#include "esp_nettap.h"

typedef struct tap_interface {
    tap_transmit_f tx_func;
    tap_free_buffer_f free_func;
} tap_interface_t;

/**
 * @brief BNEP netif driver structure
 */
struct tap_netif_driver_s {
    esp_netif_driver_base_t base;
    tap_interface_t tap_if;
};

static const char* TAG = "tap_netif";

static esp_err_t tap_transmit(void *h, void *buffer, size_t len)
{
    tap_netif_driver_t driver = h;
    return driver->tap_if.tx_func(buffer, len);
}

static void tap_l2_free(void *h, void* buffer)
{
    tap_netif_driver_t driver = h;
    if (driver->tap_if.free_func) {
        driver->tap_if.free_func(buffer);
    }
}

static esp_err_t tap_driver_start(esp_netif_t * esp_netif, void * args)
{
    tap_netif_driver_t driver = args;
    driver->base.netif = esp_netif;
    esp_netif_driver_ifconfig_t driver_ifconfig = {
            .handle =  driver,
            .transmit = tap_transmit,
            .driver_free_rx_buffer = tap_l2_free
    };

    return esp_netif_set_driver_config(esp_netif, &driver_ifconfig);
}

tap_netif_driver_t esp_tap_create_if_driver(tap_transmit_f fn_tx, tap_free_buffer_f fn_free)
{
    tap_netif_driver_t driver = calloc(1, sizeof(struct tap_netif_driver_s));
    if (driver == NULL) {
        ESP_LOGE(TAG, "No memory to create a wifi interface handle");
        return NULL;
    }
    driver->base.post_attach = tap_driver_start;
    driver->tap_if.tx_func = fn_tx;
    driver->tap_if.free_func = fn_free;
    return driver;
}

void esp_tap_destroy_if_driver(tap_netif_driver_t h)
{
    free(h);
}

esp_err_t esp_tap_process_incoming_packet(tap_netif_driver_t h, void *buffer, size_t len){
    return esp_netif_receive(h->base.netif, buffer, len, NULL);
}
