
set(SOURCES
    DICOMPlugIn.cpp
    JSDicomImage.cpp
    pldicomdec.cpp
)

set(HEADERS
    JSDicomImage.h
    pldicomdec.h
    y60_dicom_settings.h
)

y60_add_component(
    y60Dicom
    SOURCES ${SOURCES}
    HEADERS ${HEADERS}
    DEPENDS y60jslib
    EXTERNS DCMTK PAINTLIB
)

