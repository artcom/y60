
add_subdirectory(StlImporter)
add_subdirectory(X3dImporter)

if(DCMTK_FOUND)
    # These two need adjustments to work with current dcmtk.
    #add_subdirectory(CTScan)
    #add_subdirectory(Dicom)
endif(DCMTK_FOUND)

