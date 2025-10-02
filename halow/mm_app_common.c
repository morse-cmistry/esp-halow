/*
 * Copyright 2023 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "esp_netif.h"
#include "esp_log.h"

#include "mmhal.h"
#include "mmosal.h"
#include "mmwlan.h"

#include "mm_app_common.h"
#include "mm_app_loadconfig.h"
#include "halow.h"

static const char *TAG = "app_common";

/** Maximum number of DNS servers to attempt to retrieve from config store. */
#ifndef DNS_MAX_SERVERS
#define DNS_MAX_SERVERS 2
#endif

/** Binary semaphore used to start user_main() once the link comes up. */
static struct mmosal_semb *link_established = NULL;

/**
 * WLAN station status callback, invoked when WLAN STA state changes.
 *
 * @param sta_state  The new STA state.
 */
static void sta_status_callback(enum mmwlan_sta_state sta_state)
{
    switch (sta_state)
    {
    case MMWLAN_STA_DISABLED:
        printf("WLAN STA disabled\n");
        break;

    case MMWLAN_STA_CONNECTING:
        printf("WLAN STA connecting\n");
        break;

    case MMWLAN_STA_CONNECTED:
        printf("WLAN STA connected\n");
        break;
    }
}

void app_print_version_info(void)
{
    enum mmwlan_status status;
    struct mmwlan_version version;
    struct mmwlan_bcf_metadata bcf_metadata;

    printf("-----------------------------------\n");

    status = mmwlan_get_bcf_metadata(&bcf_metadata);
    if (status == MMWLAN_SUCCESS)
    {
        printf("  BCF API version:         %u.%u.%u\n", bcf_metadata.version.major,
               bcf_metadata.version.minor, bcf_metadata.version.patch);
        if (bcf_metadata.build_version[0] != '\0')
        {
            printf("  BCF build version:       %s\n", bcf_metadata.build_version);
        }
        if (bcf_metadata.board_desc[0] != '\0')
        {
            printf("  BCF board description:   %s\n", bcf_metadata.board_desc);
        }
    }
    else
    {
        printf("  !! BCF metadata retrival failed !!\n");
    }

    status = mmwlan_get_version(&version);
    if (status != MMWLAN_SUCCESS)
    {
        printf("  !! Error occured whilst retrieving version info !!\n");
    }
    printf("  Morselib version:        %s\n", version.morselib_version);
    printf("  Morse firmware version:  %s\n", version.morse_fw_version);
    printf("  Morse chip ID:           0x%04lx\n", version.morse_chip_id);
    printf("-----------------------------------\n");

    MMOSAL_ASSERT(status == MMWLAN_SUCCESS);
}

void app_wlan_init(void)
{
    /* Ensure we don't call twice */
    MMOSAL_ASSERT(link_established == NULL);
    link_established = mmosal_semb_create("link_established");

    /* Initialize Morse subsystems, note that they must be called in this order. */
    mmhal_init();
    mmwlan_init();

    mmwlan_set_channel_list(load_channel_list());

    /* Boot the WLAN interface so that we can retrieve the firmware version. */
    struct mmwlan_boot_args boot_args = MMWLAN_BOOT_ARGS_INIT;
    (void)mmwlan_boot(&boot_args);
    app_print_version_info();
}

static void wifi_start(void *esp_netif)
{
    uint8_t mac[6];
    esp_err_t ret;

    ESP_LOGD(TAG, "%s esp-netif:%p " PRId32 "", __func__, esp_netif);

    // wifi_netif_driver_t driver = esp_netif_get_io_driver(esp_netif);

    mmwlan_get_mac_addr(mac);
    // if ((ret = esp_wifi_get_if_mac(driver, mac)) != ESP_OK) {
    //     ESP_LOGE(TAG, "esp_wifi_get_mac failed with %d", ret);
    //     return;
    // }
    ESP_LOGD(TAG, "WIFI mac address: %x %x %x %x %x %x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    // if (esp_wifi_is_if_ready_when_started(driver)) {
    //     if ((ret = esp_wifi_register_if_rxcb(driver,  esp_netif_receive, esp_netif)) != ESP_OK) {
    //         ESP_LOGE(TAG, "esp_wifi_register_if_rxcb for if=%p failed with %d", driver, ret);
    //         return;
    //     }
    // }

    // TODO: I'm guessing this is for zero copy tx??
    // if ((ret = esp_wifi_internal_reg_netstack_buf_cb(esp_netif_netstack_buf_ref, esp_netif_netstack_buf_free)) != ESP_OK) {
    //     ESP_LOGE(TAG, "netstack cb reg failed with %d", ret);
    //     return;
    // }
    esp_netif_set_mac(esp_netif, mac);
    // esp_netif_action_start(esp_netif, base, event_id, data);
    esp_netif_action_start(esp_netif, NULL, 0, NULL);
}


void app_wlan_start(void)
{
    enum mmwlan_status status;

    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif);
    halow_netif_driver_t *driver = esp_halow_create_if_driver();

    esp_netif_attach(netif, driver);

    status = mmwlan_register_rx_pkt_cb(halow_rx, netif);
    MMOSAL_ASSERT(status == MMWLAN_SUCCESS);

    status = mmwlan_register_link_state_cb(halow_link_state, netif);
    MMOSAL_ASSERT(status == MMWLAN_SUCCESS);


    /* Load Wi-Fi settings from config store */
    struct mmwlan_sta_args sta_args = MMWLAN_STA_ARGS_INIT;
    load_mmwlan_sta_args(&sta_args);
    load_mmwlan_settings();

    printf("Attempting to connect to %s ", sta_args.ssid);
    if (sta_args.security_type == MMWLAN_SAE)
    {
        printf("with passphrase %s", sta_args.passphrase);
    }
    printf("\n");
    printf("This may take some time (~30 seconds)\n");

    status = mmwlan_sta_enable(&sta_args, sta_status_callback);
    MMOSAL_ASSERT(status == MMWLAN_SUCCESS);

    // TODO: we should have booted the chip already.
    wifi_start(netif);

    /* Wait for link status callback.
     * Use a binary semaphore to block us until Link is up.
    //  */
    // mmosal_semb_wait(link_established, UINT32_MAX);

    /* Wi-Fi link is now established, return to caller */
}

void app_wlan_stop(void)
{
    /* Shutdown wlan interface */
    mmwlan_shutdown();
}

int mm_test(void){
	return 5000;
}
