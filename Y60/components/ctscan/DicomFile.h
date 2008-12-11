/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
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
#include <asl/dom/Value.h>
#include <asl/base/Exception.h>

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


