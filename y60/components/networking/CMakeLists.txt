
add_subdirectory(Cluster)
add_subdirectory(CMSCache)
add_subdirectory(Nagios)
add_subdirectory(Network)
add_subdirectory(OscReceiver)
add_subdirectory(OscSender)
add_subdirectory(SynergyServer)
add_subdirectory(HttpServer)
if(CURL_VERSION VERSION_GREATER 7.21.6)
    add_subdirectory(Async)
endif(CURL_VERSION VERSION_GREATER 7.21.6)

