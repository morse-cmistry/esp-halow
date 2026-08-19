/*
 * Copyright 2023-2025 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#include "mmhal_wlan.h"
#include "mmhal_os.h"
#include "mmpkt.h"
#include "mmwlan.h"
#include "mmwlan_stats.h"

/* mmregdb.h (components/mm-iot-sdk/framework/src/mmregdb/mmregdb.h) is an internal SDK header
 * with no extern "C" guard of its own; wrap it here so get_regulatory_db() keeps C linkage when
 * this header is consumed by C++ translation units. */
#ifdef __cplusplus
extern "C" {
#endif
#include "mmregdb.h"
#ifdef __cplusplus
}
#endif

typedef struct mmhalow_wifi_config_t{
    union {
        struct mmwlan_sta_args sta;
        struct mmwlan_ap_args ap;
    };
} mmhalow_wifi_config_t;

struct mmhalow_scan_args
{
    mmwlan_scan_rx_cb_t rx_cb;
    mmwlan_scan_complete_cb_t complete_cb;
    void *cb_arg;
};

/* mmhalow.cpp is compiled as C++, so without this guard its function definitions would get
 * C++-mangled symbol names — fine for the C++ examples in this repo, but unusable from a plain C
 * translation unit, which expects the unmangled names declared below. This block is what makes
 * the functions below linkable from C at all. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the WLAN interface
 * @warning This must be called only once.
 */
esp_err_t mmhalow_init(const wifi_init_config_t *config);

/**
 * Shut down the WLAN interface
 */
esp_err_t mmhalow_deinit(void);

/**
 * Scans for APs
 */
esp_err_t mmhalow_scan(struct mmhalow_scan_args *args);

/**
 * Connect to an AP
 */
esp_err_t mmhalow_connect(mmwlan_sta_status_cb_t cb);

/**
 * Disconnect from an AP
 */
esp_err_t mmhalow_disconnect(void);

/**
 * Set the network configuration
 */
esp_err_t mmhalow_set_config(wifi_interface_t interface, mmhalow_wifi_config_t *conf);

/**
 * Get the network configuration
 */
esp_err_t mmhalow_get_config(wifi_interface_t interface, mmhalow_wifi_config_t *conf);

/**
 * Get the STA State
 */
enum mmwlan_status mmhalow_status(void);

/**
 * Print BCF/Firmware/Morselib version information
 */
void mmhalow_print_version_info(void);

/**
 * Start an AP interface
 */
void mmhalow_wifi_start(void);

#ifdef __cplusplus
}
#endif
