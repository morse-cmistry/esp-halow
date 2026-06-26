<!--
Copyright 2026 Morse Micro

SPDX-License-Identifier: Apache-2.0
-->

# iperf

An adaption of ESP32 iperf REPL to test Wi-Fi HaLow throughput.
On boot the device connects as a station, then exposes the `iperf`
command for UDP/TCP RX/TX throughput tests via the serial moitor.

---

## Running
1. Set the target for your ESP board, e.g. `idf.py set-target esp32c6`.
2. Set the Wi-Fi HaLow country code (required). Run `idf.py menuconfig` and set
   `Wi-Fi HaLow Connection Manager → Country code to use for Wi-Fi HaLow`
   (`CONFIG_HALOW_COUNTRY_CODE`) to your two-letter region code.
3. Build, flash and monitor, layering the board profile for your board/hat if required. See
   [configs](../../configs/README.md) for the full `SDKCONFIG_DEFAULTS` usage.
4. At the `iperf>` prompt, run `help` for the command list, then `iperf` to test throughput.

## Configuration
Options under `HaLow iperf Example Configuration` (`idf.py menuconfig`):

| Option | Type | Default | Description |
| --- | --- | --- | --- |
| `CONFIG_WIFI_SSID` | string | `"MorseMicro"` | SSID of the Wi-Fi HaLow AP to join. |
| `CONFIG_WIFI_PSK` | string | `"12345678"` | Passphrase for the Wi-Fi HaLow AP. |
