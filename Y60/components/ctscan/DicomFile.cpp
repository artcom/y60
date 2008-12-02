//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: DicomFile.cpp,v $
//   $Author: martin $
//   $Revision: 1.8 $
//   $Date: 2005/04/21 10:00:39 $
//
//   Description: MarchingCubes - Based on Mesch's implementation; check out
//                Vidimed from the repository for the original.
//=============================================================================

#include "DicomFile.h"

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plsubbmp.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include <asl/base/Block.h>
#include <asl/base/MappedBlock.h>
#include <y60/image/PLFilterFactory.h>
#include <y60/image/PLFilterResizePadded.h>
#include <y60/image/Image.h>

#include <dicom/dcmdata/dcistrmb.h>
#include <dicom/dcmdata/dcistrmf.h>
#include <dicom/dcmdata/dcfilefo.h>
#include <dicom/dcmdata/dcdict.h>
#include <dicom/dcmdata/dcmetinf.h>
#include <dicom/dcmdata/dcdeftag.h>
#include <dicom/dcmimgle/dcmimage.h>

using namespace asl;
using namespace std;

namespace y60 {

DicomFile::DicomFile(Ptr<ReadableBlock> theInputBlock, const std::string & theFilename)
    : _myDicomFile(0), CTFile(), _myFrameCount(1)
{
    DicomImageClass::setDebugLevel(DicomImageClass::DL_Errors);
    if (!dcmDataDict.isDictionaryLoaded()) {
        throw CTException("No DCTMK data dictionary loaded", PLUS_FILE_LINE);
    }

    DcmInputBufferStream myInputAdapter;
    myInputAdapter.setBuffer(theInputBlock->begin(), theInputBlock->size());
    myInputAdapter.setEos();

    _myDicomFile = new DcmFileFormat();

    _myDicomFile->transferInit();
    OFCondition myRetVal = _myDicomFile->read(myInputAdapter);
    _myDicomFile->transferEnd();

    OFString tempStr;
    if (_myDicomFile->getDataset()->tagExists(DCM_NumberOfFrames)) {
        _myDicomFile->getDataset()->findAndGetOFString(DCM_NumberOfFrames, tempStr);
        _myFrameCount = as<int>(tempStr.c_str());
    }
    if (_myDicomFile->getDataset()->tagExists(DCM_PixelSpacing)) {
        _myDicomFile->getDataset()->findAndGetOFString(DCM_PixelSpacing, tempStr, 0);
        _myVoxelSize[0] = as<float>(tempStr.c_str())/1000.0;
        _myDicomFile->getDataset()->findAndGetOFString(DCM_PixelSpacing, tempStr, 1);
        _myVoxelSize[1] = as<float>(tempStr.c_str())/1000.0;
    }
    if (_myDicomFile->getDataset()->tagExists(DCM_SliceThickness)) {
        _myDicomFile->getDataset()->findAndGetOFString(DCM_SliceThickness, tempStr);
        _myVoxelSize[2] = as<float>(tempStr.c_str())/1000.0;
    }
    if (_myDicomFile->getDataset()->tagExists(DCM_ImagePositionPatient)) {
        _myDicomFile->getDataset()->findAndGetOFString(DCM_ImagePositionPatient, tempStr, 0);
        _myImagePosition[0] = as<float>(tempStr.c_str())/1000.0;
        _myDicomFile->getDataset()->findAndGetOFString(DCM_ImagePositionPatient, tempStr, 1);
        _myImagePosition[1] = as<float>(tempStr.c_str())/1000.0;
        _myDicomFile->getDataset()->findAndGetOFString(DCM_ImagePositionPatient, tempStr, 2);
        _myImagePosition[2] = as<float>(tempStr.c_str())/1000.0;
    }

    _myDefaultWindow = CTFile::getDefaultWindow();

    if (_myDicomFile->getDataset()->tagExists(DCM_WindowCenter)) {
        _myDicomFile->getDataset()->findAndGetOFString(DCM_WindowCenter, tempStr);
        _myDefaultWindow[0] = as<float>(tempStr.c_str());
    }
    if (_myDicomFile->getDataset()->tagExists(DCM_WindowWidth)) {
        _myDicomFile->getDataset()->findAndGetOFString(DCM_WindowWidth, tempStr);
        _myDefaultWindow[1] = as<float>(tempStr.c_str());
    }
}

y60::PixelEncoding
DicomFile::getEncoding() const {
    E_TransferSyntax xfer = _myDicomFile->getDataset()->getOriginalXfer();
    // unsigned long opt_compatibilityMode = CIF_MayDetachPixelData | CIF_TakeOverExternalDataset;
    unsigned long opt_compatibilityMode = 0;
    DicomImage myDicomImage(_myDicomFile, xfer, opt_compatibilityMode, 0lu, 1lu);

    if (myDicomImage.getStatus() != EIS_Normal) {
        throw CTException(myDicomImage.getString(myDicomImage.getStatus()), PLUS_FILE_LINE);
    }

    const DiPixel * myPixelData = myDicomImage.getInterData();
    switch (myPixelData->getRepresentation()) {
        case EPR_Uint8:
            AC_TRACE << "dicom file is gray-8";
            return y60::GRAY;
        case EPR_Uint16:
            AC_TRACE << "dicom file is gray-U16";
            return y60::GRAY16;
        case EPR_Sint16:
            AC_TRACE << "dicom file is gray-S16";
            return y60::GRAYS16;
        default:
            throw CTException("unsupported dctk pixel format", PLUS_FILE_LINE);
    }
}

dom::ResizeableRasterPtr 
DicomFile::getRaster(int theFrame) {
    E_TransferSyntax xfer = _myDicomFile->getDataset()->getOriginalXfer();
    DicomImage myDicomImage(_myDicomFile, xfer, 0lu, static_cast<unsigned long>(theFrame), 1lu);

    if (myDicomImage.getStatus() != EIS_Normal) {
        throw CTException(myDicomImage.getString(myDicomImage.getStatus()), PLUS_FILE_LINE);
    }

    const DiPixel * myPixelData = myDicomImage.getInterData();
    PixelEncoding myEncoding = getEncoding();

    const unsigned char * myRawData = reinterpret_cast<const unsigned char*>(myPixelData->getData());

    asl::ReadableBlockAdapter myRawBlock(myRawData, myRawData + getBytesRequired(myPixelData->getCount(), myEncoding)); 
    AC_TRACE << "creating frame " << theFrame << ", " << myDicomImage.getWidth() << "x" << myDicomImage.getHeight();
    return dynamic_cast_Ptr<dom::ResizeableRaster>(
            createRasterValue(myEncoding, myDicomImage.getWidth(), myDicomImage.getHeight(), myRawBlock));
}

DicomFile::~DicomFile() {
    if (_myDicomFile) {
        delete _myDicomFile;
    }
}

int 
DicomFile::getFrameCount() const {
    return _myFrameCount;
};

asl::Vector3f 
DicomFile::getVoxelSize() const {
    return _myVoxelSize;
}

float 
DicomFile::getZPos(int theFrame) const {
    return _myImagePosition[2]+_myVoxelSize[2]*theFrame;
}

asl::Vector2f
DicomFile::getDefaultWindow() const {
    return _myDefaultWindow;
}

}

