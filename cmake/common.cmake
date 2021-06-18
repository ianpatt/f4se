cmake_minimum_required(VERSION 3.20)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "")

set(F4SE_VERSION_MAJOR 0)
set(F4SE_VERSION_MINOR 6)
set(F4SE_VERSION_PATCH 21)

set(F4_VERSION_MAJOR 1)
set(F4_VERSION_MINOR 10)
set(F4_VERSION_PATCH 163)

math(
	EXPR
	F4_VERSION_PACKED
	"((${F4_VERSION_MAJOR} & 0xFF) << 24) | ((${F4_VERSION_MINOR} & 0xFF) << 16) | ((${F4_VERSION_PATCH} & 0xFFF) << 4)"
	OUTPUT_FORMAT
		HEXADECIMAL
)

macro(set_from_environment VARIABLE)
	if(NOT DEFINED ${VARIABLE} AND DEFINED ENV{${VARIABLE}})
		set(${VARIABLE} $ENV{${VARIABLE}})
	endif()
endmacro()

macro(maybe_add_subdirectory)
	if(${ARGC} EQUAL 1)
		if(NOT TARGET "${ARGV0}")
			add_subdirectory("${ARGV0}")
		endif()
	elseif(${ARGC} EQUAL 2})
		if(NOT TARGET "${ARGV1}")
			add_subdirectory("${ARGV0}" "${ARGV1}")
		endif()
	else()
		message(FATAL_ERROR "Invalid number of parameters.")
	endif()
endmacro()

macro(add_project)
	set(_PREFIX add_project)

	set(_OPTIONS)
	set(_ONE_VALUE_ARGS
		LIBRARY_TYPE
		PROJECT
		TARGET_TYPE
	)
	set(_MULTI_VALUE_ARGS
		GROUPED_FILES
		INCLUDE_DIRECTORIES
		MISC_FILES
	)

	set(_REQUIRED
		PROJECT
		TARGET_TYPE
	)

	cmake_parse_arguments(
		${_PREFIX}
		"${_OPTIONS}"
		"${_ONE_VALUE_ARGS}"
		"${_MULTI_VALUE_ARGS}"
		${ARGN}
	)

	foreach(_ARG ${_REQUIRED})
		if(NOT DEFINED "${_PREFIX}_${_ARG}")
			message(FATAL_ERROR "Argument is required to be defined: ${_ARG}")
		endif()
	endforeach()

	set(_CLEANUP
		_PREFIX
		_OPTIONS
		_ONE_VALUE_ARGS
		_MULTI_VALUE_ARGS
		_REQUIRED
		${_PREFIX}_UNPARSED_ARGUMENTS
		${_PREFIX}_KEYWORDS_MISSING_VALUES
	)

	foreach(_ARG IN LISTS _OPTIONS _ONE_VALUE_ARGS _MULTI_VALUE_ARGS)
		list(APPEND _CLEANUP "${_PREFIX}_${_ARG}")
	endforeach()

	# ---- Argument validation

	string(TOLOWER "${${_PREFIX}_TARGET_TYPE}" "${_PREFIX}_TARGET_TYPE")

	if(DEFINED "${_PREFIX}_UNPARSED_ARGUMENTS")
		foreach(_ARG "${_PREFIX}_UNPARSED_ARGUMENTS")
			message(WARNING "Unused argument: ${_ARG}")
		endforeach()
	endif()

	set(_TARGET_TYPES executable library)
	list(APPEND _CLEANUP _TARGET_TYPES)
	if(NOT "${${_PREFIX}_TARGET_TYPE}" IN_LIST _TARGET_TYPES)
		message(FATAL_ERROR "TARGET_TYPE \"${${_PREFIX}_TARGET_TYPE}\" must be one of: ${_TARGET_TYPES}")
	endif()

	set(_LIBRARY_TYPES STATIC SHARED MODULE)
	list(APPEND _CLEANUP _LIBRARY_TYPES)
	if(DEFINED "${_PREFIX}_LIBRARY_TYPE" AND
		NOT "${${_PREFIX}_LIBRARY_TYPE}" IN_LIST _LIBRARY_TYPES)
		message(FATAL_ERROR "LIBRARY_TYPE \"${${_PREFIX}_LIBRARY_TYPE}\" must be one of: ${_LIBRARY_TYPES}")
	endif()

	if("${_PREFIX}_TARGET_TYPE" STREQUAL "executable" AND
		DEFINED "${_PREFIX}_LIBRARY_TYPE")
		message(FATAL_ERROR "LIBRARY_TYPE must not be set for \"executable\" targets")
	endif()

	# ---- Project ----

	project(
		"${${_PREFIX}_PROJECT}"
		VERSION ${F4SE_VERSION_MAJOR}.${F4SE_VERSION_MINOR}.${F4SE_VERSION_PATCH}
		LANGUAGES CXX
	)

	include(GNUInstallDirs)

	# ---- Include guards ----

	if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
		message(
			FATAL_ERROR
			"In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
		)
	endif()

	# ---- Add target ----

	cmake_language(
		CALL
		"add_${${_PREFIX}_TARGET_TYPE}"	# add_executable/add_library
		${PROJECT_NAME}
		"${${_PREFIX}_LIBRARY_TYPE}"
		${${_PREFIX}_GROUPED_FILES}
		${${_PREFIX}_MISC_FILES}
	)

	cmake_language(
		CALL
		"add_${${_PREFIX}_TARGET_TYPE}"	# add_executable/add_library
		f4se::${PROJECT_NAME} ALIAS ${PROJECT_NAME}
	)

	set_target_properties(
		${PROJECT_NAME}
		PROPERTIES
			CXX_STANDARD_REQUIRED ON
	)

	target_compile_features(
		${PROJECT_NAME}
		PUBLIC
			cxx_std_11
	)

	target_include_directories(
		${PROJECT_NAME}
		PRIVATE
			"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>"
			"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
	)

	if(MSVC)
		set(_COMPILE_OPTIONS_DEBUG
			/JMC	# Just My Code debugging
			/ZI	# Debug Information Format
		)
		list(APPEND _CLEANUP _COMPILE_OPTIONS_DEBUG)

		set(_COMPILE_OPTIONS_RELEASE
			/Oi	# Generate Intrinsic Functions
			/Zi	# Debug Information Format
		)
		list(APPEND _CLEANUP _COMPILE_OPTIONS_RELEASE)

		target_compile_options(
			${PROJECT_NAME}
			PRIVATE
				/W3	# Warning level

				"$<$<CONFIG:Debug>:${COMPILE_OPTIONS_DEBUG}>"
				"$<$<CONFIG:Release>:${COMPILE_OPTIONS_RELEASE}>"
		)

		set(_LINK_OPTIONS_DEBUG
			/INCREMENTAL	# Link Incrementally
		)
		list(APPEND _CLEANUP _LINK_OPTIONS_DEBUG)

		set(_LINK_OPTIONS_RELEASE
			/DEBUG:FULL	# Generate Debug Info
			/INCREMENTAL:NO	# Link Incrementally
			/OPT:REF	# Optimizations (eliminate functions/data never referenced)
			/OPT:ICF	# Optimizations (perform identical COMDAT folding)
		)
		list(APPEND _CLEANUP _LINK_OPTIONS_RELEASE)

		target_link_options(
			${PROJECT_NAME}
			PRIVATE
				"$<$<CONFIG:Debug>:${LINK_OPTIONS_DEBUG}>"
				"$<$<CONFIG:Release>:${LINK_OPTIONS_RELEASE}>"
		)

		if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
			target_compile_options(
				${PROJECT_NAME}
				PRIVATE
					/MP	# Build with Multiple Processes
			)
		endif()
	endif()

	source_group(
		${PROJECT_NAME}
		FILES ${${_PREFIX}_GROUPED_FILES}
	)

	# ---- Cleanup local variables ----

	foreach(_VAR "${_CLEANUP}")
		unset("${_VAR}")
	endforeach()
	unset(_CLEANUP)
endmacro()

macro(copy_files)
	if(${ARGC} GREATER 0)
		message(FATAL_ERROR "Invalid number of arguments.")
	endif()

	option(F4SE_COPY_OUTPUT "Copies output files to the fallout 4 directory" OFF)

	if(F4SE_COPY_OUTPUT)
		set_from_environment(Fallout4Path)
		if(DEFINED Fallout4Path)
			add_custom_command(
				TARGET ${PROJECT_NAME}
				POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${Fallout4Path}"
				COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${Fallout4Path}"
			)
		else()
			message(WARNING "Variable Fallout4Path is not defined. Skipping post-build copy command.")
		endif()
	endif()
endmacro()

macro(install_project)
	include(GNUInstallDirs)

	install(
		TARGETS ${PROJECT_NAME}
		EXPORT ${PROJECT_NAME}-targets
	)

	install(
		FILES ${HEADERS}
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
	)

	configure_file(
		cmake/config.cmake.in
		${PROJECT_NAME}-config.cmake
		@ONLY
	)

	install(
		FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
	)

	install(
		EXPORT ${PROJECT_NAME}-targets
		NAMESPACE f4se::
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
	)
endmacro()
