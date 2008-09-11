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
//   $RCSfile: VertexArray.h,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2005/04/21 16:25:02 $
//
//  Description: Self-Managed Memory, maybe AGP or Main Memory from a fixed size pool
//
//=============================================================================

#ifndef _ac_gl_vertex_array_h
#define _ac_gl_vertex_array_h

#include <y60/scene/VertexDataImpl.h>
#include "GLAlloc.h"

namespace y60 {

template <class T>
class VertexArray : public VertexDataImpl<T, std::vector<T, asl::GLAlloc<T> > > {
    typedef std::vector<T, asl::GLAlloc<T> > VertexDataVector;
    public:
        void lock() { 
            asl::AGPMemoryFlushSingleton::get().flushGLAGPMemory();
        }
        void unlock() {}
        void useAsPosition() const {
            glVertexPointer(3, GL_FLOAT, 0, this->getDataPtr());
            CHECK_OGL_ERROR;
        }
        void useAsNormal() const {
            glNormalPointer(GL_FLOAT, 0, this->getDataPtr());
            CHECK_OGL_ERROR;
        }
        void useAsColor() const {
            glColorPointer(4, GL_FLOAT, 0, this->getDataPtr());
            CHECK_OGL_ERROR;
        }
        void useAsTexCoord() const {
            glTexCoordPointer(this->getElementSize(), GL_FLOAT, 0, this->getDataPtr());
            CHECK_OGL_ERROR;
        }

};

}

#endif

