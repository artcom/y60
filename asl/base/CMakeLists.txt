
svn_generate_cxx(../ Revision.cpp asl ourRevision)

set(SOURCE
Arguments.cpp
Block.cpp
console_functions.cpp
Dashboard.cpp
Exception.cpp
ExitHandler.cpp
file_functions.cpp
Logger.cpp
LogMessageFormatters.cpp
LogMessageSinks.cpp
MultiThreadedUnitTest.cpp
os_functions.cpp
Path.cpp
PlugInBase.cpp
PlugInManager.cpp
PosixThread.cpp
proc_functions.cpp
Ptr.cpp
ReadWriteLock.cpp
Revision.cpp
signal_functions.cpp
SingletonManager.cpp
StackTrace.cpp
StdOutputRedirector.cpp
Stream.cpp
string_functions.cpp
ThreadHelper.cpp
ThreadLock.cpp
ThreadSemaphore.cpp
Time.cpp
TimeStreamFormater.cpp
TraceUtils.cpp
UnitTest.cpp
)

set(HEADERS
Arguments.h
Assure.h
AtomicCount.h
Auto.h
begin_end.h
Block.h
checked_delete.h
checksum.h
console_functions.h
Dashboard.h
Enum.h
error_functions.h
Exception.h
ExitHandler.h
file_functions.h
GarbageCollector.h
ITimeSource.h
Logger.h
LogMessageFormatters.h
LogMessageSinks.h
MappedBlock.h
MemoryPool.h
MultiThreadedUnitTest.h
os_functions.h
Path.h
PlugInBase.h
PlugInManager.h
PosixThread.h
proc_functions.h
Ptr.h
RangeSet.h
ReadWriteLock.h
Revision.h
RunLengthEncoder.h
settings.h
signal_functions.h
SingletonBase.h
Singleton.h
SingletonManager.h
StackTrace.h
static_dict.h
static_vector.h
StdOutputRedirector.h
Stream.h
string_functions.h
ThreadFifo.h
ThreadHelper.h
ThreadLock.h
ThreadSemaphore.h
Time.h
TimeStreamFormater.h
TraceUtils.h
UnitTest.h
)

set(TESTS
Arguments
Assure
AtomicCount
Auto
Block
Dashboard
Enum
Exception
_file_functions
GarbageCollector
Itimer
Logger
MappedBlock
MemoryPool
MultiThreadedUnitTest
MyClass
_os_functions
Path
PosixThread
PrioCeilingRWLock
_proc_functions
Ptr
ReadWriteLock
RunLengthEncoder
ShellScript
StackTrace
Stream
_string_functions
#ThreadFifo
ThreadLock
ThreadSemaphore
Time
UnitTest
)

if(APPLE)
  set(SOURCE ${SOURCE} MachOBacktrace.cpp)
  set(HEADERS ${HEADERS} MachOBacktrace.h)
endif(APPLE)

if(LINUX)
  set(SOURCE ${SOURCE} GlibcBacktrace.cpp)
  set(HEADERS ${HEADERS} GlibcBacktrace.h)
endif(LINUX)

if(WIN32)
  set(SOURCE ${SOURCE} Win32Backtrace.cpp)
  set(HEADERS ${HEADERS} Win32Backtrace.h)
endif(WIN32)

add_library(aslbase SHARED ${SOURCE})
set_target_properties(aslbase PROPERTIES PUBLIC_HEADER "${HEADERS}")
link_directories(${GLIB2_LIBRARY_DIRS})
include_directories(${GLIB2_INCLUDE_DIRS})
target_link_libraries(aslbase ${GLIB2_LIBRARIES})

install(
  TARGETS aslbase
  LIBRARY
    DESTINATION lib
  PUBLIC_HEADER 
    DESTINATION include/asl/base
)

foreach(test ${TESTS})
  add_executable(test${test} test${test}.tst.cpp)
  target_link_libraries(test${test} aslbase)
  add_test(${test} test${test})
endforeach(test)

