cmake_minimum_required(VERSION 2.6.2)

set( HEADERS Pre-built/include/semaphore.h
             Pre-built/include/pthread.h
             Pre-built/include/sched.h )

set( LIBRARIES Pre-built/lib/pthreadVC.dll
               Pre-built/lib/pthreadVC.lib )

install( FILES ${HEADERS} DESTINATION include )
install( FILES ${LIBRARIES} DESTINATION lib )

