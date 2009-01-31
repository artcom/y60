
function(ac_generate_revision PROJECT DIRECTORY FILES)
    add_custom_command(
        OUTPUT ${ACMAKE_
        COMMAND svnversion ${DIRECTORY}
    )
endfunction(ac_generate_revision)

