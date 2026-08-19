/*
 * Copyright 2023-2025 Morse Micro
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mmhalow.h"

#include <array>
#include <cassert>
#include <cstring>

#include "esp_log.h"
#include "mmlog.h"

namespace
{

constexpr const char *TAG = "Morse Micro HaLow NetIF";

/**
 * Owns all state for the single HaLow network interface: the esp_netif driver base, the
 * mmwlan STA/AP configuration the public API reads and writes, and (via the inherited
 * esp_netif_driver_base_t) the attached esp_netif handle itself.
 *
 * The shape of this class follows the pattern Espressif's own esp-idf-cxx library
 * (github.com/espressif/esp-idf-cxx) uses throughout for wrapping ESP-IDF driver state: a class
 * owns the C driver data directly, exposes it through named methods instead of public fields
 * poked at by free functions, and hands out the single valid instance through a static creation
 * method (idf-cxx's GPIOPullMode::PULLUP()/GPIOWakeupIntrType::LOW_LEVEL() are the same idiom for
 * enum-like values; instance() below is the equivalent for "there is exactly one of these").
 * What this class deliberately does *not* borrow from esp-idf-cxx is exception-based error
 * reporting (ESPException/CHECK_THROW): CONFIG_COMPILER_CXX_EXCEPTIONS is off in every example's
 * sdkconfig in this repo, and turning it on project-wide is a cross-cutting build change outside
 * the scope of a single component's internal rewrite. esp_err_t/enum mmwlan_status return codes
 * and assert() for "must never happen" invariants (matching this file's pre-existing style)
 * remain the error-reporting mechanism.
 *
 * There is exactly one instance for the lifetime of the application (mmhalow_init() may only be
 * called once), so this is a function-local static singleton (see instance()) rather than a
 * heap allocation.
 */
class HalowInterface
{
public:
    /** Returns the single HaLow interface instance, constructing it on first call. */
    static HalowInterface &instance()
    {
        static HalowInterface halow;
        return halow;
    }

    /** Attaches this driver to a freshly-created netif; triggers post_attach() below, which
     * completes esp_netif's side of the handshake. */
    esp_err_t attach(esp_netif_t *esp_netif) { return esp_netif_attach(esp_netif, this); }

    esp_netif_t *netif() const { return base_.netif; }

    /** These five accessors are only meaningful once attach() has run (mmhalow_init() does this
     * before any example can reach mmhalow_connect()/mmhalow_set_config()/etc.). The assert
     * reproduces the same "used before init" guard the previous esp_netif_get_io_driver()-based
     * active_driver() helper gave every one of its callers. */
    struct mmwlan_sta_args &sta_config()
    {
        assert(netif());
        return sta_args_;
    }
    void set_sta_config(const struct mmwlan_sta_args &conf)
    {
        assert(netif());
        sta_args_ = conf;
    }

    struct mmwlan_ap_args &ap_config()
    {
        assert(netif());
        return ap_args_;
    }
    void set_ap_config(const struct mmwlan_ap_args &conf)
    {
        assert(netif());
        ap_args_ = conf;
    }

    /** Copies just the SSID out of the STA config; see mmhalow_get_config()'s call site for why
     * the passphrase isn't included. */
    void copy_sta_ssid(struct mmwlan_sta_args &out) const
    {
        assert(netif());
        (void)strlcpy(reinterpret_cast<char *>(out.ssid),
                      reinterpret_cast<const char *>(sta_args_.ssid),
                      sizeof(out.ssid));
    }

    /** Hands a received packet up to the esp_netif stack. */
    void receive(struct mmpkt *rxpkt)
    {
        struct mmpktview *pktview = mmpkt_open(rxpkt);
        uint32_t data_len = mmpkt_get_data_length(pktview);
        esp_err_t ret =
            esp_netif_receive(netif(), mmpkt_get_data_start(pktview), data_len, pktview);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_netif_receive failed - %d", ret);
            mmpkt_close(&pktview);
            mmpkt_release(rxpkt);
        }
    }

    /** Reports a mmwlan link-state change to the esp_netif state machine. */
    void set_link_state(enum mmwlan_link_state link_state)
    {
        if (link_state == MMWLAN_LINK_DOWN)
        {
            ESP_LOGD(TAG, "Link down");
            esp_netif_action_disconnected(netif(), nullptr, 0, nullptr);
        }
        else
        {
            ESP_LOGD(TAG, "Link up");
            esp_netif_action_connected(netif(), nullptr, 0, nullptr);
        }
    }

    /** Reads the hardware MAC address, applies it to the netif, and starts it. */
    void start()
    {
        std::array<uint8_t, 6> mac{};

        mmwlan_get_mac_addr(mac.data());
        ESP_LOGI(TAG, "Wi-Fi MAC address: " MM_MAC_ADDR_FMT, MM_MAC_ADDR_VAL(mac));

        esp_netif_set_mac(netif(), mac.data());
        esp_netif_action_start(netif(), nullptr, 0, nullptr);
    }

private:
    /** Wires up post_attach() so esp_netif_attach() (see attach() above) can complete its
     * handshake; this is the only configuration the driver needs before attachment. */
    HalowInterface() { base_.post_attach = &HalowInterface::post_attach; }

    /** esp_netif's driver-attach callback. `args` is always the HalowInterface passed to
     * esp_netif_attach() in attach() above. */
    static esp_err_t post_attach(esp_netif_t *esp_netif, void *args)
    {
        auto *self = static_cast<HalowInterface *>(args);
        self->base_.netif = esp_netif;
        esp_netif_driver_ifconfig_t driver_ifconfig{
            .handle = self,
            .transmit = transmit,
            .transmit_wrap = transmit_wrap,
            .driver_free_rx_buffer = free_rx_buffer,
        };
        return esp_netif_set_driver_config(esp_netif, &driver_ifconfig);
    }

    static esp_err_t transmit(void * /*h*/, void *buffer, size_t len)
    {
        struct mmwlan_tx_metadata metadata{
            .tid = 0,
        };

        enum mmwlan_status status = mmwlan_tx_wait_until_ready(1000);
        if (status != MMWLAN_SUCCESS)
        {
            ESP_LOGE(TAG, "Transmit blocked: %d", status);
            return ESP_FAIL;
        }

        struct mmpkt *pkt = mmwlan_alloc_mmpkt_for_tx(len, metadata.tid);
        if (pkt == nullptr)
        {
            ESP_LOGE(TAG, "Failed to allocate packet for transmit.");
            return ESP_ERR_NO_MEM;
        }
        struct mmpktview *pktview = mmpkt_open(pkt);
        mmpkt_append_data(pktview, static_cast<const uint8_t *>(buffer), len);
        mmpkt_close(&pktview);

        status = mmwlan_tx_pkt(pkt, &metadata);
        if (status != MMWLAN_SUCCESS)
        {
            ESP_LOGE(TAG, "Packet failed to send - %d", status);
            return ESP_FAIL;
        }
        return ESP_OK;
    }

    static esp_err_t transmit_wrap(void *h, void *buffer, size_t len, void * /*netstack_buf*/)
    {
        return transmit(h, buffer, len);
    }

    static void free_rx_buffer(void * /*h*/, void *buffer)
    {
        auto *pktview = static_cast<struct mmpktview *>(buffer);
        struct mmpkt *rxpkt = mmpkt_from_view(pktview);
        mmpkt_close(&pktview);
        mmpkt_release(rxpkt);
    }

    esp_netif_driver_base_t base_{};
    struct mmwlan_sta_args sta_args_ = MMWLAN_STA_ARGS_INIT;
    struct mmwlan_ap_args ap_args_{};
};

} // namespace

void mmhalow_print_version_info(void)
{
    struct mmwlan_version version;
    struct mmwlan_bcf_metadata bcf_metadata;

    ESP_LOGI(TAG, "-----------------------------------");

    enum mmwlan_status status = mmwlan_get_bcf_metadata(&bcf_metadata);
    if (status == MMWLAN_SUCCESS)
    {
        ESP_LOGI(TAG,
                 "  BCF API version:         %u.%u.%u",
                 bcf_metadata.version.major,
                 bcf_metadata.version.minor,
                 bcf_metadata.version.patch);
        if (bcf_metadata.build_version[0] != '\0')
        {
            ESP_LOGI(TAG, "  BCF build version:       %s", bcf_metadata.build_version);
        }
        if (bcf_metadata.board_desc[0] != '\0')
        {
            ESP_LOGI(TAG, "  BCF board description:   %s", bcf_metadata.board_desc);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Error occured whilst retrieving BCF metadata - %d", status);
    }

    status = mmwlan_get_version(&version);
    if (status != MMWLAN_SUCCESS)
    {
        ESP_LOGE(TAG, "Error occured whilst retrieving version info - %d", status);
        ESP_LOGI(TAG, "-----------------------------------");
        return;
    }

    ESP_LOGI(TAG, "  HaLow version:           %s", HALOW_VERSION);
    ESP_LOGI(TAG, "  Morselib version:        %s", version.morselib_version);
    ESP_LOGI(TAG, "  Morse firmware version:  %s", version.morse_fw_version);
    ESP_LOGI(TAG, "  Morse chip ID:           0x%04lx", version.morse_chip_id);
    ESP_LOGI(TAG, "-----------------------------------");
}

esp_err_t mmhalow_init(const wifi_init_config_t *config)
{
    ESP_UNUSED(config);
    HalowInterface &halow = HalowInterface::instance();
    assert(halow.netif() == nullptr);

    /* Initialize Morse subsystems, note that they must be called in this order. */
    mmhal_init();
    mmwlan_init();

    const struct mmwlan_regulatory_db *db = get_regulatory_db();
    const struct mmwlan_s1g_channel_list *channel_list =
        mmwlan_lookup_regulatory_domain(db, CONFIG_HALOW_COUNTRY_CODE);
    ESP_LOGI(TAG, "Setting Channel List %s", CONFIG_HALOW_COUNTRY_CODE);
    mmwlan_set_channel_list(channel_list);

    /* Boot the WLAN interface so that we can retrieve the firmware version. */
    struct mmwlan_boot_args boot_args = MMWLAN_BOOT_ARGS_INIT;
    enum mmwlan_status boot_status = mmwlan_boot(&boot_args);
    if (boot_status != MMWLAN_SUCCESS)
    {
        /* There is no teardown path here, so treat this as fatal for the interface. */
        ESP_LOGE(TAG, "mmwlan_boot failed - %d, transceiver is not usable", boot_status);
        return ESP_FAIL;
    }
    mmhalow_print_version_info();
#if !CONFIG_HALOW_PS_MODE
    mmwlan_set_power_save_mode(MMWLAN_PS_DISABLED);
#endif /* CONFIG_HALOW_PS_MODE */

    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif);

    halow.attach(netif);

    enum mmwlan_status status = mmwlan_register_rx_pkt_cb(
        [](struct mmpkt *rxpkt, void *arg) { static_cast<HalowInterface *>(arg)->receive(rxpkt); },
        &halow);
    assert(status == MMWLAN_SUCCESS);

    status = mmwlan_register_link_state_cb(
        [](enum mmwlan_link_state link_state, void *arg) {
            static_cast<HalowInterface *>(arg)->set_link_state(link_state);
        },
        &halow);
    assert(status == MMWLAN_SUCCESS);

    halow.start();

    return status;
}

esp_err_t mmhalow_deinit(void)
{
    /* Shutdown wlan interface */
    return mmwlan_shutdown();
}

esp_err_t mmhalow_scan(struct mmhalow_scan_args *args)
{
    assert(args);
    struct mmwlan_scan_req scan_req = MMWLAN_SCAN_REQ_INIT;
    scan_req.scan_rx_cb = args->rx_cb;
    scan_req.scan_complete_cb = args->complete_cb;
    scan_req.scan_cb_arg = args->cb_arg;
    return mmwlan_scan_request(&scan_req);
}

esp_err_t mmhalow_set_config(wifi_interface_t interface, mmhalow_wifi_config_t *conf)
{
    assert(conf);
    HalowInterface &halow = HalowInterface::instance();
    switch (interface)
    {
        case WIFI_IF_STA:
            halow.set_sta_config(conf->sta);
            break;
        case WIFI_IF_AP:
            halow.set_ap_config(conf->ap);
            break;
        case WIFI_IF_NAN:
        default:
            return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_OK;
}

esp_err_t mmhalow_get_config(wifi_interface_t interface, mmhalow_wifi_config_t *conf)
{
    assert(conf);
    switch (interface)
    {
        case WIFI_IF_STA:
            HalowInterface::instance().copy_sta_ssid(conf->sta);

            /* We cannot safely copy password from sta_args.passphrase into conf->password
             * because: sta_args.passphrase == 100 bytes, conf->password == 64 bytes, GCC
             * complains about truncation. */
            break;
        case WIFI_IF_AP:
        case WIFI_IF_NAN:
        default:
            return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_OK;
}

esp_err_t mmhalow_connect(mmwlan_sta_status_cb_t cb)
{
    HalowInterface &halow = HalowInterface::instance();

    ESP_LOGI(TAG, "Attempting to connect to: %s", halow.sta_config().ssid);

    return mmwlan_sta_enable(&halow.sta_config(), cb);
}

esp_err_t mmhalow_disconnect()
{
    return mmwlan_sta_disable();
}

enum mmwlan_status mmhalow_status(void)
{
    /* mmwlan_get_sta_state() returns enum mmwlan_sta_state, not enum mmwlan_status. C silently
     * converted between the two unrelated enums; C++ does not. The cast below preserves the
     * pre-existing (likely unintended) numeric behaviour rather than changing this function's
     * public return type as part of an unrelated C++ migration. */
    return static_cast<enum mmwlan_status>(mmwlan_get_sta_state());
}

void mmhalow_wifi_start(void)
{
    mmwlan_ap_enable(&HalowInterface::instance().ap_config());
}
