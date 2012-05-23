
if(Boost_VERSION GREATER 104099)
    # we need (boost-sprit >= 2.1), which is in Boost 1.41
    add_subdirectory(CSVImporter)
endif(Boost_VERSION GREATER 104099)

