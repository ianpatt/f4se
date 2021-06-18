cmake_minimum_required(VERSION 3.20)

project(umbrella)

include(cmake/common.cmake)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR
		"In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
)
endif()

maybe_add_subdirectory(f4se)
maybe_add_subdirectory(f4se_common)
maybe_add_subdirectory(f4se_loader)
maybe_add_subdirectory(f4se_loader_common)
maybe_add_subdirectory(f4se_steam_loader)

set_from_environment(Fallout4Path)

set(Python3_FIND_STRATEGY VERSION)
find_package(
	Python3
	3.9
	QUIET
	MODULE
	COMPONENTS
		Interpreter
)

if (Python3_Interpreter_FOUND AND DEFINED Fallout4Path)
	include(cmake/modified_scripts.cmake)
	include(cmake/vanilla_scripts.cmake)

	set(SCRIPT "scripts/papyrus_cmake.py")

	source_group(
		TREE ${CMAKE_CURRENT_SOURCE_DIR}
		FILES ${SCRIPT}
	)

	macro(papyrus_target TARGET)
		set(_PREFIX "papyrus_target")
		cmake_parse_arguments(${_PREFIX} "" "TARGET_DEPENDS" "DEPENDS;OUTPUT" ${ARGN})

		if(DEFINED ${_PREFIX}_UNPARSED_ARGUMENTS)
			foreach(_ARG ${${_PREFIX}_UNPARSED_ARGUMENTS})
				message(WARNING "Unused argument: ${_ARG}")
			endforeach()
		endif()

		set(_DUMMY "${CMAKE_CURRENT_BINARY_DIR}/papyrus_${TARGET}_ready")

		add_custom_target(
			"papyrus_${TARGET}"
			ALL
			DEPENDS
				"${_DUMMY}"
			SOURCES
				"${SCRIPT}"
		)

		add_custom_command(
			OUTPUT
				"${_DUMMY}"
				${${_PREFIX}_OUTPUT}
			COMMAND
				"${Python3_EXECUTABLE}"
				"${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT}"
				"--f4dir=${Fallout4Path}"
				"--src-dir=${CMAKE_CURRENT_SOURCE_DIR}"
				"--target=${TARGET}"
			COMMAND
				${CMAKE_COMMAND} -E touch "${_DUMMY}"
			DEPENDS
				${${_PREFIX}_DEPENDS}
			WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
			VERBATIM
		)

		if(DEFINED ${_PREFIX}_TARGET_DEPENDS)
			add_dependencies("papyrus_${TARGET}" "papyrus_${${_PREFIX}_TARGET_DEPENDS}")
		endif()

		unset(_DUMMY)
		unset(${_PREFIX}_TARGET_DEPENDS)
		unset(${_PREFIX}_DEPENDS)
		unset(${_PREFIX}_OUTPUT)
		unset(_PREFIX)
	endmacro()

	cmake_path(SET VANILLA_ARCHIVE NORMALIZE "${Fallout4Path}/Data/Scripts/Source/Base/Base.zip")
	papyrus_target(
		"copy_vanilla"
		DEPENDS
			"${VANILLA_ARCHIVE}"
	)

	set(IN_VANILLA ${MODIFIED_SCRIPTS})
	set(REJECT)
	foreach(ITEM ${IN_VANILLA})
		list(FIND VANILLA_SCRIPTS ${ITEM} FOUND)
		if(${FOUND} EQUAL -1)
			list(APPEND REJECT ${ITEM})
		endif()
	endforeach()
	list(REMOVE_ITEM IN_VANILLA ${REJECT})
	list(TRANSFORM IN_VANILLA PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/vanilla/")
	list(TRANSFORM IN_VANILLA APPEND ".psc")
	set(IN_MODIFIED ${MODIFIED_SCRIPTS})
	list(TRANSFORM IN_MODIFIED PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/modified/")
	list(TRANSFORM IN_MODIFIED APPEND ".psc")
	set(MERGE_OUT ${MODIFIED_SCRIPTS})
	list(TRANSFORM MERGE_OUT APPEND ".psc")
	list(APPEND MERGE_OUT "Institute_Papyrus_Flags.flg")
	list(TRANSFORM MERGE_OUT PREPEND "${CMAKE_CURRENT_BINARY_DIR}/artifacts/papyrus/src/")
	papyrus_target(
		"merge"
		TARGET_DEPENDS "copy_vanilla"
		DEPENDS
			${IN_VANILLA}
			${IN_MODIFIED}
		OUTPUT
			${MERGE_OUT}
	)

	set(VANILLA ${VANILLA_SCRIPTS})
	list(TRANSFORM VANILLA PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/vanilla/")
	list(TRANSFORM VANILLA APPEND ".psc")
	set(COMPILE_OUT ${MODIFIED_SCRIPTS})
	list(TRANSFORM COMPILE_OUT PREPEND "${CMAKE_CURRENT_BINARY_DIR}/artifacts/papyrus/bin/")
	list(TRANSFORM COMPILE_OUT APPEND ".pex")
	papyrus_target(
		"compile"
		TARGET_DEPENDS "merge"
		DEPENDS
			${VANILLA}
			${MERGE_OUT}
		OUTPUT
			${COMPILE_OUT}
	)

	set(SANITIZE_OUT ${MODIFIED_SCRIPTS})
	list(TRANSFORM SANITIZE_OUT PREPEND "${CMAKE_CURRENT_BINARY_DIR}/artifacts/papyrus/sanitized/")
	list(TRANSFORM SANITIZE_OUT APPEND ".pex")
	papyrus_target(
		"sanitize"
		TARGET_DEPENDS "compile"
		DEPENDS
			${COMPILE_OUT}
		OUTPUT
			${SANITIZE_OUT}
	)
else()
	if(NOT Python3_Interpreter_FOUND)
		set(REASON "Failed to find python interpreter")
	elseif(NOT DEFINED Fallout4Path)
		set(REASON "Environment variable Fallout4Path is not defined")
	else()
		set(REASON "¯\\_(ツ)_/¯")
	endif()

	message(WARNING "${REASON}. Skipping papyrus target.")
endif()
