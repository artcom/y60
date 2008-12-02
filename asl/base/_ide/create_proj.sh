#!/bin/bash 

function createprj() {
	echo creating asl-base-test$1.vcproj
	cat asl-base-testTempl.vcproj | sed "s/Template/$1/" > asl-base-test$1.vcproj
}

createprj "Arguments"
createprj "Assure"
createprj "AtomicCount"
createprj "Auto"
createprj "Block"
createprj "Dashboard"
createprj "Enum"
createprj "Exception"
createprj "GarbageCollector"
createprj "Logger"
createprj "MappedBlock"
createprj "MemoryPool"
createprj "MultiThreadedUnitTest"
createprj "MyClass"
createprj "Path"
createprj "PosixThread"
createprj "Ptr"
createprj "ReadWriteLock"
createprj "RunLengthEncoder"
createprj "StackTrace"
createprj "Stream"
createprj "ThreadLock"
createprj "Time"
createprj "UnitTest"
createprj "_file_functions"
createprj "_os_functions"
createprj "_proc_functions"
createprj "_string_functions"
