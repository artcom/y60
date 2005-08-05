//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: CTSlice.h,v $
//   $Author: martin $
//   $Revision: 1.6 $
//   $Date: 2005/04/21 10:00:39 $
//
//
//=============================================================================

#ifndef _ac_y60_components_dicomfile_h_
#define _ac_y60_components_dicomfile_h_

#include "CTFile.h"
#include <dom/Value.h>
#include <asl/Exception.h>

class DcmFileFormat;

namespace y60 {

class DicomFile : public CTFile {
    public:
        DicomFile(asl::Ptr<asl::ReadableBlock> theInputBlock, const std::string & theFilename);
        virtual ~DicomFile();
        virtual int getFrameCount() const;
        virtual float getZPos(int theFrame) const;
        virtual dom::ResizeableRasterPtr getRaster(int theFrame);
        virtual asl::Vector2f getDefaultWindow() const;
        virtual asl::Vector3f getVoxelSize() const;
        virtual y60::PixelEncoding getEncoding() const;
    private:
        DcmFileFormat * _myDicomFile;
        asl::Vector3f _myVoxelSize;
        asl::Vector3f _myImagePosition;
        asl::Vector2f _myDefaultWindow;
        int _myFrameCount;
};

typedef asl::Ptr<DicomFile> DicomFilePtr;

}
#endif


