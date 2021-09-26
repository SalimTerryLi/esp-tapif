/*
 * SPDX-FileCopyrightText: 2021 SalimTerryLi <lhf2613@gmail.com>
 * SPDX-FileType: SOURCE
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_netif.h"
#include "esp_netif_lwip_internal.h"

#ifdef CONFIG_ESP_NETIF_TCPIP_LWIP

#include "esp_netif_lwip_tap.h"

//
// Purpose of this object is to define default network stack configuration
//  of basic types of interfaces using lwip network stack
//

static const struct esp_netif_netstack_config s_tap_netif_config = {
        .lwip = {
            .init_fn = tapif_init,
            .input_fn = tapif_input,
        }
};

const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_tap      = &s_tap_netif_config;

#endif /*CONFIG_ESP_NETIF_TCPIP_LWIP*/
