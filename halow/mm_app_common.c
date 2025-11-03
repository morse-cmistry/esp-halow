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

#include "mmhal.h"
#include "mmosal.h"
#include "mmwlan.h"
#include "mm_app_regdb.h"

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

/** Length of string representation of a MAC address (i.e., "XX:XX:XX:XX:XX:XX")
 * including null terminator. */
#define MAC_ADDR_STR_LEN    (18)

/*
 * If ASNI_ESCAPE_ENABLED is non-zero (the default) then ANSI escape characters will be used to
 *  format the log output.
 */
#if !(defined(ASNI_ESCAPE_ENABLED) && ASNI_ESCAPE_ENABLED == 0)
/** ANSI escape sequence for bold text. */
#define ANSI_BOLD  "\x1b[1m"
/** ANSI escape sequence to reset font. */
#define ANSI_RESET "\x1b[0m"
#else
/** ANSI escape sequence for bold text (disabled so no-op). */
#define ANSI_BOLD  ""
/** ANSI escape sequence to reset font (disabled so no-op). */
#define ANSI_RESET ""
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
    mmwlan_set_power_save_mode(MMWLAN_PS_DISABLED);

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
    (void)(arg);
    char bssid_str[MAC_ADDR_STR_LEN];
    char ssid_str[MMWLAN_SSID_MAXLEN];
    int ret;
    static int num_scan_results;
	num_scan_results++;

    snprintf(bssid_str, MAC_ADDR_STR_LEN, "%02x:%02x:%02x:%02x:%02x:%02x",
             result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3],
             result->bssid[4], result->bssid[5]);
    snprintf(ssid_str, (result->ssid_len+1), "%s", result->ssid);

    printf(ANSI_BOLD "%2d. %s" ANSI_RESET "\n", num_scan_results, ssid_str);
    printf("    Operating BW: %u MHz\n",  result->op_bw_mhz);
    printf("    BSSID: %s\n", bssid_str);
    printf("    RSSI: %3d\n", result->rssi);
    printf("    Beacon Interval(TUs): %u\n", result->beacon_interval);
    printf("    Capability Info: 0x%04x\n", result->capability_info);

    //ret = parse_rsn_information(result->ies, result->ies_len, &rsn_info);
    //if (ret < 0)
    //{
    //    printf("    Invalid probe response\n");
    //}
    //else if (rsn_info.num_akm_suites == 0)
    //{
    //    printf("    Security: None\n");
    //}
    //else if (ret > 0)
    //{
    //    unsigned ii;
    //    printf("    Security:");
    //    for (ii = 0; ii < rsn_info.num_akm_suites; ii++)
    //    {
    //        printf(" %s", akm_suite_to_string(rsn_info.akm_suites[ii]));
    //    }
    //    printf("\n");
    //}
	return;
}
void scan_complete_cb(enum mmwlan_scan_state scan_state, void *arg){
	return;
}

int mm_halow_scan(void){
	struct mmwlan_scan_req scan_req	= MMWLAN_SCAN_REQ_INIT;
	scan_req.scan_rx_cb = scan_rx_cb;
	scan_req.scan_complete_cb = scan_complete_cb;
	printf("Scan Request init\n");
	int ret = mmwlan_scan_request(&scan_req);
	printf("Scan Request done\n");
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
}

void mm_halow_disconnect(){
	mmwlan_sta_disable();
}

int mm_halow_status(){
	return mmwlan_get_sta_state();
}
