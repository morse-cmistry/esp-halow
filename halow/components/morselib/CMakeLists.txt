# Copyright 2025-2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(INC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/internal
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/rc/mmrc_osal
    ${CMAKE_CURRENT_SOURCE_DIR}/mmrc/src/core
)
set(SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/mmrc/src/core/mmrc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/mac_address.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/mmpkt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/mmpkt_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dot11/dot11_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/ap/umac_ap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/ate/umac_ate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/ba/umac_ba.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/connection/umac_connection.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/config/umac_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/core/umac_core.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/core/umac_evtloop.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/core/umac_evtq.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/core/umac_timeout.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/data/umac_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/data/umac_sta_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/datapath/datapath_defrag.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/datapath/umac_datapath.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/datapath/umac_datapath_ap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/frames/action.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/frames/association.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/frames/authentication.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/frames/deauthentication.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/frames/disassociation.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/frames/frame_constructor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/frames/probe_request.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/frames/probe_response.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/frames/utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/ies/aid_request.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/ies/twt_ie.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/ies/s1g_capabilities.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/ies/s1g_operation.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/ies/s1g_tim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/ies/ies_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/ies/wmm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/ies/morse_ie.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/interface/umac_interface.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/keys/connection_keys.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/keys/umac_keys.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/ps/umac_ps.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/rc/mmrc_osal/mmrc_osal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/rc/umac_rc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/regdb/umac_regdb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/scan/umac_scan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/scan/hw_scan.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/stats/umac_stats.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/supplicant_shim/bip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/supplicant_shim/ccmp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/supplicant_shim/config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/supplicant_shim/driver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/supplicant_shim/driver_ap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/supplicant_shim/eloop.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/supplicant_shim/morse_dpp_event.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/supplicant_shim/random.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/supplicant_shim/supplicant_core.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/supplicant_shim/supplicant_core_ap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/twt/umac_twt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/umac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/umac_mmdrv_shim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/wnm_sleep/umac_wnm_sleep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/umac/offload/umac_offload.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/mm6108/mm6108.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/mm6108/pager_if_hw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/mm6108/pager_if.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/mm6108/pageset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/mm8108/mm8108.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/mm8108/yaps-hw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/mm8108/yaps.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/beacon/beacon.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/driver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/driver_task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/health/driver_health.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/puff/puff.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_crc/morse_crc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/command.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/event.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/ext_host_table.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/firmware.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/firmware_mbin.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/hw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/ps.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/morse_driver/skbq.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/transport/sdio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/driver/transport/sdio_spi.c
)

add_library(libmorse STATIC ${SRCS} ${INC})
target_include_directories(libmorse PUBLIC ${INC})
add_dependencies(libmorse mmhostap)
target_link_libraries(libmorse PRIVATE mmutils mmhostap)

set(PROTECTED_SYMS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/tools/metadata/protected_syms.txt")
file(STRINGS "${PROTECTED_SYMS_FILE}" _raw_syms
     REGEX "^[^#].*")
list(TRANSFORM _raw_syms STRIP)
list(FILTER _raw_syms EXCLUDE REGEX "^$")

set(protected_syms)
foreach(sym IN LISTS _raw_syms)
    list(APPEND protected_syms -p "${sym}")
endforeach()

string(REGEX REPLACE "objcopy$" "" TOOLCHAIN_BASE "${CMAKE_OBJCOPY}")
set(AR "${TOOLCHAIN_BASE}ar")
set(RANLIB "${TOOLCHAIN_BASE}ranlib")
set(LIBMORSE  "$<TARGET_FILE:libmorse>")
set(LIBHOSTAP "$<TARGET_FILE:mmhostap>")
set(MANGLER "${CMAKE_CURRENT_SOURCE_DIR}/tools/buildsystem/librarymangler.py")
set(MERGESCRIPT "${CMAKE_CURRENT_BINARY_DIR}/libmorse.ar")
set(LIBMORSEMANG "${CMAKE_CURRENT_BINARY_DIR}/libmorse.a")

file(GENERATE
    OUTPUT "${MERGESCRIPT}"
    CONTENT
    "CREATE ${LIBMORSEMANG}
    ADDLIB ${LIBMORSE}
    ADDLIB ${LIBHOSTAP}
    SAVE
    END")

add_custom_command(
    OUTPUT ${LIBMORSEMANG}
    COMMAND ${CMAKE_COMMAND} -E echo "Creating mangled libmorse"
    COMMAND ${CMAKE_COMMAND} -E cat ${MERGESCRIPT} | ${AR} -M
    COMMAND ${RANLIB} ${LIBMORSEMANG}
    COMMAND ${MANGLER} -t ${TOOLCHAIN_BASE} -m ${CMAKE_CURRENT_BINARY_DIR} ${protected_syms} ${LIBMORSEMANG}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS libmorse mmhostap
)

add_custom_target(morselib_build ALL DEPENDS ${LIBMORSEMANG})
add_library(morselib STATIC IMPORTED GLOBAL)
set_target_properties(morselib PROPERTIES IMPORTED_LOCATION "${LIBMORSEMANG}")

set_property(TARGET morselib PROPERTY INTERFACE_LINK_LIBRARIES mmpktmem)

target_include_directories(morselib INTERFACE ${INC})
target_link_libraries(morselib INTERFACE shims
    mmpktmem)
