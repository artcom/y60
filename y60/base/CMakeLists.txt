
set(SOURCES
CommonTags.cpp
DataTypes.cpp
DecoderManager.cpp
FrustumValue.cpp
IScene.cpp
NodeNames.cpp
NodeValueNames.cpp
RankedFeature.cpp
TypeTraits.cpp
Y60xsd.cpp
iostream_functions.cpp
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
y60_base_settings.h
)

set(TESTS
DataTypes
XmlDomY60Types
)

list( APPEND EXTERNALS OPENEXR )

if(WIN32)
    list( APPEND EXTERNALS PTHREADVC )
endif(WIN32)

ac_add_library(
    y60base y60/base
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS aslbase aslmath asldom
    EXTERNS ${EXTERNALS}
    TESTS   ${TESTS}
)

