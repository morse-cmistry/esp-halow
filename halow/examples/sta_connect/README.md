<!--
Copyright 2026 Morse Micro

SPDX-License-Identifier: Apache-2.0
-->

# sta_connect

Connects to a Wi-Fi HaLow AP as a station using SAE security, then sends a single
UDP packet to a hard-coded peer (`192.168.12.1:12345`) to confirm connectivity.

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
Flash and open the serial monitor to watch the station connect and send its UDP packet:

    idf.py flash monitor

## Configuration
Options under `HaLow sta_connect Example Configuration` (`idf.py menuconfig`):

| Option | Type | Default | Description |
| --- | --- | --- | --- |
| `CONFIG_WIFI_SSID` | string | `"MorseMicro"` | SSID of the Wi-Fi HaLow AP to join. |
| `CONFIG_WIFI_PSK` | string | `"12345678"` | Passphrase for the Wi-Fi HaLow AP. |
