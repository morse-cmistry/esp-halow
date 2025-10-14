
#include "esp_netif.h"

typedef struct halow_netif_driver {
    esp_netif_driver_base_t base;
    // TODO, do we want extra info here, esp just has an enum with interface thalow_netif_driver_type
} halow_netif_driver_t;

halow_netif_driver_t *esp_halow_create_if_driver(void);

void halow_rx(struct mmpkt *rxpkt, void *arg);
void halow_link_state(enum mmwlan_link_state link_state, void *arg);

