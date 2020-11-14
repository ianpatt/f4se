set(F4SE_COPY_OUTPUT OFF CACHE BOOL "copies output files to the fallout 4 directory")

if (F4SE_COPY_OUTPUT)
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "$ENV{Fallout4Path}"
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> "$ENV{Fallout4Path}"
	)
endif()
