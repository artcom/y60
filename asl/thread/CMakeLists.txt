
set(SOURCES
)

set(HEADERS
asl_thread_settings.h
concurrent_queue.h
)

set(TESTS
_concurrent_queue
)

find_package(Boost COMPONENTS thread)

set(EXTERNS Boost)

ac_add_library(
    aslthread asl/thread
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS ${EXTERNS}
    TESTS   ${TESTS}
)

