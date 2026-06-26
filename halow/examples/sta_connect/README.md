<!--
Copyright 2026 Morse Micro

SPDX-License-Identifier: Apache-2.0
-->

# sta_connect

Connects to a Wi-Fi HaLow AP as a station using SAE security, then sends a single
UDP packet to a hard-coded peer (`192.168.12.1:12345`) to confirm connectivity.

---

## Running
1. Set the target for your ESP board, e.g. `idf.py set-target esp32c6`.
2. Set the Wi-FiHaLow country code (required). Run `idf.py menuconfig` and set
   `Wi-Fi HaLow Connection Manager → Country code to use for Wi-Fi HaLow`
   (`CONFIG_HALOW_COUNTRY_CODE`) to your two-letter region code.
3. Build, flash and monitor, layering the board profile for your board/hat if required. See
   [configs](../../configs/README.md) for the full `SDKCONFIG_DEFAULTS` usage.

## Configuration
Options under `HaLow sta_connect Example Configuration` (`idf.py menuconfig`):

| Option | Type | Default | Description |
| --- | --- | --- | --- |
| `CONFIG_WIFI_SSID` | string | `"MorseMicro"` | SSID of the Wi-Fi HaLow AP to join. |
| `CONFIG_WIFI_PSK` | string | `"12345678"` | Passphrase for the Wi-Fi HaLow AP. |
