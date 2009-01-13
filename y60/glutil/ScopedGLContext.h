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

#ifndef _y60_gl_scopedglcontext_included_
#define _y60_gl_scopedglcontext_included_

#include "y60_glutil_settings.h"

#include "IGLContextManager.h"

namespace y60 {

class ScopedGLContext {
    public:
        ScopedGLContext(IGLContextManager * theManager) {
            _myManager = theManager;
            _myManager->activateGLContext();
        }
        virtual ~ScopedGLContext() {
            _myManager->deactivateGLContext();
        };
    private:
        IGLContextManager * _myManager;
        ScopedGLContext();
        ScopedGLContext(const ScopedGLContext &);
        ScopedGLContext & operator=(const ScopedGLContext &);
};

} // namespace 

#endif

