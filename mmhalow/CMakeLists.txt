# Copyright 2025 Robert Carey Copyright 2025 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(src "mmhalow.c")

set(inc ".")

idf_component_register(
	INCLUDE_DIRS
	${inc}
	SRCS
	${src}
	REQUIRES
	mmutils
	morselib
	shims
	regdb
	esp_netif
	PRIV_REQUIRES
	esp_wifi
)
