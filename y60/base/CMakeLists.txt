
set(SOURCES
CommonTags.cpp
DataTypes.cpp
DecoderManager.cpp
FrustumValue.cpp
IScene.cpp
IDecoder.cpp
NodeNames.cpp
NodeValueNames.cpp
RankedFeature.cpp
TypeTraits.cpp
Y60xsd.cpp
iostream_functions.cpp
FFMpegLockManager.cpp
)

set(HEADERS
AcBool.h
CommonTags.h
DataTypes.h
DecoderManager.h
FrustumValue.h
IDecoder.h
IResourceManager.h
IScene.h
NodeNames.h
NodeValueNames.h
PropertyNames.h
RankedFeature.h
SettingsParser.h
TypeTraits.h
VertexDataRoles.h
Y60xsd.h
iostream_functions.h
property_functions.h
typedefs.h
y60_pixel_types.h
y60_base_settings.h
FFMpegLockManager.h
)

set(TESTS
DataTypes
XmlDomY60Types
)

if(NOT WIN32)
    find_package(Boost COMPONENTS thread system)
endif(NOT WIN32)

add_definitions(-D__STDC_CONSTANT_MACROS) # ffmpeg is c99 only and needs some c macros

ac_add_library(
    y60base y60/base
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath asldom
    EXTERNS OPENEXR FFMPEG Boost
    TESTS   ${TESTS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

asl_add_schema(
    Y60.xsd
    CXX_FILE      Y60xsd.cpp
    CXX_NAMESPACE y60
    CXX_VARNAME   ourY60xsd
    CXX_INCLUDE   Y60xsd.h
)
