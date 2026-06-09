# Copyright 2024-2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(src "mmosal_shim_freertos_esp32.c"
	"mmhal_core.c"
	"mmhal_os.c"
	"mmhal_wlan_binaries.c"
	"mmhal_wlan.c"
	"crypto_mbedtls_mm.c"
)

# Define architecture specific paths and variables
if(${IDF_TARGET} STREQUAL "esp32s3")
	set(inc "include/esp32s3" ".")
elseif(${IDF_TARGET} STREQUAL "esp32c6")
	set(inc "include/esp32c6" ".")
elseif(${IDF_TARGET} STREQUAL "esp32c3")
	set(inc "include/esp32c3" ".")
else()
	message(FATAL_ERROR "Target ${IDF_TARGET} not known. Exiting...")
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
	${MM_FIRMWARE_LIB}
	idf::log
	idf::driver
	idf::mbedtls
	idf::esp_timer
)

target_link_libraries(shims PUBLIC
	morselib
	mmutils
)

add_dependencies(shims morselib ${MM_FIRMWARE_LIB})

