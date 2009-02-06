# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#
# Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
#
# This file is part of the ART+COM CMake Library (acmake).
#
# It is distributed under the Boost Software License, Version 1.0. 
# (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)             
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#
#
# Utility macros for copying files
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#

#copy a single file
macro(copy_file FROMFILE TOFILE)
  #configure_file needs full pathes
  #files beginning with "." are not allowed
  configure_file(${FROMFILE} ${TOFILE} COPYONLY)
endmacro(copy_file)

#copy a directory recursively  
macro(copy_dir FROMDIR TODIR)
  if(NOT EXISTS ${TODIR})
    file(MAKE_DIRECTORY ${TODIR})
  endif(NOT EXISTS ${TODIR})
  file(GLOB FILES_IN_DIR RELATIVE ${FROMDIR} ${FROMDIR}/*)
  string(REPLACE ";" " " FILE_LIST ${FILES_IN_DIR})
  foreach(AFILE ${FILES_IN_DIR})
    string(SUBSTRING ${AFILE} 0 1 FIRST_CHAR)
    #skip files beginning with "." 
    if(NOT ${FIRST_CHAR} STREQUAL ".")
      if(IS_DIRECTORY ${FROMDIR}/${AFILE})
        #recursive copy
        copy_dir(${FROMDIR}/${AFILE} ${TODIR}/${AFILE})
      else(IS_DIRECTORY ${FROMDIR}/${AFILE})
        copy_file(${FROMDIR}/${AFILE} ${TODIR}/${AFILE})
      endif(IS_DIRECTORY ${FROMDIR}/${AFILE})
    endif(NOT ${FIRST_CHAR} STREQUAL ".")
  endforeach(AFILE ${FILES_IN_DIR})
endmacro(copy_dir)

