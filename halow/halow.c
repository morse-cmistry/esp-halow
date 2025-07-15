#include "esp_netif.h"
#include "esp_log.h"

#include "mmwlan.h"
#include "mmpkt.h"

#include "halow.h"

static const char *TAG = "HaLow netif";

// This is how espressif do it but I think we can do it by reference
static esp_netif_t *halow_netif = NULL;

void halow_link_state(enum mmwlan_link_state link_state, void *arg)
{
    esp_netif_t *esp_netif = (esp_netif_t *)arg;

    if (link_state == MMWLAN_LINK_DOWN)
    {
        ESP_LOGI(TAG, "Link down\n");
        esp_netif_action_disconnected(esp_netif, NULL, 0, NULL);
    }
    else
    {
        ESP_LOGI(TAG, "Link up\n");
        // esp_netif_action_connected(s_wifi_netifs[WIFI_IF_STA], base, event_id, data);
        esp_netif_action_connected(esp_netif, NULL, 0, NULL);
    }
}

static void halow_free(void *h, void* buffer)
{
    struct mmpktview *pktview = (struct mmpktview *)buffer;
    struct mmpkt *rxpkt = mmpkt_from_view(pktview);
    mmpkt_close(&pktview);
    mmpkt_release(rxpkt);
    // if (buffer)
    // {
    //     mmosal_free(buffer);
    // }
}

// esp_err_t esp_netif_receive(esp_netif_t *esp_netif, void *buffer, size_t len, void *eb)
void halow_rx(struct mmpkt *rxpkt, void *arg)
{
    // ESP_LOGI(TAG, "RX data %lu\n", rxpkt);
    esp_netif_t *esp_netif = (esp_netif_t *)arg;
    assert(esp_netif);

    struct mmpktview *pktview = mmpkt_open(rxpkt);
    uint32_t data_len = mmpkt_get_data_length(pktview);
    ESP_LOGI(TAG, "RX data %lu\n", data_len);

    // TODO investigate if we can do zero copy, it does not appear so at first glance. Make a
    // comment explaining this. Look into esp_wifi_internal_reg_netstack_buf_cb
    // void *buf_copy = mmosal_malloc(data_len);
    // if (!buf_copy) {
    //     goto exit;
    // }
    // memcpy(buf_copy, mmpkt_get_data_start(pktview), data_len);
    // todo handle failed case
    // Confusingly the *eb (last) param appear to be the buffer ref returned in halow_free
    esp_err_t ret = esp_netif_receive(halow_netif, mmpkt_get_data_start(pktview), data_len, pktview);

    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "esp_netif_rx input error\n");
        mmpkt_close(&pktview);
        mmpkt_release(rxpkt);
    }
    else
    {
        ESP_LOGI(TAG, "rx success\n");
    }
}


// static esp_err_t wifi_transmit(void *h, void *buffer, size_t len)
// {
//     wifi_netif_driver_t driver = h;
//     return esp_wifi_internal_tx(driver->wifi_if, buffer, len);
// }
static esp_err_t halow_transmit(void *h, void *buffer, size_t len)
{
    halow_netif_driver_t *driver = (halow_netif_driver_t *)h;

    struct mmpkt *pkt;
    struct mmpktview *pktview;
    enum mmwlan_status status;
    struct mmwlan_tx_metadata metadata = {
        // .tid = get_netif_state(netif)->tx_qos_tid,
        .tid = 0,
    };

    status = mmwlan_tx_wait_until_ready(1000);
    if (status != MMWLAN_SUCCESS)
    {
        ESP_LOGI(TAG, "Transmit blocked\n");
        return ESP_FAIL;
    }

    pkt = mmwlan_alloc_mmpkt_for_tx(len, metadata.tid);
    if (pkt == NULL)
    {
        ESP_LOGI(TAG, "TX pkt alloc failed\n");
        return ESP_ERR_NO_MEM;
    }
    pktview = mmpkt_open(pkt);
    mmpkt_append_data(pktview, (const uint8_t *)buffer, len);
    mmpkt_close(&pktview);

    status = mmwlan_tx_pkt(pkt, &metadata);
    if (status != MMWLAN_SUCCESS)
    {
        ESP_LOGI(TAG, "Error sending pkt\n");
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "Pkt sent\n");
    return ESP_OK;
}

static esp_err_t halow_transmit_wrap(void *h, void *buffer, size_t len, void *netstack_buf)
{
    return halow_transmit(h, buffer, len);
}



static esp_err_t halow_driver_start(esp_netif_t * esp_netif, void * args)
{
    halow_netif_driver_t *driver = (halow_netif_driver_t *)args;
    driver->base.netif = esp_netif;
    esp_netif_driver_ifconfig_t driver_ifconfig = {
        .handle =  driver,
        .transmit = halow_transmit,
        .transmit_wrap = halow_transmit_wrap,
        .driver_free_rx_buffer = halow_free
    };
    halow_netif = esp_netif;
    return esp_netif_set_driver_config(esp_netif, &driver_ifconfig);
}


halow_netif_driver_t *esp_halow_create_if_driver(void)
{
    halow_netif_driver_t *driver = calloc(1, sizeof(struct halow_netif_driver));
    if (driver == NULL) {
        ESP_LOGE(TAG, "No memory to create a wifi interface handle");
        return NULL;
    }
    driver->base.post_attach = halow_driver_start;
    return driver;
}
