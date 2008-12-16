
set(SOURCES
CgProgramInfo.cpp
CgShader.cpp
FFShader.cpp
GLResourceManager.cpp
GLShader.cpp
Renderer.cpp
ShaderFeatureSet.cpp
ShaderLibrary.cpp
ShaderLibraryxsd.cpp
SkinAndBonesShader.cpp
)

set(HEADERS
BodyPart.h
CgProgramInfo.h
CgShader.h
FFShader.h
GLResourceManager.h
GLShader.h
Renderer.h
ShaderDescription.h
ShaderFeatureSet.h
ShaderLibrary.h
ShaderLibraryxsd.h
ShaderTypes.h
SkinAndBonesShader.h
)

set(TESTS
CgShaders
)

if(OSX)
endif(OSX)

if(LINUX)
endif(LINUX)

if(WIN32)
endif(WIN32)

ac_add_library(
    y60glrender y60/glrender
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath asldom aslzip
            y60base y60image y60video y60scene y60glutil y60gltext
    EXTERNS GLEW CG OPENEXR PAINTLIB
    TESTS   ${TESTS}
)

