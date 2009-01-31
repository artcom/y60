
set(SOURCES
Arguments.cpp
Block.cpp
console_functions.cpp
Dashboard.cpp
Exception.cpp
ExitHandler.cpp
file_functions.cpp
initialization.cpp
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
asl_base_settings.h
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
initialization.h
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
Logger
MappedBlock
MemoryPool
MultiThreadedUnitTest
MyClass
_os_functions
Path
PlugInManager
PosixThread
_proc_functions
Ptr
ReadWriteLock
RunLengthEncoder
StackTrace
Stream
_string_functions
#ThreadFifo
ThreadLock
ThreadSemaphore
Time
UnitTest
)

if(UNIX)
    list(APPEND SOURCES signal_functions.cpp)
	list(APPEND HEADERS signal_functions.h)
    list(APPEND TESTS Itimer PrioCeilingRWLock ShellScript)
endif(UNIX)

if(OSX)
    list(APPEND SOURCES MachOBacktrace.cpp)
    list(APPEND HEADERS MachOBacktrace.h)
    list(APPEND EXTERNS ${CARBON}) # XXX: framework reference
endif(OSX)

if(LINUX)
    list(APPEND SOURCES GlibcBacktrace.cpp)
    list(APPEND HEADERS GlibcBacktrace.h)
    list(APPEND EXTERNS GLIB2)
endif(LINUX)

if(WIN32)
    list(APPEND SOURCES Win32Backtrace.cpp)
    list(APPEND HEADERS Win32Backtrace.h)
	set(PSAPI_LIBRARIES Psapi) # XXX: write find package
    list(APPEND EXTERNS PTHREADVC PSAPI)
endif(WIN32)

ac_add_library(
    aslbase asl/base
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    EXTERNS ${EXTERNS}
    TESTS   ${TESTS}
)

ac_add_plugin(
    asltestplugin asl/testplugin
    SOURCES testPlugIn.cpp
    HEADERS testPlugIn.h
    DONT_INSTALL
)

ac_add_executable(
  text2src
  SOURCES text2src.cpp
  DEPENDS aslbase
  DONT_INSTALL
)
