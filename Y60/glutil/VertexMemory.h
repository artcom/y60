//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_scene_VertexMemory_h
#define _ac_scene_VertexMemory_h

#include <y60/scene/VertexDataImpl.h>
#include <y60/scene/VertexMemoryBase.h>

namespace y60 {
    // Standard Main Memory managed by new() and delete() on heap
    template <class T>
    class VertexMemory : public VertexMemoryBase<T> {
    public:
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
