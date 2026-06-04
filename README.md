# ESP-Halow

This component adds Morse Micro HaLow support to ESP32 boards.

## Tree
```
├── firmware
└── halow
```
### firmware
Contains the firmware used by Morse Micro boards.

### halow
Implements an integration layer to present the Morse Micro Wi-Fi HaLow driver libraries, as an [ESP-NETIF](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_netif.html).

### halow/configs
Contains sdkconfig.defaults for confguring hardware specific settings for common board/hat combinations.

These are intended for use with [SDKCONFIG_DEFAULTS](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/build-system.html#target-dependent-sdkconfig-defaults). Example usage:

```
idf.py set target SDKCONFIG_DEFAULTS="sdkconfig.defaults;managed_components/morsemicro__halow/configs/sdkconfig.defaults.seeed_xiao_esp32c3-seeed_xiao_mm6108"
```

### halow/components
The `mmpktmem`, `mmutils` , `morselib` and `regdb` directories contain components which are brought from [MM-IoT-SDK](https://github.com/MorseMicro/mm-iot-sdk/).

### halow/components/shims
Contains ESP32 board-specific shims for interacting with the hardware.

## Known Issues
* `PARTITION_TABLE_SINGLE_APP_LARGE` must be set by applications using these libraries.
