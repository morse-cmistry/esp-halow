# Copyright 2026 Morse Micro
# SPDX-License-Identifier: Apache-2.0

idf_component_register()

# Define architecture specific paths and variables
if(${IDF_TARGET} STREQUAL "esp32s3")
	set(BFDNAME "elf32-xtensa-le")
	set(ARCH "xtensa")
elseif(${IDF_TARGET} STREQUAL "esp32c6")
	set(BFDNAME "elf32-littleriscv")
	set(ARCH "riscv")
elseif(${IDF_TARGET} STREQUAL "esp32c3")
	set(BFDNAME "elf32-littleriscv")
	set(ARCH "riscv")
else()
	message(FATAL_ERROR "Target ${IDF_TARGET} not known. Exiting...")
endif()

if(NOT (CONFIG_MM_CHIP_MM6108 OR CONFIG_MM_CHIP_MM8108) OR
   NOT CONFIG_MM_BCF_FILE)
    # FATAL_ERROR here blocks set-target and menuconfig
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

function(find_binary_blob FILE_VAR OBJ_VAR CONFIG_VAR)
	# Clear the CMAKE cache to force it to search for the file every time.
	unset(${FILE_VAR} CACHE)

	find_file(
		${FILE_VAR}
		NAMES ${${CONFIG_VAR}}
		PATHS ${MMIOT_BLOB_DIR}
		NO_DEFAULT_PATH
	)

	if(NOT ${FILE_VAR})
		message(WARNING "Invalid ${FILE_VAR} specified: ${${CONFIG_VAR}}\n"
						"File must be present in :\n"
						" - ${MMIOT_BLOB_DIR}"
		)
	endif()

	message(STATUS "${FILE_VAR}: ${${FILE_VAR}}")

	set(${OBJ_VAR}
		"${${FILE_VAR}}.o"
		PARENT_SCOPE
	)
endfunction()

find_binary_blob(BCF_MBIN BCF_OBJ CONFIG_MM_BCF_FILE)
find_binary_blob(FW_MBIN FW_OBJ FW_FILE)

function(make_binary_object __file __prefix)
	get_filename_component(__file_name ${__file} NAME)
	set(__object ${MMIOT_BLOB_DIR}/${__file_name}.o)
	string(MAKE_C_IDENTIFIER ${__file} __file_c_identifier)
	add_custom_command(
		OUTPUT ${__object}
		COMMAND
			${CMAKE_OBJCOPY} -I binary -O ${BFDNAME} -B ${ARCH} ${__file}
			${__object} --redefine-sym
			_binary_${__file_c_identifier}_start=${__prefix}_start
			--redefine-sym _binary_${__file_c_identifier}_size=${__prefix}_size
			--redefine-sym _binary_${__file_c_identifier}_end=${__prefix}_end
			--rename-section
			.data=.rodata._fw_mbin,contents,alloc,load,readonly,data
		DEPENDS ${__file}
		COMMENT "Running objcopy on ${__file} to generate ${__object}"
	)
endfunction()

make_binary_object(${FW_MBIN} firmware_binary)
make_binary_object(${BCF_MBIN} bcf_binary)
add_custom_target(morsefirmware ALL DEPENDS ${FW_OBJ} ${BCF_OBJ})
add_dependencies(${COMPONENT_TARGET} morsefirmware)
target_link_libraries(${COMPONENT_TARGET} INTERFACE "${FW_OBJ}" "${BCF_OBJ}")
