
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

ac_add_library(
  asldom
  SOURCES ${SOURCES}
  HEADERS ${HEADERS}
  TESTS ${TESTS}
  DEPENDS aslbase
)
