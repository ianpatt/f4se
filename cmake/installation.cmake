option(F4SE_COPY_OUTPUT "Copies output files to the fallout 4 directory" OFF)

if (F4SE_COPY_OUTPUT)
	if (NOT DEFINED Fallout4Path AND DEFINED ENV{Fallout4Path})
		set(Fallout4Path $ENV{Fallout4Path})
	endif()

	if (DEFINED Fallout4Path)
		add_custom_command(
			TARGET ${PROJECT_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${PROJECT_NAME}> "${Fallout4Path}"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PDB_FILE:${PROJECT_NAME}> "${Fallout4Path}"
		)
	else()
		message(
			WARNING
			"Variable Fallout4Path is not set. Skipping post-build copy command."
		)
	endif()
endif()
