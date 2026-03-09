# Copyright 2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(INC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant
    ${CMAKE_CURRENT_SOURCE_DIR}/src/crypto
)

set(SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/drivers_morse.c
  ${CMAKE_CURRENT_SOURCE_DIR}/os_mmosal.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/common/dragonfly.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/common/hw_features_common.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/common/ieee802_11_common.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/common/ptksa_cache.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/common/sae.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/common/wpa_common.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/crypto/aes-unwrap.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/crypto/dh_groups.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/crypto/sha256-kdf.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/crypto/sha384-kdf.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/crypto/tls_none.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/crypto/sha512-kdf.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/driver_common.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/eap_common/eap_common.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/eap_peer/eap.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/eap_peer/eap_methods.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/eapol_supp/eapol_supp_sm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rsn_supp/preauth.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rsn_supp/pmksa_cache.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rsn_supp/wpa.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rsn_supp/wpa_ie.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/crc32.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/bitfield.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/common.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/wpa_debug.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/wpabuf.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/bgscan.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/bgscan_simple.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/autoscan.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/autoscan_exponential.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/bss.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/config.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/eap_register.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/events.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/notify.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/op_classes.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/scan.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/sme.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/wmm_ac.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/wpa_supplicant.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/wpas_glue.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/bssid_ignore.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/wnm_sta.c
  ${CMAKE_CURRENT_SOURCE_DIR}/crypto_mbedtls_mm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/morse_stubs.c
  ${CMAKE_CURRENT_SOURCE_DIR}/wpa_supplicant/ap.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/authsrv.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/bss_load.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/wmm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/ap_list.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/comeback_token.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pasn/pasn_responder.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/hw_features.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/dfs.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/tkip_countermeasures.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/hostapd.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/ieee802_1x.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/ieee802_11_auth.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/ieee802_11_s1g.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/sta_info.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/ap_config.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/ap_drv_ops.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/ap_mlme.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/beacon.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/drv_callbacks.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/eap_user_db.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/ieee802_11.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/ieee802_11_ht.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/ieee802_11_shared.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/neighbor_db.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/pmksa_cache_auth.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/rrm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/utils.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/wpa_auth.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/wpa_auth_glue.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ap/wpa_auth_ie.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/eapol_auth/eapol_auth_sm.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/eap_server/eap_server.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/eap_server/eap_server_methods.c
)

set(BUILD_DEFINES
  IEEE8021X_EAPOL=1
  CONFIG_SME=1
  CONFIG_SAE=1
  CONFIG_OWE=1
  CONFIG_ECC=1
  CONFIG_SHA256=1
  CONFIG_SHA512=1
  CONFIG_SHA384=1
  CONFIG_FIPS=1
  CONFIG_NO_ACCOUNTING=1
  CONFIG_NO_BSS_TRANS_MGMT=1
  CONFIG_NO_CONFIG_BLOBS=1
  CONFIG_NO_RADIUS=1
  CONFIG_NO_RANDOM_POOL=1
  CONFIG_NO_RC4=1
  CONFIG_NO_ROBUST_AV=1
  CONFIG_NO_RRM=1
  CONFIG_NO_VLAN=1
  CONFIG_WNM=1
  CONFIG_IEEE80211AH=1
  CONFIG_NO_CONFIG_WRITE=1
  OS_NO_C_LIB_DEFINES=1
  CONFIG_BGSCAN=1
  CONFIG_BGSCAN_SIMPLE=1
  CONFIG_AUTOSCAN=1
  CONFIG_AUTOSCAN_EXPONENTIAL=1
  CONFIG_S1G_TWT=1
  MAX_NUM_MLD_LINKS=1
  MAX_NUM_MLO_LINKS=1
  WPA_SUPPLICANT_CLEANUP_INTERVAL=120
  MM_IOT
  CONFIG_OPENSSL_INTERNAL_AES_WRAP=1
  MM_IOT_DPP_HEADER=1
  CONFIG_AP=1
  NEED_AP_MLME=1
)

add_library(mmhostap STATIC ${SRCS})
target_include_directories(mmhostap PUBLIC ${INC})
target_compile_definitions(mmhostap PUBLIC ${BUILD_DEFINES})
target_compile_options(mmhostap PRIVATE "-Wno-c++-compat")
target_compile_options(mmhostap PRIVATE "-Wno-unused-but-set-variable")
target_compile_options(mmhostap PRIVATE "-Wno-unused-function")
target_compile_options(mmhostap PRIVATE "-Wno-unused-parameter")
target_compile_options(mmhostap PRIVATE "-Wno-unused-variable")
target_compile_options(mmhostap PRIVATE "-Wno-format")
target_compile_options(mmhostap PRIVATE "-Wno-maybe-uninitialized")
target_compile_options(mmhostap PRIVATE "-Wno-dangling-else")
target_compile_options(mmhostap PRIVATE "-Wno-type-limits")
target_compile_options(mmhostap PRIVATE "-Wno-sign-compare")
target_compile_options(mmhostap PRIVATE "-Wno-parentheses")
target_compile_options(mmhostap PRIVATE "-Wno-packed-not-aligned")
target_compile_options(mmhostap PRIVATE "-Wno-misleading-indentation")
target_compile_options(mmhostap PRIVATE "-Wno-address")
add_dependencies(mmhostap idf::mbedtls)
target_link_libraries(mmhostap PUBLIC
    mmutils idf::mbedtls)
