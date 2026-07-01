<!--
Copyright 2026 Morse Micro

SPDX-License-Identifier: Apache-2.0
-->

# scan

Initialises the Wi-Fi HaLow interface and performs a single shot scan, printing each
discovered AP's details.

---

## Building
Set the target, layering in the board profile for your board/hat, then configure and
build. See [configs](../../configs/README.md) for the full `SDKCONFIG_DEFAULTS` usage.

Setting the Wi-Fi HaLow country code is required — in `menuconfig` it is under
`Wi-Fi HaLow Connection Manager → Country code to use for Wi-Fi HaLow`
(`CONFIG_HALOW_COUNTRY_CODE`).

For example, for a Seeed Studio XIAO ESP32-S3 paired with a XIAO Wi-Fi HaLow
(MM6108) hat:

    SDKCONFIG_DEFAULTS="sdkconfig.defaults;managed_components/morsemicro__halow/configs/sdkconfig.defaults.seeed_xiao_esp32s3-seeed_xiao_mm6108" \
        idf.py set-target esp32s3
    idf.py menuconfig build   # set the country code (required)

## Running
Flash and open the serial monitor to view the discovered APs:

    idf.py flash monitor
