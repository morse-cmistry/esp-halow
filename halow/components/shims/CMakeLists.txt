# Copyright 2024-2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(src "mmosal_shim_freertos_esp32.c"
	"mmhal_core.c"
	"mmhal_os.c"
	"mmhal_wlan_binaries.c"
	"mmhal_wlan.c"
	"crypto_mbedtls_mm.c"
)

idf_build_get_property(ARCH IDF_TARGET_ARCH)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/${ARCH}")
    message(WARN "Architecture ${ARCH} missing morselib debug macro mappings. Stubbing with noops.")
    set(inc "include/unknown" ".")
else()
    set(inc "include/${ARCH}" ".")
endif()

foreach(pin MM_RESET_N MM_BUSY MM_SPI_SCK MM_SPI_MOSI MM_SPI_MISO MM_SPI_CS)
    if(CONFIG_${pin} LESS 0)
        list(APPEND _mm_unset_pins ${pin})
    endif()
endforeach()
if(_mm_unset_pins)
    string(REPLACE ";" " " _mm_unset_pins_str "${_mm_unset_pins}")
    # FATAL_ERROR here blocks set-target and menuconfig
    add_custom_target(mm_pin_config_error ALL
        COMMAND ${CMAKE_COMMAND} -E echo
            "ERROR: the following MM pins must be set to a valid pin number: ${_mm_unset_pins_str}. Set them via menuconfig or use a sdkconfig.defaults found in managed_components/morsemicro__halow/configs"
        COMMAND ${CMAKE_COMMAND} -E false
    )
endif()

add_library(shims STATIC ${src})
target_include_directories(shims PUBLIC ${inc})

if(CONFIG_MM_CHIP_MM6108)
	target_compile_definitions(shims PRIVATE CONFIG_MMHAL_CHIP_TYPE_MM6108=1)
elseif(CONFIG_MM_CHIP_MM8108)
	target_compile_definitions(shims PRIVATE CONFIG_MMHAL_CHIP_TYPE_MM8108=1)
endif()

target_link_libraries(shims PUBLIC
	idf::log
	idf::driver
	idf::mbedtls
	idf::esp_timer
)

target_link_libraries(shims PUBLIC
	morsefirmware
	morselib
	mmutils
)

add_dependencies(shims morselib morsefirmware)

