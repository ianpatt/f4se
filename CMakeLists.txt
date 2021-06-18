cmake_minimum_required(VERSION 3.20)

project(umbrella)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR
		"In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
)
endif()

macro(maybe_add_subdirectory TARGET)
	if (NOT TARGET ${TARGET})
		add_subdirectory(${TARGET})
	endif()
endmacro()

maybe_add_subdirectory(f4se)
maybe_add_subdirectory(f4se_common)
maybe_add_subdirectory(f4se_loader)
maybe_add_subdirectory(f4se_loader_common)
maybe_add_subdirectory(f4se_steam_loader)
