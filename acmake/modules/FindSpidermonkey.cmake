
include(${CMAKE_SOURCE_DIR}/netsrc/spidermonkey/SpidermonkeyDefinitions.cmake)
include(${CMAKE_SOURCE_DIR}/netsrc/spidermonkey/SpidermonkeyDependencies.cmake)

set(SPIDERMONKEY_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/netsrc/spidermonkey/")
set(SPIDERMONKEY_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/netsrc/spidermonkey/include/")
set(SPIDERMONKEY_LIBRARIES spidermonkey)

