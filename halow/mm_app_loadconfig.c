/*
 * Copyright 2021-2023 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Morse Micro load configuration helper
 *
 * This file contains helper routines to load commonly used configuration settings
 * such as SSID, password, IP address settings and country code from the config store.
 * If a particular setting is not found, defaults are used.  It is safe to call these
 * functions if none of the settings are available in config store.
 */

#include "mm_app_loadconfig.h"
#include "mm_app_regdb.h"
#include "mmosal.h"
#include "mmwlan.h"

#ifndef COUNTRY_CODE
#define COUNTRY_CODE CONFIG_HALOW_COUNTRY_CODE
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

/** Stringify macro. Do not use directly; use @ref STRINGIFY(). */
#define _STRINGIFY(x) #x
/** Convert the content of the given macro to a string. */
#define STRINGIFY(x) _STRINGIFY(x)

const struct mmwlan_s1g_channel_list *load_channel_list(void)
{
    char strval[16];
    const struct mmwlan_s1g_channel_list *channel_list;

    (void)mmosal_safer_strcpy(strval, COUNTRY_CODE, sizeof(strval));
    channel_list = mmwlan_lookup_regulatory_domain(get_regulatory_db(), strval);
    if (channel_list == NULL)
    {
        printf("Could not find specified regulatory domain matching country code %s\n", strval);
        printf("Please set the configuration key wlan.country_code to the correct country code.\n");
        MMOSAL_ASSERT(false);
    }
    return channel_list;
}

void load_mmwlan_sta_args(struct mmwlan_sta_args *sta_config)
{
    (void)mmosal_safer_strcpy((char *)sta_config->ssid, SSID, sizeof(sta_config->ssid));
    sta_config->ssid_len = strlen((char *)sta_config->ssid);

    (void)mmosal_safer_strcpy(sta_config->passphrase, SAE_PASSPHRASE,
                              sizeof(sta_config->passphrase));
    sta_config->passphrase_len = strlen(sta_config->passphrase);

    sta_config->security_type = SECURITY_TYPE;
}

void load_mmwlan_settings(void)
{
}
