
set(SOURCES
Events.cpp
Facade.cpp
Field.cpp
NodeList.cpp
Nodes.cpp
Schema.cpp
Value.cpp
XmlSchemaString.cpp
)

set(HEADERS
asl_dom_settings.h
AttributePlug.h
ChildNodePlug.h
Dictionary.h
Events.h
Exceptions.h
Facade.h
Field.h
NodeList.h
Nodes.h
PlugHelper.h
PropertyPlug.h
Schema.h
SchemaStrings.h
ThreadingModel.h
typedefs.h
Value.h
)

set(TESTS
Facade
#UTF8
#Value
XmlDom
Xml
)

# XXX hack to find boost for OSX
# include_directories(/opt/local/include)

ac_add_library(
    asldom asl/dom
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    TESTS   ${TESTS}
    DEPENDS aslbase aslraster
    EXTERNS OPENEXR
)
