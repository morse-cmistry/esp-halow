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

/* Default SSID  */
#ifndef SSID
/** SSID of the AP to connect to. (Do not quote; it will be stringified.) */
#define SSID CONFIG_HALOW_SSID
#endif

/* Default passphrase  */
#ifndef SAE_PASSPHRASE
/** Passphrase of the AP (ignored if security type is not SAE).
 *  (Do not quote; it will be stringified.) */
#define SAE_PASSPHRASE CONFIG_HALOW_PASSWORD
#endif

/* Default security type  */
#ifndef SECURITY_TYPE
/** Security type (@see mmwlan_security_type). */
#define SECURITY_TYPE MMWLAN_SAE
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

static void wifi_start(void *esp_netif)
{
    uint8_t mac[6];

    ESP_LOGD(TAG, "%s esp-netif:%p " PRId32 "", __func__, esp_netif);

    mmwlan_get_mac_addr(mac);
    ESP_LOGD(TAG, "WIFI mac address: %x %x %x %x %x %x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    esp_netif_set_mac(esp_netif, mac);
    esp_netif_action_start(esp_netif, NULL, 0, NULL);
}

void mm_halow_init()
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

    wifi_start(netif);
}

void scan_rx_cb(const struct mmwlan_scan_result *result, void *arg){
	printf("SSID: %s", result->ssid);
	return;
}
void scan_complete_cb(enum mmwlan_scan_state scan_state, void *arg){
	return;
}

int mm_halow_scan(void){
	struct mmwlan_scan_req scan_req	= MMWLAN_SCAN_REQ_INIT;
	scan_req.scan_rx_cb = scan_rx_cb;
	scan_req.scan_complete_cb = scan_complete_cb;
	int ret = mmwlan_scan_request(&scan_req);
	return ret;
}

void mm_halow_stop(void)
{
    /* Shutdown wlan interface */
    mmwlan_shutdown();
}

void mm_halow_connect(const char* ssid, const char* pass){

    enum mmwlan_status status;

    struct mmwlan_sta_args sta_args = MMWLAN_STA_ARGS_INIT;

    (void)mmosal_safer_strcpy((char *)sta_args.ssid, ssid, MMWLAN_SSID_MAXLEN-1);
    sta_args.ssid_len = strlen((char *)sta_args.ssid);

    (void)mmosal_safer_strcpy(sta_args.passphrase, pass,
                              MMWLAN_PASSPHRASE_MAXLEN);
    sta_args.passphrase_len = strlen(sta_args.passphrase);

    sta_args.security_type = SECURITY_TYPE;

    status = mmwlan_sta_enable(&sta_args, sta_status_callback);
    MMOSAL_ASSERT(status == MMWLAN_SUCCESS);

    mmwlan_set_power_save_mode(MMWLAN_PS_DISABLED);
}

void mm_halow_disconnect(){
	mmwlan_sta_disable();
}

int mm_halow_status(){
	return mmwlan_get_sta_state();
}
