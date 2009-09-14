
set(SOURCES
)

set(HEADERS
asl_thread_settings.h
concurrent_queue.h
completion.h
)

set(TESTS
_concurrent_queue
_completion
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

