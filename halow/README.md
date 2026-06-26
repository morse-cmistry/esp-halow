# mmhalow - Morse Micro Wi-Fi HaLow ESP32 Component

| [💬 Community Forum](https://community.morsemicro.com/latest) |
[🌐 Website](https://www.morsemicro.com) |
[📦 Repositories](https://github.com/morsemicro) |

An ESP-IDF component that provides Wi-Fi HaLow (802.11ah) connectivity for ESP32 microcontrollers using Morse Micro wireless transceivers.

**Version:** 2.10.4-esp32-1  
**License:** Apache-2.0  
**ESP-IDF requirement:** >= 5.4.2  

---

## Overview

Wi-Fi HaLow (IEEE 802.11ah) operates in the sub-1 GHz band, offering longer range and better obstacle penetration than standard Wi-Fi, at the cost of lower throughput. It is well suited for IoT applications.

This component wraps the Morse Micro IoT SDK (`mm-iot-sdk`) and exposes a concise API modelled on ESP-IDF's `esp_wifi` interface so it integrates naturally into existing ESP-IDF projects.

---

## Supported Hardware

| MCU | Transceiver |
|-----|-------------|
| ESP32-C3 | MM6108 |
| ESP32-C5 | MM6108 |
| ESP32-C6 | MM6108 |
| ESP32-S3 | MM6108 |
| ESP32-P4 | MM6108, MM8108 |

---

## Getting Started

### 1. Add the component

In your project's `idf_component.yml`:

```yaml
dependencies:
  morsemicro/mmhalow: "==2.11.2-esp32-1"
```

Or clone the repository and add `halow` as a local component:

```bash
git clone https://github.com/MorseMicro/esp-halow.git
```

Then add the path to your project's `CMakeLists.txt`:

```cmake
set(EXTRA_COMPONENT_DIRS /path/to/esp-halow/halow)
```

### 2. Configure

Run `idf.py menuconfig` and navigate to **Wi-Fi HaLow Connection Manager**:

| Option | Default | Description |
|--------|---------|-------------|
| `WIFI_HALOW` | `y` | Enable HaLow support |
| `HALOW_COUNTRY_CODE` | `??` | Two-character country code (e.g. `US`, `AU`, `JP`) |
| `HALOW_PS_MODE` | `n` | Enable power-save mode (requires BUSY/WAKE pins) |

### 3. Typical STA usage

```c
#include "mmhalow.h"

// 1. Initialize
ESP_ERROR_CHECK(esp_netif_init());
ESP_ERROR_CHECK(esp_event_loop_create_default());
ESP_ERROR_CHECK(mmhalow_init(WIFI_INIT_CONFIG_DEFAULT()));

// 2. Set credentials
mmhalow_wifi_config_t cfg = {
    .sta = MMWLAN_STA_ARGS_INIT,
};
strlcpy((char *)cfg.sta.ssid, "my-ssid", sizeof(cfg.sta.ssid));
strlcpy((char *)cfg.sta.passphrase, "my-passphrase", sizeof(cfg.sta.passphrase));
cfg.sta.security_type = MMWLAN_SAE;
mmhalow_set_config(WIFI_IF_STA, &cfg);

// 3. Connect
mmhalow_connect(NULL);   // pass a status callback or NULL
```

### 4. Typical AP usage

```c
#include "mmhalow.h"

ESP_ERROR_CHECK(mmhalow_init(WIFI_INIT_CONFIG_DEFAULT()));

mmhalow_wifi_config_t cfg = {
    .ap = MMWLAN_AP_ARGS_INIT,
};
strlcpy((char *)cfg.ap.ssid, "my-halow-ap", sizeof(cfg.ap.ssid));
cfg.ap.security_type = MMWLAN_SAE;
mmhalow_set_config(WIFI_IF_AP, &cfg);
mmhalow_wifi_start();
```

---

## Examples

All examples are under `examples/`. Each is a self-contained ESP-IDF project.

| Example | Description |
|---------|-------------|
| `sta_connect` | Connect to an AP in STA mode, send a UDP packet |
| `scan` | Scan for HaLow APs and display results |
| `softap` | Start a HaLow access point |
| `iperf` | Interactive throughput testing via console REPL |
| `dual_if` | Run STA and AP interfaces simultaneously |
| `sta_reboot` | Reconnect gracefully after a reboot |
| `porting_assistant` | Hardware diagnostics for new board bring-up |

Build and flash an example:

```bash
cd examples/sta_connect
SDKCONFIG_DEFAULTS="sdkconfig.defaults;../../configs/sdkconfig.defaults.seeed_xiao_esp32c6-seeed_xiao_mm6108" \
    idf.py set-target esp32c6
idf.py menuconfig build  # set country code, SSID, passphrase
idf.py flash monitor
```

---

## Component Structure

```
halow/
├── mmhalow.c            # Component implementation
├── mmhalow.h            # Public API
├── CMakeLists.txt
├── idf_component.yml
├── Kconfig
├── components/
│   ├── firmware/        # Morse Micro Firmware and BCF binaries
|   ├── mm-iot-sdk/      # Morse Micro IoT SDK (firmware, morselib headers)
│   ├── morselib/        # Compiled Morse WLAN driver (static library)
│   ├── hostap/          # WPA Supplicant / Hostapd (SAE, OWE, CCMP)
│   ├── shims/           # Architecture HAL (RISCV/XTENSA)
│   ├── mmpktmem/        # Packet memory pool
│   ├── mmutils/         # WLAN helper utilities
│   └── regdb/           # Regulatory channel database
└── examples/
```

---
