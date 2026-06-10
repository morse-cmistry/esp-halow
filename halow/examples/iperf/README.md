<!--
Copyright 2026 Morse Micro

SPDX-License-Identifier: Apache-2.0
-->

# Iperf

## About
This sample is a half-and-half implementation of the ESP32 iperf and MM-IoT-SDK iperf tests.
We pull in the the iperf component from the registry and access it via the REPL.
We do not allow for Wi-Fi configuration by using the REPL. Instead you bake in your AP's
SSID and PSK using the KConfig parameters, similar to how MM-IoT-SDK uses config_store.
