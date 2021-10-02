/*
 * SPDX-FileCopyrightText: 2021 SalimTerryLi <lhf2613@gmail.com>
 * SPDX-FileType: SOURCE
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_netif.h"
#include "esp_tapif_defaults.h"

//
// Default configuration of TAP
//
const esp_netif_inherent_config_t _g_esp_netif_inherent_tap_config = ESP_NETIF_INHERENT_DEFAULT_TAP();

