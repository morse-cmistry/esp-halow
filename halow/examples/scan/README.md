<!--
Copyright 2026 Morse Micro

SPDX-License-Identifier: Apache-2.0
-->

# scan

Initialises the Wi-Fi HaLow interface and performs a single shot scan, printing each
discovered AP's details.

---

## Running
1. Set the target for your ESP board, e.g. `idf.py set-target esp32c6`.
2. Set the Wi-Fi HaLow country code (required). Run `idf.py menuconfig` and set
   `Wi-Fi HaLow Connection Manager → Country code to use for Wi-Fi HaLow`
   (`CONFIG_HALOW_COUNTRY_CODE`) to your two-letter region code.
3. Build, flash and monitor, layering the board profile for your board/hat if required. See
   [configs](../../configs/README.md) for the full `SDKCONFIG_DEFAULTS` usage.
