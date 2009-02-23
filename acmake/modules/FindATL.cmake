

# The following macro was "stolen" from http://www.vtk.org/Bug/view.php?id=5193.
# It checks for MFC. We expect that where there is MFC, there will be ATL, too.
# And so, if we have MFC, we do not need FakeATL. Else, we do.
#-----------------------------------------------------------------------
# a macro to check for MFC and setup to build the MFC Dialog
# simply to improve readability of the main script
#-----------------------------------------------------------------------
MACRO(CMAKE_TEST_FOR_MFC)
  SET(CMAKE_BUILD_ON_VISUAL_STUDIO 0)
  IF(WIN32 AND NOT UNIX AND NOT BORLAND AND NOT MINGW )
    SET(CMAKE_BUILD_ON_VISUAL_STUDIO 1)
  ENDIF(WIN32 AND NOT UNIX AND NOT BORLAND AND NOT MINGW )
  
  IF(CMAKE_BUILD_ON_VISUAL_STUDIO)
    IF("CMake_HAVE_MFC" MATCHES "^CMake_HAVE_MFC$")
      SET(CHECK_INCLUDE_FILE_VAR "afxwin.h")
      CONFIGURE_FILE(${CMAKE_ROOT}/Modules/CheckIncludeFile.cxx.in
        ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/CheckIncludeFile.cxx)
      MESSAGE(STATUS "Looking for MFC")
      TRY_COMPILE(CMake_HAVE_MFC
        ${CMAKE_BINARY_DIR}
        ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/CheckIncludeFile.cxx
        CMAKE_FLAGS
        -DCMAKE_MFC_FLAG:STRING=2
        -DCOMPILE_DEFINITIONS:STRING=-D_AFXDLL
        OUTPUT_VARIABLE OUTPUT)
      IF(CMake_HAVE_MFC)
        MESSAGE(STATUS "Looking for MFC - found")
        SET(CMake_HAVE_MFC 1 CACHE INTERNAL "Have MFC?")
        FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeOutput.log
          "Determining if MFC exists passed with the following output:\n"
          "${OUTPUT}\n\n")
      ELSE(CMake_HAVE_MFC)
        MESSAGE(STATUS "Looking for MFC - not found")
        SET(CMake_HAVE_MFC 0 CACHE INTERNAL "Have MFC?")
        FILE(APPEND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeError.log
          "Determining if MFC exists failed with the following output:\n"
          "${OUTPUT}\n\n")
      ENDIF(CMake_HAVE_MFC)
    ENDIF("CMake_HAVE_MFC" MATCHES "^CMake_HAVE_MFC$")
    
    IF(CMake_HAVE_MFC)
      OPTION(BUILD_MFCDialog "Whether to build the CMakeSetup MFC dialog." ON)
    ELSE(CMake_HAVE_MFC)
      SET(BUILD_MFCDialog 0)
    ENDIF(CMake_HAVE_MFC)
  ELSE(CMAKE_BUILD_ON_VISUAL_STUDIO)
    SET(BUILD_MFCDialog 0)
  ENDIF(CMAKE_BUILD_ON_VISUAL_STUDIO)
ENDMACRO(CMAKE_TEST_FOR_MFC)

if(WIN32)
    cmake_test_for_mfc()
    
    if(CMake_HAVE_MFC)
        set(ATL_SOURCE AssumedPresent)
    else(CMake_HAVE_MFC)
        find_package(FakeATL)
        set(ATL_INCLUDE_DIRS ${FakeATL_INCLUDE_DIRS})
        set(ATL_SOURCE FakeATL)
    endif(CMake_HAVE_MFC)
endif(WIN32)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ATL DEFAULT_MSG ATL_SOURCE)
