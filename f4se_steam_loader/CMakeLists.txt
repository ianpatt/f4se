cmake_minimum_required(VERSION 3.18)

# ---- Project ----

project(
	f4se_steam_loader
	VERSION 0.6.21
	LANGUAGES CXX
)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR
			"In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
)
endif()

# ---- Build options ----

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)

if (MSVC)
	set(COMPILE_OPTIONS
		/Gy	# Enable Function-Level Linking
		/MP	# Build with Multiple Processes
		
		/W3	# Warning level
	)
	
	set(COMPILE_OPTIONS_DEBUG
		/JMC	# Just My Code debugging
		/ZI	# Debug Information Format
	)
	
	set(COMPILE_OPTIONS_RELEASE
		/Zi	# Debug Information Format
		/Oi	# Generate Intrinsic Functions
	)
	
	set(LINK_OPTIONS_DEBUG
		/INCREMENTAL	# Link Incrementally
	)
	
	set(LINK_OPTIONS_RELEASE
		/INCREMENTAL:NO	# Link Incrementally
		/OPT:REF	# Optimizations (eliminate functions/data never referenced)
		/OPT:ICF	# Optimizations (perform identical COMDAT folding)
	)
endif()

# ---- Dependencies ----

if (NOT TARGET f4se_common)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../f4se_common f4se_common)	# bundled
endif()

if (NOT TARGET f4se_loader_common)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../f4se_loader_common f4se_loader_common)	# bundled
endif()

# ---- Add source files ----

set(sources
	main.cpp
)

source_group(
	${PROJECT_NAME}
	FILES
		${headers}
		${sources}
)

# ---- Create library ----

add_library(
	${PROJECT_NAME}
	SHARED
	${headers}
	${sources}
	${f4se_common_SOURCE_DIR}/f4se_version.rc
)

add_library(f4se::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(
	${PROJECT_NAME}
	PUBLIC
		cxx_std_11
)

target_compile_options(
	${PROJECT_NAME}
	PRIVATE
		${COMPILE_OPTIONS}
		"$<$<CONFIG:Debug>:${COMPILE_OPTIONS_DEBUG}>"
		"$<$<CONFIG:Release>:${COMPILE_OPTIONS_RELEASE}>"
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
		$<INSTALL_INTERFACE:include>
)

target_link_libraries(
	${PROJECT_NAME}
	PUBLIC
		f4se::f4se_common
		f4se::f4se_loader_common
		Version.lib
)

target_link_options(
	${PROJECT_NAME}
	PRIVATE
		"$<$<CONFIG:Debug>:${LINK_OPTIONS_DEBUG}>"
		"$<$<CONFIG:Release>:${LINK_OPTIONS_RELEASE}>"
)

# ---- Configure all targets ----

set_target_properties(
	${PROJECT_NAME}
	f4se_common
	f4se_loader_common
	PROPERTIES
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# ---- Create an installable target ----

include(GNUInstallDirs)

install(
	TARGETS
		${PROJECT_NAME}
	EXPORT
		${PROJECT_NAME}-targets
)

install(
	FILES
		${headers}
	DESTINATION
		${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

configure_file(
	cmake/config.cmake.in
	${PROJECT_NAME}-config.cmake
	@ONLY
)

install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
	DESTINATION
		${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(
	EXPORT
		${PROJECT_NAME}-targets
	NAMESPACE
		f4se::
	DESTINATION
		${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
