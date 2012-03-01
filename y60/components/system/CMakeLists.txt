
add_subdirectory(EventLoop)
add_subdirectory(ProcessFunctions)
add_subdirectory(TimeSyncBarrier)

if(WIN32)
    #add_subdirectory(TaskManager)
endif(WIN32)

