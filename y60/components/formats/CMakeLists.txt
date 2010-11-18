
add_subdirectory(StlImporter)
add_subdirectory(X3dImporter)
add_subdirectory(OpenEXR)

if(Boost_VERSION GREATER 104099)
    # we need (boost-sprit >= 2.1), which is in Boost 1.41
    add_subdirectory(CSVImporter)
endif(Boost_VERSION GREATER 104099)

if(DCMTK_FOUND)
    # These two need adjustments to work with current dcmtk.
    #add_subdirectory(CTScan)
    #add_subdirectory(Dicom)
endif(DCMTK_FOUND)

