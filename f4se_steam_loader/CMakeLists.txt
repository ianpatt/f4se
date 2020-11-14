cmake_minimum_required(VERSION 3.18)

# ---- Project ----

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/versioning.cmake)

project(
	f4se_steam_loader
	VERSION ${F4SE_VERSION_MAJOR}.${F4SE_VERSION_MINOR}.${F4SE_VERSION_PATCH}
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

# ---- Dependencies ----

if (NOT TARGET f4se_common)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../f4se_common f4se_common)	# bundled
endif()

if (NOT TARGET f4se_loader_common)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../f4se_loader_common f4se_loader_common)	# bundled
endif()

# ---- Add source files ----

include(cmake/headerlist.cmake)
include(cmake/sourcelist.cmake)

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

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/configuration.cmake)

target_compile_features(
	${PROJECT_NAME}
	PUBLIC
		cxx_std_11
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

# ---- Configure all targets ----

set_target_properties(
	${PROJECT_NAME}
	f4se_common
	f4se_loader_common
	PROPERTIES
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/installation.cmake)
