# Copyright 2025-2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(inc ".")
set(src "regdb.c")

add_library(regdb STATIC ${src})
target_include_directories(regdb PUBLIC ${inc})
add_dependencies(regdb shims morselib)
target_link_libraries(regdb PUBLIC shims morselib)
