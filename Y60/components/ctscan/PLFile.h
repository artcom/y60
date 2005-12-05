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
//   $RCSfile: PLFile.h,v $
//   $Author: christian $
//   $Revision: 1.5 $
//   $Date: 2005/03/24 23:35:59 $
//
//
//=============================================================================

#ifndef _ac_y60_components_plslice_h_
#define _ac_y60_components_plslice_h_

#include "CTFile.h"
#include <asl/Block.h>
#include <y60/ImageLoader.h>
#include <dom/Nodes.h>

namespace y60 {

class PLFile : public CTFile {
    public:
        PLFile(asl::Ptr<asl::ReadableBlock> theInputData, const std::string & theFilename);
        virtual ~PLFile();
        virtual int getFrameCount() const { return 1; };
        virtual float getZPos(int theFrame) const;
        virtual dom::ResizeableRasterPtr getRaster(int theFrame);
        virtual y60::PixelEncoding getEncoding() const;
    private:
        mutable ImageLoader _myBitmap; // paintlib not const-correct
        std::string _myFilename;
};

typedef asl::Ptr<PLFile> PLFilePtr;

}
#endif


