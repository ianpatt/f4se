cmake_minimum_required(VERSION 3.20)

include(../cmake/common.cmake)
include(cmake/headerlist.cmake)
include(cmake/sourcelist.cmake)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)

add_project(
	PROJECT f4se_steam_loader
	TARGET_TYPE LIBRARY
	LIBRARY_TYPE SHARED
	GROUPED_FILES
		${HEADERS}
		${SOURCES}
)

maybe_add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../f4se_common f4se_common)	# bundled
maybe_add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../f4se_loader_common f4se_loader_common)	# bundled

target_link_libraries(
	${PROJECT_NAME}
	PUBLIC
		f4se::f4se_common
		f4se::f4se_loader_common
		Version.lib
)

target_sources(
	${PROJECT_NAME}
	PRIVATE
		${f4se_common_SOURCE_DIR}/f4se_version.rc
)

copy_files()
