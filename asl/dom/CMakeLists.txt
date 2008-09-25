
set(SOURCE
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

if(APPLE)
  set(SOURCE ${SOURCE})
  set(HEADERS ${HEADERS})
endif(APPLE)

if(LINUX)
  set(SOURCE ${SOURCE})
  set(HEADERS ${HEADERS})
endif(LINUX)

if(WIN32)
  set(SOURCE ${SOURCE})
  set(HEADERS ${HEADERS})
endif(WIN32)

add_library(asldom SHARED ${SOURCE})
set_target_properties(asldom PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_link_libraries(asldom aslbase)

install(
  TARGETS asldom
  LIBRARY
    DESTINATION lib
  PUBLIC_HEADER 
    DESTINATION include/asl/dom
)

foreach(test ${TESTS})
  add_executable(test${test} test${test}.tst.cpp)
  target_link_libraries(test${test} asldom)
  add_test(${test} test${test})
endforeach(test)

