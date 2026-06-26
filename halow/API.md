# mmhalow API Reference

This document covers the public API exposed by `mmhalow.h`.  
For lower-level access to the Morse WLAN driver, see `components/mm-iot-sdk/framework/morselib/include/mmwlan.h`.

---

## Types

### `mmhalow_wifi_config_t`

Configuration union for STA and AP modes.

```c
typedef struct mmhalow_wifi_config_t {
    union {
        struct mmwlan_sta_args sta;
        struct mmwlan_ap_args  ap;
    };
} mmhalow_wifi_config_t;
```

Set the `sta` member when configuring `WIFI_IF_STA`, or the `ap` member when configuring `WIFI_IF_AP`. Initialise before use with `MMWLAN_STA_ARGS_INIT` or `MMWLAN_AP_ARGS_INIT` respectively.

**`struct mmwlan_sta_args` key fields** (defined in `mmwlan.h`):

| Field | Type | Description |
|-------|------|-------------|
| `ssid` | `uint8_t[33]` | Target AP SSID (null-terminated) |
| `passphrase` | `char[101]` | WPA3-SAE passphrase |
| `security_type` | `enum mmwlan_security_type` | `MMWLAN_OPEN`, `MMWLAN_OWE`, or `MMWLAN_SAE` |
| `pmf_mode` | `enum mmwlan_pmf_mode` | `MMWLAN_PMF_REQUIRED` or `MMWLAN_PMF_DISABLED` |

**`struct mmwlan_ap_args` key fields** (defined in `mmwlan.h`):

| Field | Type | Description |
|-------|------|-------------|
| `ssid` | `uint8_t[33]` | AP SSID to advertise |
| `passphrase` | `char[101]` | SAE passphrase |
| `security_type` | `enum mmwlan_security_type` | Security mode |
| `channel` | `uint8_t` | S1G channel number |
| `operating_class` | `int16_t` | S1G operating class |

---

### `mmhalow_scan_args`

Arguments for an AP scan request.

```c
struct mmhalow_scan_args {
    mmwlan_scan_rx_cb_t       rx_cb;       /* called for each AP found       */
    mmwlan_scan_complete_cb_t complete_cb; /* called when scan finishes      */
    void                     *cb_arg;      /* passed to both callbacks       */
};
```

Callback signatures (from `mmwlan.h`):

```c
typedef void (*mmwlan_scan_rx_cb_t)(const struct mmwlan_scan_result *result, void *arg);
typedef void (*mmwlan_scan_complete_cb_t)(enum mmwlan_status status, void *arg);
```

---

## Functions

### `mmhalow_init`

```c
esp_err_t mmhalow_init(const wifi_init_config_t *config);
```

Initialises the HaLow subsystem and creates the network interface.

- Calls `mmhal_init()` then `mmwlan_init()` internally.
- Looks up the regulatory channel list for `CONFIG_HALOW_COUNTRY_CODE`.
- Boots the transceiver firmware and logs version information.
    - **Note: CONFIG_HALOW_COUNTRY_CODE must be set to a valid region to boot**
- Disables power-save unless `CONFIG_HALOW_PS_MODE` is set.
- Creates an `esp_netif` in STA mode and attaches the Morse driver.

**Parameters:**  
`config` - Ignored; pass `WIFI_INIT_CONFIG_DEFAULT()` or `NULL` for compatibility.

**Returns:**   
`ESP_OK` on success.

**Constraints:**   
Must be called once. Asserts if called a second time.

---

### `mmhalow_deinit`

```c
esp_err_t mmhalow_deinit(void);
```

Shuts down the WLAN interface. Calls `mmwlan_shutdown()` internally.

**Returns:**   
`ESP_OK` on success.

---

### `mmhalow_set_config`

```c
esp_err_t mmhalow_set_config(wifi_interface_t interface, mmhalow_wifi_config_t *conf);
```

Stores the network configuration for the specified interface. Must be called before `mmhalow_connect()` (STA) or `mmhalow_wifi_start()` (AP).

**Parameters:**  
`interface` - `WIFI_IF_STA` or `WIFI_IF_AP`.  
`conf` - Pointer to a populated `mmhalow_wifi_config_t`.

**Returns:**  
`ESP_OK` on success.  
`ESP_ERR_NOT_SUPPORTED` for `WIFI_IF_NAN` or any unsupported interface.

---

### `mmhalow_get_config`

```c
esp_err_t mmhalow_get_config(wifi_interface_t interface, mmhalow_wifi_config_t *conf);
```

Retrieves the stored configuration for the specified interface.

**Parameters:**  
`interface` - `WIFI_IF_STA` only (AP retrieval not currently supported).  
`conf` - Output buffer.

**Returns:**. 
`ESP_OK` on success.  
`ESP_ERR_NOT_SUPPORTED` for `WIFI_IF_AP`, `WIFI_IF_NAN`, or unsupported interfaces.

**Note:** Only the `ssid` field is copied back; the passphrase is not exposed.

---

### `mmhalow_connect`

```c
esp_err_t mmhalow_connect(mmwlan_sta_status_cb_t cb);
```

Enables STA mode and begins connecting to the AP specified by a prior `mmhalow_set_config(WIFI_IF_STA, ...)` call. Wraps `mmwlan_sta_enable()`.

**Parameters:**  
`cb` - Optional status callback invoked on connection state changes. Pass `NULL` if not needed.

```c
typedef void (*mmwlan_sta_status_cb_t)(enum mmwlan_status status, void *arg);
```

**Returns:**  
`ESP_OK` if the connection request was accepted.

---

### `mmhalow_disconnect`

```c
esp_err_t mmhalow_disconnect(void);
```

Disconnects from the AP. Wraps `mmwlan_sta_disable()`.

**Returns:**  
`ESP_OK` on success. May return `MMWLAN_SHUTDOWN_BLOCKED` (cast to `esp_err_t`) if the transceiver could not be fully shut down immediately.

---

### `mmhalow_status`

```c
enum mmwlan_status mmhalow_status(void);
```

Returns the current STA connection state. Wraps `mmwlan_get_sta_state()`.

**Returns:**  One of:

| Value | Meaning |
|-------|---------|
| `MMWLAN_SUCCESS` | Connected |
| `MMWLAN_UNAVAILABLE` | Not connected or not initialised |
| other | Error or intermediate state |

---

### `mmhalow_scan`

```c
esp_err_t mmhalow_scan(struct mmhalow_scan_args *args);
```

Initiates an AP scan. Results are delivered asynchronously via the callbacks in `args`.

**Parameters:**  
`args` - Scan parameters. Must not be `NULL`. The pointer need not remain valid after the call returns; the fields are copied internally.

**Returns:**  
`ESP_OK` if the scan was started successfully.

---

### `mmhalow_wifi_start`

```c
void mmhalow_wifi_start(void);
```

Enables AP mode using the configuration previously set with `mmhalow_set_config(WIFI_IF_AP, ...)`. Wraps `mmwlan_ap_enable()`.

---

### `mmhalow_print_version_info`

```c
void mmhalow_print_version_info(void);
```

Logs BCF metadata, morselib version, firmware version, and chip ID via `ESP_LOGI`. Called automatically by `mmhalow_init()`; can be called again at any point after init.

Example output:

```
I (123) Morse Micro HaLow NetIF: -----------------------------------
I (124) Morse Micro HaLow NetIF:   BCF API version:         1.2.0
I (125) Morse Micro HaLow NetIF:   BCF board description:   MM-EVK-C3
I (126) Morse Micro HaLow NetIF:   Morselib version:        2.10.4
I (127) Morse Micro HaLow NetIF:   Morse firmware version:  2.10.4-esp32-1
I (128) Morse Micro HaLow NetIF:   Morse chip ID:           0x6108
I (129) Morse Micro HaLow NetIF: -----------------------------------
```

---

## Enumerations

### `mmwlan_status` (from `mmwlan.h`)

Returned by lower-level functions and `mmhalow_status()`.

| Value | Description |
|-------|-------------|
| `MMWLAN_SUCCESS` | Operation succeeded |
| `MMWLAN_ERROR` | Unspecified failure |
| `MMWLAN_INVALID_ARGUMENT` | Bad parameter |
| `MMWLAN_UNAVAILABLE` | Temporarily unavailable |
| `MMWLAN_CHANNEL_LIST_NOT_SET` | Call `mmwlan_set_channel_list()` first |
| `MMWLAN_NO_MEM` | Memory allocation failure |
| `MMWLAN_TIMED_OUT` | Operation timed out |
| `MMWLAN_SHUTDOWN_BLOCKED` | STA disable did not shut down transceiver |
| `MMWLAN_CHANNEL_INVALID` | Channel not in regulatory database |
| `MMWLAN_NOT_FOUND` | Resource not found |
| `MMWLAN_NOT_RUNNING` | Device not booted |
| `MMWLAN_NOT_INITIALIZED` | `mmwlan_init()` not called |
| `MMWLAN_VIF_ERROR` | VIF not active |
| `MMWLAN_NOT_SUPPORTED` | Feature not supported in this build |
| `MMWLAN_COMMAND_ERROR` | Chip returned a command error |

### `mmwlan_security_type` (from `mmwlan.h`)

| Value | Description |
|-------|-------------|
| `MMWLAN_OPEN` | No security |
| `MMWLAN_OWE` | Opportunistic Wireless Encryption |
| `MMWLAN_SAE` | WPA3 Simultaneous Authentication of Equals |

### `mmwlan_pmf_mode` (from `mmwlan.h`)

| Value | Description |
|-------|-------------|
| `MMWLAN_PMF_REQUIRED` | Protected Management Frames mandatory |
| `MMWLAN_PMF_DISABLED` | Protected Management Frames off |

---

## Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `MMWLAN_SSID_MAXLEN` | 32 | Maximum SSID length (bytes) |
| `MMWLAN_PASSPHRASE_MAXLEN` | 100 | Maximum passphrase length (bytes) |
| `MMWLAN_MAC_ADDR_LEN` | 6 | MAC address length (bytes) |
| `MMWLAN_COUNTRY_CODE_LEN` | 3 | Country code string length including null terminator |

---

## Lower-Level API

The following headers are available from `mm-iot-sdk` for advanced use cases. They are not required for typical applications.

| Header | Purpose |
|--------|---------|
| `mmwlan.h` | Full WLAN control API: scan, connect, TX/RX, TWT, rate control |
| `mmhal_wlan.h` | Hardware abstraction for the transceiver (SPI/SDIO, firmware load, BCF) |
| `mmpkt.h` | Packet buffer management (open, append, prepend, release) |
| `mmosal.h` | OS abstraction: tasks, semaphores, mutexes, queues, timers |
| `mmwlan_stats.h` | UMAC statistics: RX/TX queue depths, reorder buffer, connection timestamps |
| `mmlog.h` | Logging macros (`MM_APP`, `MM_DBG`, `MM_INFO`, `MM_WRN`, `MM_ERR`) |
| `mmversion.h` | SDK version macros and version comparison helpers |
