/*
 * SPDX-FileCopyrightText: 2021 SalimTerryLi <lhf2613@gmail.com>
 * SPDX-FileType: SOURCE
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_nettap.h"
#include "esp_tapif_defaults.h"
#include "esp_event.h"

static const char *TAG = "TAPIF_EXAMPLE";

static esp_err_t send_packets(void *buffer, size_t len){
    ESP_LOGI(TAG, "packet of %u bytes is going to be sent", len);
    return ESP_OK;
}

static esp_err_t free_buffer_cb(void* buffer){
    return ESP_OK;
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "TAP interface Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "Netmask: " IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "Gateway: " IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");

    // manually update DNS if required
    esp_netif_t *tap_if = event_data;
    esp_netif_dns_info_t dnsinfo={};
    esp_netif_str_to_ip4("223.5.5.5", &dnsinfo.ip.u_addr.ip4);
    dnsinfo.ip.type = ESP_IPADDR_TYPE_V4;
    esp_netif_set_dns_info(tap_if, ESP_NETIF_DNS_MAIN, &dnsinfo);
}

void app_main(void)
{
    ESP_LOGI(TAG, "esp tap interface example");

    // init esp_netif
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // create tap_if @ ip stack layer
    esp_netif_config_t tap_cfg = ESP_NETIF_DEFAULT_TAP();
    esp_netif_t *tap_if = NULL;
    tap_if = esp_netif_new(&tap_cfg);
    if (tap_if == NULL){
        ESP_ERROR_CHECK(ESP_ERR_INVALID_STATE);
    }

    // create tap_if @ ethernet driver layer
    tap_netif_driver_t tapif_drv;
    tapif_drv = esp_tap_create_if_driver(send_packets, free_buffer_cb);
    if (tapif_drv == NULL){
        ESP_ERROR_CHECK(ESP_ERR_INVALID_STATE);
    }

    // set MAC addr
    uint8_t mac_addr[6] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88};
    ESP_ERROR_CHECK(esp_netif_set_mac(tap_if, mac_addr));

    // register event callbacks
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, tap_if));

    // attach ip stack with eth driver
    ESP_ERROR_CHECK(esp_netif_attach(tap_if, tapif_drv));

    ESP_LOGI(TAG, "starting tapif...");
    esp_netif_action_start(tap_if, NULL, 0, NULL);
    // these two usually should be called when ethernet link is available
    esp_netif_action_connected(tap_if, NULL, 0, NULL);

    // pass something to ip stack
    uint8_t buffer[128];
    ESP_ERROR_CHECK(esp_tap_process_incoming_packet(tapif_drv, buffer, sizeof(buffer)));

    vTaskDelay(pdMS_TO_TICKS(10000));

    // stop tapif
    esp_netif_action_disconnected(tap_if, NULL, 0, NULL);
    esp_netif_action_stop(tap_if, NULL, 0, NULL);
    ESP_LOGI(TAG, "tapif stopped");
}
