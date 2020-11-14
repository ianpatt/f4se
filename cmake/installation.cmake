option(F4SE_COPY_OUTPUT "Copies output files to the fallout 4 directory" OFF)

if (F4SE_COPY_OUTPUT)
	add_custom_command(
		TARGET ${PROJECT_NAME}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "$ENV{Fallout4Path}"
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> "$ENV{Fallout4Path}"
	)
endif()
