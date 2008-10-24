
set(SOURCES
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
#GarbageCollector
Itimer
Logger
MappedBlock
MemoryPool
MultiThreadedUnitTest
MyClass
_os_functions
Path
PlugInManager
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

if(OSX)
  list(APPEND SOURCES MachOBacktrace.cpp)
  list(APPEND HEADERS MachOBacktrace.h)
endif(OSX)

if(LINUX)
  list(APPEND SOURCES GlibcBacktrace.cpp)
  list(APPEND HEADERS GlibcBacktrace.h)
  list(APPEND EXTERNALS glib)
endif(LINUX)

if(WIN32)
  list(APPEND SOURCES Win32Backtrace.cpp)
  list(APPEND HEADERS Win32Backtrace.h)
endif(WIN32)


ac_add_library(
  base
  SOURCES ${SOURCES}
  HEADERS ${HEADERS}
  EXTERNS GLIB2
  TESTS   ${TESTS}
)

ac_add_plugin(
  testpluginOPT
  SOURCES testPlugIn.cpp
  HEADERS testPlugIn.h
)
