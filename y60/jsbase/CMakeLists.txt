
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
ClassMapSingleton.h
Documentation.h
IFactoryPlugin.h
IScriptablePlugin.h
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
JSWrapper.h
JScppUtils.h
PropertyValue.h
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
)

