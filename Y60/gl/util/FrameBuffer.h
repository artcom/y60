//============================================================================
//
// Copyright (C) 2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: FrameBuffer.h,v $
//
//     $Author: danielk $
//
//   $Revision: 1.5 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef AC_Y60_FRAMEBUFFER_INCLUDED
#define AC_Y60_FRAMEBUFFER_INCLUDED

#include "GLContext.h"
#include <asl/Exception.h>
#include <asl/Ptr.h>

#include <y60/IFrameBuffer.h>

#include <vector>
#include <string>

namespace y60 {
    /// opengl framebuffer
    class FrameBuffer : public IFrameBuffer {
    public:
        DEFINE_NESTED_EXCEPTION(FrameBuffer, Exception, asl::Exception);

        virtual bool activate() {
            return getContext().activate();
        }
        virtual DrawingContext & getContext() = 0;
        virtual const DrawingContext & getContext() const = 0;
    };

} // namespace
#endif // AC_Y60_PBUFFER
