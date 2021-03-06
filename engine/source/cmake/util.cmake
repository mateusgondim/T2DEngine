function(prepend_string_to_list str values output_list)
	set(aux_list "")
	foreach(v ${values})
		list(APPEND aux_list ${str}${v})
		message("Appending ${str} to ${v} = ${str}${v}")
	endforeach(v)
	set(${output_list} "${aux_list}" PARENT_SCOPE)
endfunction()
