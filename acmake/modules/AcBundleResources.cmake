# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#
# Copyright (C) 2009, ART+COM AG Berlin, Germany <www.artcom.de>
#
# This file is part of the ART+COM CMake Library (acmake).
#
# It is distributed under the Boost Software License, Version 1.0. 
# (See accompanying file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)             
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#
#
# Utilities to help with adding resources to Mac OS X bundles.
#
# __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
#


macro(ac_create_resource_set RESOURCE_SET BASEDIR BUNDLE_DIR )
    set_global(${RESOURCE_SET}_BASEDIR ${BASEDIR})
    set_global(${RESOURCE_SET}_BUNDLE_DIR ${BUNDLE_DIR})
    set_global(${RESOURCE_SET}_FILES ${ARGN})
endmacro(ac_create_resource_set)

macro(ac_add_resources RESOURCE_SET)
    append_global(${RESOURCE_SET}_FILES ${ARGN})
endmacro(ac_add_resources)

macro(ac_get_all_resources OUTPUT)
    foreach(RESOURCE_SET ${ARGN})
        get_globals(${RESOURCE_SET} ${RESOURCE_SET} FILES BASEDIR BUNDLE_DIR)
        list(APPEND ${OUTPUT} ${${RESOURCE_SET}_FILES})
        clear_locals( ${RESOURCE_SET} FILES BASEDIR BUNDLE_DIR )
    endforeach(RESOURCE_SET ${ARGN})
endmacro(ac_get_all_resources)

macro(_ac_attach_resources)
    foreach(RESOURCE_SET ${ARGN})
        get_globals(${RESOURCE_SET} ${RESOURCE_SET} FILES BASEDIR BUNDLE_DIR)
        foreach(RES ${${RESOURCE_SET}_FILES})
            file(RELATIVE_PATH REL_PATH ${${RESOURCE_SET}_BASEDIR} ${RES})
            get_filename_component(LOC ${REL_PATH} PATH)
            set_source_files_properties( ${RES} PROPERTIES
                    MACOSX_PACKAGE_LOCATION ${${RESOURCE_SET}_BUNDLE_DIR}/${LOC})
        endforeach(RES ${${RESOURCE_SET}_FILES})
        clear_locals( ${RESOURCE_SET} FILES BASEDIR BUNDLE_DIR )
    endforeach(RESOURCE_SET ${ARGN})
endmacro(_ac_attach_resources)
