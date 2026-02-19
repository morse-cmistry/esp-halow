# Copyright 2024 Morse Micro
# SPDX-License-Identifier: Apache-2.0
set(inc
    ".")
set(src
    "mmbuf.c")
set(src
    "mmutils_wlan.c")

idf_component_register(INCLUDE_DIRS ${inc}
                       SRCS ${src}
                       PRIV_REQUIRES morselib shims)
