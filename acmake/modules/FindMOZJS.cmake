# - Find tracemonkey javascript library
# Find the native MOZJS headers and libraries.
#
#  MOZJS_INCLUDE_DIRS - where to find js/jsapi.h, etc.
#  MOZJS_LIBRARIES    - List of libraries when using js.
#  MOZJS_FOUND        - True if js found.

# Look for the header file.
FIND_PATH(MOZJS_INCLUDE_DIR NAMES js/jsapi.h)
MARK_AS_ADVANCED(MOZJS_INCLUDE_DIR)

# Look for the library.
FIND_LIBRARY(MOZJS_LIBRARY NAMES mozjs libmozjs)
FIND_LIBRARY(MOZJS_LIBRARY_D NAMES mozjsd libmozjsd)
MARK_AS_ADVANCED(MOZJS_LIBRARY MOZJS_LIBRARY_D)

#TODO: Use deps/bin/js-config script to get --libs and --cflags

# handle the QUIETLY and REQUIRED arguments and set MOZJS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MOZJS DEFAULT_MSG MOZJS_LIBRARY MOZJS_INCLUDE_DIR)

IF(MOZJS_FOUND)
  SET(MOZJS_LIBRARIES ${MOZJS_LIBRARY})
  SET(MOZJS_LIBRARIES_D ${MOZJS_LIBRARY_D})
  SET(MOZJS_INCLUDE_DIRS ${MOZJS_INCLUDE_DIR})
ELSE(MOZJS_FOUND)
  SET(MOZJS_LIBRARIES)
  SET(MOZJS_INCLUDE_DIRS)
ENDIF(MOZJS_FOUND)
