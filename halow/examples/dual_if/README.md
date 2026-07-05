<!--
Copyright 2026 Morse Micro

SPDX-License-Identifier: Apache-2.0
-->

# dual_if

Runs two interfaces at once: the ESP's native Wi-Fi as a SoftAP hosting a small HTTP
server, and the Wi-Fi HaLow interface as a station. After the Wi-Fi HaLow link is up it
periodically sends UDP packets to a hard-coded peer (`192.168.12.1:12345`).

This example uses a custom partition format as the coexistence of two Wi-Fi stacks is
too large for the defaults.

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
Flash and open the serial monitor to watch both interfaces come up:

    idf.py flash monitor

Once the Wi-Fi HaLow link is up, connect to the ESP's native SoftAP to reach the HTTP
server, and watch for the periodic UDP packets sent to `192.168.12.1:12345`.

The ESP AP should be accessible from 2.4GHz Wi-Fi via the netwrok `SSID = ESP32-HELLO` with the pasword of `PSK = hello1234`.

## Configuration
Options under `HaLow dual_if Example Configuration` (`idf.py menuconfig`):

| Option | Type | Default | Description |
| --- | --- | --- | --- |
| `CONFIG_HALOW_SSID` | string | `"MorseMicro"` | SSID of the Wi-Fi HaLow AP to join. |
| `CONFIG_HALOW_PSK` | string | `"12345678"` | Passphrase for the Wi-Fi HaLow AP. |
