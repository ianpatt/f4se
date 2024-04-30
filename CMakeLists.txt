cmake_minimum_required(VERSION 3.18)

# ---- Project ----

project(
	umbrella
	LANGUAGES CXX
)

# ---- Include guards ----

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR
			"In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
)
endif()

# ---- Add sub projects ----

if (NOT TARGET f4se)
	add_subdirectory(f4se)
endif()

if (NOT TARGET f4se_common)
	add_subdirectory(f4se_common)
endif()

if (NOT TARGET f4se_loader)
	add_subdirectory(f4se_loader)
endif()

if (NOT TARGET f4se_loader_common)
	add_subdirectory(f4se_loader_common)
endif()
