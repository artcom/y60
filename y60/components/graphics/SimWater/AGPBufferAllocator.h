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

#ifndef Y60_WATER_AGP_BUFFER_ALLOCATOR_INCLUDED
#define Y60_WATER_AGP_BUFFER_ALLOCATOR_INCLUDED

#include "WaterRepresentation.h"

#include <y60/glutil/GLUtils.h>

//#define USE_AGP_MEMORY

#if defined(LINUX) || defined(OSX)
    #define AC_USE_X11
#include <GL/glxew.h>
#endif

namespace y60 {

class AGPBufferAllocator : public BufferAllocator {
public:
    AGPBufferAllocator() :
        _buffer(0),
        _isAGPMem(false)
    {
    }

    virtual ~AGPBufferAllocator() {
    }

    // old buffer size: BUFFER_SIZE*sizeof(GLfloat)
    virtual void *  allocateSingleBuffer(unsigned int numBytes) {
        assert(_buffer == 0);

        float megabytes = (numBytes/1000000.f);

#ifdef USE_AGP_MEMORY

        float priority = .5f;

#ifdef WIN32
        _buffer = (GLfloat *)wglAllocateMemoryNV(numBytes, 0, 0, priority);
#else
        _buffer = (GLfloat *)glXAllocateMemoryNV(numBytes, 0, 0, priority);
#endif

        if(_buffer) {
            AC_DEBUG << "Allocated " << megabytes << " megabytes of fast AGP memory";
            _isAGPMem = true;
            memset(_buffer, 0, numBytes);
            return _buffer;
        }
        if(!_buffer) {
#ifdef WIN32
            _buffer = (GLfloat *)wglAllocateMemoryNV(numBytes, 0, 0, 1.f);
#else
            _buffer = (GLfloat *)glXAllocateMemoryNV(numBytes, 0, 0, 1.f);
#endif

        }

        if(_buffer) {
            memset(_buffer, 0, numBytes);
            AC_DEBUG << "Allocated " << megabytes << " megabytes of fast AGP memory";
            _isAGPMem = true;
            return _buffer;
        }

        // fall back to normal system memory
        AC_DEBUG << "Failed to allocate " << megabytes << " megabytes of fast AGP memory";
#endif
        AC_DEBUG << "Allocated " << megabytes << " megabytes of normal heap mem";
        _buffer = (GLfloat *)malloc(numBytes);
        assert(_buffer);
        _isAGPMem = false;
        return _buffer;
    }

    virtual bool    freeSingleBuffer() {
        assert(_buffer);
        if (_buffer) {
            if (_isAGPMem) {
            } else {
                free(_buffer);
            }
            _buffer = 0;
        }
        return true;
    }

private:
    void *  _buffer;
    bool    _isAGPMem;
};

typedef asl::Ptr<AGPBufferAllocator> AGPBufferAllocatorPtr;

} // end of namespace y60

#endif // Y60_WATER_AGP_BUFFER_ALLOCATOR_INCLUDED
