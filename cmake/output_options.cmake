macro(set_output_dirs target_name)
	set_target_properties("${target_name}"
		PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>"
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>"
		PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
		COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>")
endmacro()

macro(set_msvc_runtime_library target_name runtime_library_type)
	set_property(TARGET "${target_name}" PROPERTY
		MSVC_RUNTIME_LIBRARY "${runtime_library_type}")
endmacro()
