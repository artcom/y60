
# Doxygen - API reference generator

find_package(Doxygen)

function(ac_add_doxygen NAME SOURCE_DIR OUTPUT_DIR DOXYFILE_IN)
    if(DOXYGEN_FOUND)
        make_directory(${OUTPUT_DIR})
	set(AC_DOXYGEN_NAME ${NAME})
	set(AC_DOXYGEN_SOURCE_DIR ${SOURCE_DIR})
	set(AC_DOXYGEN_OUTPUT_DIR ${OUTPUT_DIR})
        configure_file(
            ${DOXYFILE_IN}
            ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            @ONLY
         )
	 set(AC_DOXYGEN_NAME "")
	 set(AC_DOXYGEN_SOURCE_DIR "")
	 set(AC_DOXYGEN_OUTPUT_DIR "")
        add_custom_target(
            ${NAME}-doxygen
            ${DOXYGEN_EXECUTABLE} Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating doxygen documentation for ${NAME}"
            VERBATIM
        )
    endif(DOXYGEN_FOUND)
endfunction(ac_add_doxygen PROJECT SOURCE DOXYFILE_IN)

# XSDDOC - Schema reference generator

find_package(Java)

set(AC_XSDDOC_JARS xalan-2.7.0.jar xerces.jar xsddoc.jar)

set(AC_XSDDOC_CLASSPATH "")
foreach(JAR ${AC_XSDDOC_JARS})
    set(AC_XSDDOC_CLASSPATH "${ACMAKE_TOOLS_DIR}/java/${JAR}:${AC_XSDDOC_CLASSPATH}")
endforeach(JAR ${AC_XSDDOC_JARS})

function(ac_add_xsddoc NAME SCHEMA DESTINATION)
    if(JAVA_RUNTIME)
        make_directory(${DESTINATION})
        add_custom_target(
	    ${NAME}-xsddoc
	    java -cp ${AC_XSDDOC_CLASSPATH}
	        net.sf.xframe.xsddoc.Main -t ${NAME} -o ${DESTINATION} ${SCHEMA}
	    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	    COMMENT "Generating xml schema documentation for ${SCHEMA}"
	)
    endif(JAVA_RUNTIME)
endfunction(ac_add_xsddoc NAME SCHEMA DESTINATION)
