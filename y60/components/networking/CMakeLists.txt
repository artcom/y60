
add_subdirectory(Cluster)
add_subdirectory(CMSCache)
add_subdirectory(Nagios)
add_subdirectory(Network)
add_subdirectory(OscReceiver)
add_subdirectory(OscSender)
add_subdirectory(SynergyServer)
add_subdirectory(HttpServer)
if(LINUX)
   # Note: waiting for libcurl 7.12.7 or higher on this plattform
else(LINUX)
add_subdirectory(Async)
endif(LINUX)

