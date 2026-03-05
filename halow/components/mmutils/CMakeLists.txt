# Copyright 2024-2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(src
    "mmutils_wlan.c")

set(inc
    ".")

add_library(mmutils STATIC ${src})
add_dependencies(mmutils morselib)
target_link_libraries(mmutils INTERFACE morselib)
target_include_directories(mmutils INTERFACE ${inc})
