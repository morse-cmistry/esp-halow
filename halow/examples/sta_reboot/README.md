<!--
Copyright 2026 Morse Micro

SPDX-License-Identifier: Apache-2.0
-->

# sta_reboot

Stress test that repeats a boot → connect → send UDP → deinit cycle (10 iterations).
Each iteration boots the HaLow chip, connects to the AP using SAE, sends a single UDP
packet to `192.168.12.1:12345`, then shuts the interface down. Used for validating
repeated power-up/teardown.

---

## Running
1. Set the target for your ESP board, e.g. `idf.py set-target esp32c6`.
2. Set the Wi-Fi HaLow country code (required). Run `idf.py menuconfig` and set
   `Wi-Fi HaLow Connection Manager → Country code to use for Wi-Fi HaLow`
   (`CONFIG_HALOW_COUNTRY_CODE`) to your two-letter region code.
3. Build, flash and monitor, layering the board profile for your board/hat if required. See
   [configs](../../configs/README.md) for the full `SDKCONFIG_DEFAULTS` usage.

## Configuration
Options under `HaLow sta_reboot Example Configuration` (`idf.py menuconfig`):

| Option | Type | Default | Description |
| --- | --- | --- | --- |
| `CONFIG_WIFI_SSID` | string | `"MorseMicro"` | SSID of the Wi-Fi HaLow AP to join. |
| `CONFIG_WIFI_PSK` | string | `"12345678"` | Passphrase for the Wi-Fi HaLow AP. |
