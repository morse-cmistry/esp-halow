# Copyright 2025-2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

set(inc "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(libmorse STATIC IMPORTED GLOBAL)

if(${IDF_TARGET} STREQUAL "esp32s3")
    set_target_properties(libmorse PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libmorse_nosupplicant.a")
elseif(${IDF_TARGET} STREQUAL "esp32c6")
    set_target_properties(libmorse PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32c6/libmorse_nosupplicant.a")
elseif(${IDF_TARGET} STREQUAL "esp32c3")
    set_target_properties(libmorse PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32c3/libmorse_nosupplicant.a")
else()
	message(FATAL_ERROR "ESP-IDF target ${IDF_TARGET} not supported, exiting.")
endif()

set_target_properties(libmorse PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")

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

target_include_directories(morselib INTERFACE ${inc})
target_link_libraries(morselib INTERFACE shims
    mmpktmem)
