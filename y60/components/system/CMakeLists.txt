
add_subdirectory(EventLoop)
add_subdirectory(ProcFunctions)

if(WIN32)
    add_subdirectory(TaskManager)
endif(WIN32)

