
set(SOURCES
ArgumentHolder.cpp
ClassMapSingleton.cpp
Documentation.cpp
IScriptablePlugin.cpp
JSAccessibleVector.cpp
JSBlock.cpp
JSBox.cpp
JSBox2f.cpp
JSDomEvent.cpp
JSFrustum.cpp
JSGlobal.cpp
JSLine.cpp
JSMatrix.cpp
JSNamedNodeMap.cpp
JSNode.cpp
JSNodeList.cpp
JSPlane.cpp
JSQuaternion.cpp
JSResizeableRaster.cpp
JSResizeableVector.cpp
JSScriptablePlugin.cpp
JSSphere.cpp
JSTriangle.cpp
JSVector.cpp
JSWrapper.cpp
JScppUtils.cpp
PropertyValue.cpp
)

set(HEADERS
ArgumentHolder.h
ArgumentHolder.impl
ClassMapSingleton.h
Documentation.h
IFactoryPlugin.h
IJSModuleLoader.h
IScriptablePlugin.h
IScriptablePlugin.impl
JSAccessibleVector.h
JSBitset.h
JSBlock.h
JSBox.h
JSBox2f.h
JSDomEvent.h
JSEnum.h
JSFrustum.h
JSGlobal.h
JSLine.h
JSLine.impl
JSMatrix.h
JSNamedNodeMap.h
JSNode.h
JSNodeList.h
JSPlane.h
JSQuaternion.h
JSResizeableRaster.h
JSResizeableVector.h
JSScriptablePlugin.h
JSSphere.h
JSTriangle.h
JSVector.h
JSVector.impl
JSWrapper.h
JSWrapper.impl
JScppUtils.h
JScppUtils.impl
PropertyValue.h
PropertyValue.impl
QuitFlagSingleton.h
jssettings.h
y60_jsbase_settings.h
)

set(TESTS
GarbageCollection
)

ac_add_library(
    y60jsbase y60/jsbase
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS GLIB2 ${JSEXTERN}
    DEPENDS aslbase aslmath asldom aslxpath
            y60base y60scene y60image ${JSDEPENDS} 
            paintlib
    TESTS   ${TESTS}
    VERSION ${VERSION_STRING}
    SOVERSION ${VERSION_MAJOR}
    RUNTIME_INSTALL_COMPONENT ${RUNTIME_COMPONENT}
    DEVELOPMENT_INSTALL_COMPONENT ${DEVELOPMENT_COMPONENT}
)

