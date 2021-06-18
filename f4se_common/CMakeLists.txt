cmake_minimum_required(VERSION 3.20)

include(../cmake/common.cmake)
include(cmake/headerlist.cmake)
include(cmake/sourcelist.cmake)

add_project(
	PROJECT f4se_common
	TARGET_TYPE LIBRARY
	LIBRARY_TYPE STATIC
	GROUPED_FILES
		${HEADERS}
		${SOURCES}
		f4se_version.rc
)

find_package(common REQUIRED CONFIG)

target_link_libraries(
	${PROJECT_NAME}
	PUBLIC
		xse::common
)

install_project()
