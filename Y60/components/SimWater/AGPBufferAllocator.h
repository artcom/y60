//============================================================================
//
// Copyright (C) 2002-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef Y60_WATER_AGP_BUFFER_ALLOCATOR_INCLUDED
#define Y60_WATER_AGP_BUFFER_ALLOCATOR_INCLUDED

#include "WaterRepresentation.h"


#include <y60/GLUtils.h>

#define USE_AGP_MEMORY
//#define AC_USE_X11


#ifdef AC_USE_X11
// hack to allow old glx header without the prototypes as well as new ones
extern "C" {   
#ifndef PFNGLXALLOCATEMEMORYNVPROC
typedef void * ( * PFNGLXALLOCATEMEMORYNVPROC) (int size, float readfreq, float writefreq, float priority);
#endif

#ifndef PFNGLXFREEMEMORYNVPROC
typedef void ( * PFNGLXFREEMEMORYNVPROC) (GLvoid *pointer);
#endif


PFNGLXALLOCATEMEMORYNVPROC _ac_glXAllocateMemoryNV = 
    (PFNGLXALLOCATEMEMORYNVPROC) glXGetProcAddressARB((const GLubyte *) "glXAllocateMemoryNV");
#define glXAllocateMemoryNV _ac_glXAllocateMemoryNV

PFNGLXFREEMEMORYNVPROC _ac_glXFreeMemoryNV = 
    (PFNGLXFREEMEMORYNVPROC) glXGetProcAddressARB((const GLubyte *) "glXFreeMemoryNV");
#define glXFreeMemoryNV _ac_glXFreeMemoryNV

}

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

        _buffer = (GLfloat *)glXAllocateMemoryNV(numBytes, 0, 0, priority);
        if(_buffer) {
            AC_DEBUG << "Allocated " << megabytes << " megabytes of fast AGP memory";
            _isAGPMem = true;
            memset(_buffer, 0, numBytes);
            return _buffer;
        }

        if(!_buffer) {
            _buffer = (GLfloat *)glXAllocateMemoryNV(numBytes, 0, 0, 1.f);
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
