# Copyright 2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

if(NOT (CONFIG_MM_CHIP_MM6108 OR CONFIG_MM_CHIP_MM8108) OR
   NOT CONFIG_MM_BCF_FILE)
    # lib def and no FATAL_ERROR to prevent blocking set-target and menuconfig
    add_library(morsefirmware INTERFACE)
    add_custom_target(mm_fw_config_error ALL
        COMMAND ${CMAKE_COMMAND} -E echo
            "ERROR: CONFIG_MM_CHIP and CONFIG_MM_BCF_FILE must be set. Set manually via menuconfig or use a sdkconfig.defaults found in managed_components/morsemicro__halow/configs"
        COMMAND ${CMAKE_COMMAND} -E false
    )
    return()
endif()

set(BCF_SEARCH_ROOT "${CMAKE_CURRENT_LIST_DIR}/morse-firmware/bcf")
set(FW_SEARCH_ROOT  "${CMAKE_CURRENT_LIST_DIR}/morse-firmware/firmware")

if(CONFIG_MM_CHIP_MM6108)
    set(FW_FILE "mm6108.bin")
elseif(CONFIG_MM_CHIP_MM8108)
    set(FW_FILE "mm8108b2-rl.bin")
endif()

function(find_binary_blob OUT_VAR REQUESTED_NAME SEARCH_ROOT)
    get_filename_component(req_dir  "${REQUESTED_NAME}" DIRECTORY)
    get_filename_component(req_name "${REQUESTED_NAME}" NAME)
    string(REGEX REPLACE "\\.(bin|mbin)$" "" base "${req_name}")

    # Candidate filenames, (user > mbin > bin).
    if(req_name MATCHES "\\.bin$")
        set(exts bin mbin)
    else()
        set(exts mbin bin)
    endif()
    list(GET exts 0 preferred_ext)
    set(candidates "")
    foreach(ext IN LISTS exts)
        list(APPEND candidates "${base}.${ext}")
    endforeach()

    set(found "")

    foreach(cand IN LISTS candidates)
        if(req_dir)
            set(rel "${req_dir}/${cand}")
        else()
            set(rel "${cand}")
        endif()
        get_filename_component(abs "${rel}" ABSOLUTE BASE_DIR "${PROJECT_DIR}")
        if(EXISTS "${abs}" AND NOT IS_DIRECTORY "${abs}")
            set(found "${abs}")
            break()
        endif()
    endforeach()

    if(NOT found)
        foreach(cand IN LISTS candidates)
            file(GLOB_RECURSE matches LIST_DIRECTORIES false "${SEARCH_ROOT}/${cand}")
            if(matches)
                list(LENGTH matches n)
                if(n GREATER 1)
                    message(WARNING "Multiple matches for '${cand}' under ${SEARCH_ROOT}:\n"
                                    "  ${matches}\n"
                                    "Using the first one.")
                endif()
                list(GET matches 0 found)
                break()
            endif()
        endforeach()
    endif()

    if(NOT found)
        message(FATAL_ERROR "Could not resolve blob '${REQUESTED_NAME}'.\n"
                            "Specify a path relative to the project root, or a filename present under:\n"
                            " - ${SEARCH_ROOT}")
    endif()

    get_filename_component(found_ext "${found}" LAST_EXT)
    if(NOT found_ext STREQUAL ".${preferred_ext}")
        message(WARNING "${OUT_VAR}: exact match not found, "
                        "substituted '${found}' for requested '${base}.${preferred_ext}'")
    endif()
    message(STATUS "${OUT_VAR}: ${found}")

    # Convert .bin -> .mbin; an existing .mbin passes through unchanged.
    if(found_ext STREQUAL ".bin")
        idf_build_get_property(python PYTHON)
        get_filename_component(name "${found}" NAME)
        set(mbin "${CMAKE_CURRENT_BINARY_DIR}/${name}.mbin")
        add_custom_command(
            OUTPUT "${mbin}"
            COMMAND ${python} ${CMAKE_CURRENT_LIST_DIR}/../mm-iot-sdk/framework/tools/buildsystem/convert-bin-to-mbin.py
                -t 1,2:2 -o "${mbin}" "${found}"
            DEPENDS "${found}"
        )
        message(STATUS "Converting ${name} to mbin")
        set(found "${mbin}")
    endif()

    set(${OUT_VAR} "${found}" PARENT_SCOPE)
endfunction()

find_binary_blob(BCF_MBIN "${CONFIG_MM_BCF_FILE}" "${BCF_SEARCH_ROOT}")
find_binary_blob(FW_MBIN  "${FW_FILE}" "${FW_SEARCH_ROOT}")

add_library(morsefirmware STATIC)
target_add_binary_data(morsefirmware "${FW_MBIN}"  BINARY RENAME_TO firmware_binary)
target_add_binary_data(morsefirmware "${BCF_MBIN}" BINARY RENAME_TO bcf_binary)
