
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
y60_glrender_settings.h
)

set(TESTS
CgShaders
)

ac_add_library(
    y60glrender y60/glrender
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath asldom aslzip
            y60base y60image y60video y60scene y60glutil y60gltext
    EXTERNS GLEW NVIDIA_CG
    TESTS   ${TESTS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

asl_add_schema(
    ShaderLibrary.xsd
    CXX_FILE      ShaderLibraryxsd.cpp
    CXX_NAMESPACE y60
    CXX_VARNAME   ourShaderLibraryxsd
    CXX_INCLUDE   ShaderLibraryxsd.h
)
