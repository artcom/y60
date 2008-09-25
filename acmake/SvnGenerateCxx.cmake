
function(svn_generate_cxx directory sourcefile namespace variable)
  add_custom_command(
    OUTPUT ${sourcefile}
    COMMAND revision2cpp ${directory} ${sourcefile} ${namespace} ${variable}
    COMMENT "Generating C++ source file with svn revision"
    )
endfunction(svn_generate_cxx)

