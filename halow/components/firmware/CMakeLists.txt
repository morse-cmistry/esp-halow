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

if(CONFIG_MM_CHIP_MM6108)
    set(MMIOT_BLOB_DIR "${CMAKE_CURRENT_LIST_DIR}/mm6108")
    set(FW_FILE "mm6108.mbin")
elseif(CONFIG_MM_CHIP_MM8108)
    set(MMIOT_BLOB_DIR "${CMAKE_CURRENT_LIST_DIR}/mm8108")
    set(FW_FILE "mm8108.mbin")
endif()

function(find_binary_blob FILE_VAR CONFIG_VAR)
	# Clear the CMAKE cache to force it to search for the file every time.
	unset(${FILE_VAR} CACHE)

	get_filename_component(__project_path ${${CONFIG_VAR}} ABSOLUTE BASE_DIR ${PROJECT_DIR})

	# Check if the blob exists at a path relative to the project root
	if(EXISTS ${__project_path} AND NOT IS_DIRECTORY ${__project_path})
		set(${FILE_VAR} ${__project_path})
	else()
		find_file(
			${FILE_VAR}
			NAMES ${${CONFIG_VAR}}
			PATHS ${MMIOT_BLOB_DIR}
			NO_DEFAULT_PATH
		)
	endif()

	if(NOT ${FILE_VAR})
		message(WARNING "Invalid ${FILE_VAR} specified: ${${CONFIG_VAR}}\n"
						"File must be present in :\n"
						" - ${MMIOT_BLOB_DIR}"
		)
	endif()

	message(STATUS "${FILE_VAR}: ${${FILE_VAR}}")

	get_filename_component(__blob_ext ${${FILE_VAR}} LAST_EXT)

	if(${__blob_ext} STREQUAL ".bin")

		idf_component_get_property(_halow_dir halow COMPONENT_DIR)

		idf_build_get_property(__python PYTHON)

		set(__bin ${${FILE_VAR}})

		get_filename_component(__base_file ${__bin} NAME)

		set(${FILE_VAR} ${CMAKE_CURRENT_BINARY_DIR}/${__base_file}.mbin)

		add_custom_command(
			OUTPUT ${${FILE_VAR}}
            COMMAND ${__python} ${CMAKE_CURRENT_LIST_DIR}/../mm-iot-sdk/framework/tools/buildsystem/convert-bin-to-mbin.py
				-o ${${FILE_VAR}} ${__bin}
			DEPENDS ${__bin}
			COMMENT "Converting ${__bin} to mbin"
		)

	endif()

	get_filename_component(__file_name ${${FILE_VAR}} NAME)

    set(${FILE_VAR} "${${FILE_VAR}}" PARENT_SCOPE)
endfunction()

find_binary_blob(BCF_MBIN CONFIG_MM_BCF_FILE)
find_binary_blob(FW_MBIN FW_FILE)

add_library(morsefirmware STATIC)

target_add_binary_data(morsefirmware "${FW_MBIN}"  BINARY RENAME_TO firmware_binary)
target_add_binary_data(morsefirmware "${BCF_MBIN}" BINARY RENAME_TO bcf_binary)
