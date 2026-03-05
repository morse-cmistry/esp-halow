# Copyright 2025-2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(inc "include")

add_library(morselib STATIC IMPORTED GLOBAL)

if(${IDF_TARGET} STREQUAL "esp32s3")
    set_target_properties(morselib PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libmorse_nocrypto.a")
elseif(${IDF_TARGET} STREQUAL "esp32c6")
    set_target_properties(morselib PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32c6/libmorse_nocrypto.a")
elseif(${IDF_TARGET} STREQUAL "esp32c3")
    set_target_properties(morselib PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32c3/libmorse_nocrypto.a")
else()
	message(FATAL_ERROR "ESP-IDF target ${IDF_TARGET} not supported, exiting.")
endif()

set_target_properties(morselib PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(morselib INTERFACE shims mmpktmem)
