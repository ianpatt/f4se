if (MSVC)
	target_compile_options(
		${PROJECT_NAME}
		PRIVATE
			/Gy	# Enable Function-Level Linking
			/MP	# Build with Multiple Processes
			/W3	# Warning level
			
			$<$<CONFIG:Debug>:
				/JMC	# Just My Code debugging
				/ZI	# Debug Information Format
			>
			
			$<$<CONFIG:Release>:
				/Zi	# Debug Information Format
				/Oi	# Generate Intrinsic Functions
			>
	)
	
	target_link_options(
		${PROJECT_NAME}
		PRIVATE
			$<$<CONFIG:Debug>:
				/INCREMENTAL	# Link Incrementally
			>
			
			$<$<CONFIG:Release>:
				/INCREMENTAL:NO	# Link Incrementally
				/OPT:REF	# Optimizations (eliminate functions/data never referenced)
				/OPT:ICF	# Optimizations (perform identical COMDAT folding)
			>
	)
endif()
