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

## Running
1. Set the target for your ESP board, e.g. `idf.py set-target esp32c6`.
2. Set the Wi-Fi HaLow country code (required). Run `idf.py menuconfig` and set
   `Wi-Fi HaLow Connection Manager → Country code to use for Wi-Fi HaLow`
   (`CONFIG_HALOW_COUNTRY_CODE`) to your two-letter region code.
3. Build, flash and monitor, layering the board profile for your board/hat if required. See
   [configs](../../configs/README.md) for the full `SDKCONFIG_DEFAULTS` usage.

## Configuration
Options under `HaLow dual_if Example Configuration` (`idf.py menuconfig`):

| Option | Type | Default | Description |
| --- | --- | --- | --- |
| `CONFIG_HALOW_SSID` | string | `"MorseMicro"` | SSID of the Wi-Fi HaLow AP to join. |
| `CONFIG_HALOW_PSK` | string | `"12345678"` | Passphrase for the Wi-Fi HaLow AP. |
