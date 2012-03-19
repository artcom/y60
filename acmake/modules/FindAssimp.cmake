# TODO: Add implementation for Windows 

FIND_PATH(ASSIMP_INCLUDE_DIR assimp/assimp.h)

SET(ASSIMP_NAMES ${ASSIMP_NAMES} assimp libassimp) 
FIND_LIBRARY(ASSIMP_LIBRARY NAMES ${ASSIMP_NAMES} )
SET(ASSIMP_NAMES_D ${ASSIMP_NAMES_D} assimpd libassimpd) 
FIND_LIBRARY(ASSIMP_LIBRARY_D NAMES ${ASSIMP_NAMES_D} )

# handle the QUIETLY and REQUIRED arguments and set ASSIMP_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ASSIMP DEFAULT_MSG ASSIMP_LIBRARY ASSIMP_INCLUDE_DIR)

IF(ASSIMP_FOUND)
  SET(ASSIMP_LIBRARIES ${ASSIMP_LIBRARY})
  SET(ASSIMP_LIBRARIES_D ${ASSIMP_LIBRARY_D})
ENDIF(ASSIMP_FOUND)

# Deprecated declarations.
SET (NATIVE_ASSIMP_INCLUDE_PATH ${ASSIMP_INCLUDE_DIR} )
IF(ASSIMP_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_ASSIMP_LIB_PATH ${ASSIMP_LIBRARY} PATH)
ENDIF(ASSIMP_LIBRARY)

MARK_AS_ADVANCED(ASSIMP_LIBRARY ASSIMP_LIBRARY_D ASSIMP_INCLUDE_DIR )

# 
# Set(ASSIMP_LIB_NAME "libassimp.so")
# 
# If(APPLE)
#     set(ASSIMP_LIB_NAME "libassimp.dylib")
# Endif(APPLE)
# 
# FIND_PATH(ASSIMP_INCLUDE_DIR 
#         assimp/assimp.h 
#     PATHS
#         /opt/local/include
#         /usr/local/include
#         /usr/include
#     )
# 
# Set(ASSIMP_INCLUDE_DIRS ${ASSIMP_INCLUDE_DIR})
# Mark_as_advanced(ASSIMP_DEFINITIONS ASSIMP_INCLUDE_DIR ASSIMP_INCLUDE_DIRS)
# 
# Find_library(ASSIMP_LIBRARIES NAMES ${ASSIMP_LIB_NAME} 
#     PATH_SUFFIXES
#         lib
#     PATHS
#         /opt/local
#         /usr/local
#         /usr
#     )
# 
# Include(FindPackageHandleStandardArgs)
# Find_package_handle_standard_args(
#         ASSIMP DEFAULT_MSG
#         ASSIMP_LIBRARIES ASSIMP_INCLUDE_DIR
# )
# 
