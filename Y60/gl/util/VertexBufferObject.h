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
//   $RCSfile: VertexBufferObject.h,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2005/04/21 16:25:02 $
//
//  Description: OpenGL 1.5 Vertex Buffer Objects
//
//=============================================================================

#ifndef _ac_gl_vertex_data_buffer_h
#define _ac_gl_vertex_data_buffer_h

#include "VertexDataImpl.h"

namespace y60 {
    
    template <class T>
    class VertexBufferObject : public VertexDataImpl<T, VertexVector<T> > {
        typedef VertexVector<T> VertexDataVector;
    public:
        VertexBufferObject(VertexBufferUsage theUsageType) {
            this->getDataVector().setUsage(theUsageType);
        }
        void lock(bool forWriting = true, bool forReading = false) const {
            this->getDataVector().lock(forWriting, forReading);
        }
        void unlock() const {
            this->getDataVector().unlock();
        }
        void useAsPosition() const {
            //std::cerr << "useAsPosition():";
            //_myData.print(std::cerr);
            this->getDataVector().bindUnlocked();
            glVertexPointer(3, GL_FLOAT, 0, 0);
            CHECK_OGL_ERROR;
        }
        void useAsNormal() const {
            this->getDataVector().bindUnlocked();
            glNormalPointer(GL_FLOAT, 0, 0);
            CHECK_OGL_ERROR;
        }
        void useAsColor() const {
            this->getDataVector().bindUnlocked();
            glColorPointer(4, GL_FLOAT, 0, 0);
            CHECK_OGL_ERROR;
        }
        void useAsTexCoord() const {
            this->getDataVector().bindUnlocked();
            glTexCoordPointer(this->getElementSize(), GL_FLOAT, 0, 0);
            CHECK_OGL_ERROR;
        }
    };



}


#endif

