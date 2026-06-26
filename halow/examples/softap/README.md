<!--
Copyright 2026 Morse Micro

SPDX-License-Identifier: Apache-2.0
-->

# softap

Starts a Wi-Fi HaLow SoftAP on a configurable S1G channel. Uses SAE security when a
passphrase is set. PMF is required and a single station is allowed.

---

## Running
1. Set the target for your ESP board, e.g. `idf.py set-target esp32c6`.
2. Set the Wi-Fi HaLow country code (required). Run `idf.py menuconfig` and set
   `Wi-Fi HaLow Connection Manager → Country code to use for Wi-Fi HaLow`
   (`CONFIG_HALOW_COUNTRY_CODE`) to your two-letter region code.
3. Build, flash and monitor, layering the board profile for your board/hat if required. See
   [configs](../../configs/README.md) for the full `SDKCONFIG_DEFAULTS` usage.

## Configuration
Options under `HaLow softap Example Configuration` (`idf.py menuconfig`):

| Option | Type | Default | Description |
| --- | --- | --- | --- |
| `CONFIG_WIFI_SSID` | string | `"MorseMicroESP32AP"` | SSID to advertise. |
| `CONFIG_WIFI_PSK` | string | `"12345678"` | Passphrase; leave empty for an open AP. |
| `CONFIG_S1G_CHANNEL` | int | `27` | S1G channel number to operate on. |
| `CONFIG_S1G_OPCLASS` | int | `68` | S1G operating class. |

The channel and operating class must be valid for the configured country code.
