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
//   $RCSfile: CTSlice.h,v $
//   $Author: martin $
//   $Revision: 1.6 $
//   $Date: 2005/04/21 10:00:39 $
//
//
//=============================================================================

#ifndef _ac_y60_components_ctfile_h_
#define _ac_y60_components_ctfile_h_

#include <dom/Value.h>
#include <asl/Exception.h>
#include <y60/PixelEncoding.h>

namespace y60 {

DEFINE_EXCEPTION(CTException, asl::Exception);

class CTFile {
    public:
        virtual ~CTFile() {};
        /** returns the number of frames in the file */
        virtual int getFrameCount() const = 0;
        virtual float getZPos(int theFrame) const = 0;
        virtual dom::ResizeableRasterPtr getRaster(int theFrame) = 0;
        /** returns the default window center/width */
        virtual asl::Vector2f getDefaultWindow() const;
        
        virtual y60::PixelEncoding getEncoding() const = 0;
        /** returns the size of a single voxel (in meters) in x/y/z direction. 
         *  Any component may be 0 = unknown. */
        virtual asl::Vector3f getVoxelSize() const { return asl::Vector3f(0,0,0); };
};

typedef asl::Ptr<CTFile> CTFilePtr;

}
#endif

