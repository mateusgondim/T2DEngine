# run SID preprocessor tool on source files

function(preprocess_source_files unprocessed_files preprocessed_files unprocessed_dir preprocessed_dir)
	set(output_list "")
	foreach(unprocessed_file ${unprocessed_files})
		set(preprocessed_file "preprocessed_${unprocessed_file}")
		add_custom_command(
			OUTPUT ${preprocessed_file}
			COMMAND sid_preprocessor "${unprocessed_dir}/${unprocessed_file}" 
			                         "${preprocessed_dir}/${preprocessed_file}"
			PRE_BUILD
			DEPENDS "${unprocessed_dir}/${unprocessed_file}"
			COMMENT "Running SID preprocessor on file ${unprocessed_file}...\
		  generating ${preprocessed_file}"
		)
		message("Appending to ${output_list} ${preprocessed_dir}/${preprocessed_file}")
		list(APPEND output_list "${preprocessed_dir}/${preprocessed_file}")
	endforeach(unprocessed_file)
	set(${preprocessed_files} "${output_list}" PARENT_SCOPE)
endfunction()
