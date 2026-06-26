<!--
Copyright 2026 Morse Micro

SPDX-License-Identifier: Apache-2.0
-->

# porting_assistant

Self-test tool for bringing up a new platform. Runs a sequence of OSAL, HAL and
hardware tests (memory, timing, task creation, SDIO/SPI bus, chip ID, firmware/BCF
validation, throughput, BUSY pin) and reports the result of each. A known-good
platform should pass every step.

---

## Running
1. Set the target for your ESP board, e.g. `idf.py set-target esp32c6`.
2. Set the Wi-Fi HaLow country code (required). Run `idf.py menuconfig` and set
   `Wi-Fi HaLow Connection Manager → Country code to use for Wi-Fi HaLow`
   (`CONFIG_HALOW_COUNTRY_CODE`) to your two-letter region code.
3. Build, flash and monitor, layering the board profile for your board/hat if required. See
   [configs](../../configs/README.md) for the full `SDKCONFIG_DEFAULTS` usage.
