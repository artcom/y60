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
*/

#ifndef _asl_included_asl_GLAlloc_h_
#define _asl_included_asl_GLAlloc_h_

#include "y60_glutil_settings.h"

#include <limits>
#include <iostream>
#include <map>
#include <limits.h>

#include <asl/base/Block.h>
#include <asl/base/Ptr.h>
#include <asl/base/Singleton.h>
#include <asl/base/Logger.h>

#include "GLUtils.h"

#define DB2(x) //  x

namespace y60 {
    class Primitive;
}
namespace asl {
    bool Y60_GLUTIL_DECL initializeGLMemoryExtensions();
    struct Y60_GLUTIL_DECL AGPMemoryFlushSingleton : public asl::Singleton<AGPMemoryFlushSingleton>
    {
        void resetGLAGPMemoryFlush();
        void flushGLAGPMemory();
        bool  _myAGPMemoryIsFlushed;
    };
    class Y60_GLUTIL_DECL GLMemory : public FixedCapacityBlock {
    public:
        enum MemoryType {MainMemoryType=0, AGPMemoryType=1, GPUMemoryType=2};
        GLMemory(MemoryType theMemoryType)
            : _myMem(0),
            _mySize(0),
            _myCapacity(0),
            _myMemoryType(theMemoryType),
            _myHint(_myFreeChunks.end()),
            _myTotalUsedBytes(0),
            _myFreeStoreBytes(0)
        {}

        operator const void*() const {
            return _myMem;
        }
        bool claim(asl::AC_SIZE_TYPE theCapacity);
        void release();
        bool enableDMA();
        void disableDMA();
        void resize(asl::AC_SIZE_TYPE theSize) {
            if (_myCapacity >= theSize) {
                _mySize = theSize;
            }
        }
        unsigned char * begin() {
            return _myMem;
        }
        const unsigned char * begin() const {
            return _myMem;
        }
        unsigned char * end() {
            return _myMem + _mySize;
        }
        const unsigned char * end() const {
            return _myMem + _mySize;
        }
        asl::AC_SIZE_TYPE size() const {
            return _mySize;
        }
        asl::AC_SIZE_TYPE capacity() const {
            return _myCapacity;
        }

        ~GLMemory() {
            std::cerr << "~GLMemory()" << std::endl;
            if (_myMem) {
                //disableDMA();
                release();
            }
        }
        unsigned char * allocateChunk(asl::AC_SIZE_TYPE theRequiredBytes);
        void deallocateChunk(unsigned char * theChunk);

    private:
        unsigned char * _myMem;
        asl::AC_SIZE_TYPE _mySize;
        asl::AC_SIZE_TYPE _myCapacity;
        MemoryType _myMemoryType;

        typedef std::map<unsigned char *,asl::AC_SIZE_TYPE> ChunkMap;
        ChunkMap _myFreeChunks;
        ChunkMap _myUsedChunks;
        ChunkMap::iterator _myHint;
        asl::AC_SIZE_TYPE _myTotalUsedBytes;
        asl::AC_SIZE_TYPE _myFreeStoreBytes;
    };

    //typedef asl::Ptr<GLMemory> GLMemoryPtr;
    typedef GLMemory * GLMemoryPtr;

    extern GLMemoryPtr AGPMemory();
    extern GLMemoryPtr GPUMemory();
    extern GLMemoryPtr MainMemory();

    // Set Y60_AGP_VERTEX_MEMORY to default vertex memory size in MB
    // If Y60_AGP_VERTEX_MEMORY is "0", AGP-Memory will be disabled
    // If Y60_AGP_VERTEX_MEMORY is undefined, up to 64 MB will be used
    struct AGPVertexArray {
        enum { DISABLED = UINT_MAX , UNKNOWN = UINT_MAX-1 };

        static unsigned getDefaultCapacity() {
            static unsigned myDefaultCapacity = static_cast<unsigned>(UNKNOWN);
            if (myDefaultCapacity == UNKNOWN) {
                char * myCapacityString = ::getenv("Y60_AGP_VERTEX_MEMORY");
                if (myCapacityString) {
                    unsigned myCapacity = asl::as<unsigned>(myCapacityString);
                    if (myCapacity > 0) {
                        myDefaultCapacity = myCapacity * 1024 * 1024;
                    } else {
                        myDefaultCapacity = static_cast<unsigned>(DISABLED);
                    }
                } else {
                    AC_INFO << "Y60_AGP_VERTEX_MEMORY not set - guessing hardware..";
                    std::string myHardwareString(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
                    AC_INFO << "found '" << myHardwareString << "'";
                    if (myHardwareString.find("/AGP") == std::string::npos) {
                        AC_INFO << "no AGP found, using main memory." << std::endl;
                        myDefaultCapacity = static_cast<unsigned>(DISABLED);
                    } else {
                        AC_INFO << "AGP found." << std::endl;
                        myDefaultCapacity = 64 * 1024 * 1024;
                    }
                }
            }
            return myDefaultCapacity;
        }
    };

    extern Y60_GLUTIL_DECL GLMemoryPtr VertexMemory(GLMemory::MemoryType theMemoryType = GLMemory::AGPMemoryType,
                                    asl::AC_SIZE_TYPE theCapacity=0);

    template <typename T>
    class GLAlloc
    {
    public:
        // Typedefs
        typedef asl::AC_SIZE_TYPE    size_type;
        typedef asl::AC_OFFSET_TYPE  difference_type;
        typedef T*        pointer;
        typedef const T*  const_pointer;
        typedef T&        reference;
        typedef const T&  const_reference;
        typedef T         value_type;
        template<typename U> friend class GLAlloc;
    public:
        // Constructors

        GLAlloc(GLMemory::MemoryType theMem = GLMemory::AGPMemoryType, //GLAlloc(GLMemory::MemoryType theMem = GLMemory::GPUMemoryType, GLAlloc(GLMemory::MemoryType theMem = GLMemory::MainMemoryType,
                asl::AC_SIZE_TYPE theCapacity = AGPVertexArray::getDefaultCapacity())
            : _myType(theMem),
            _myMem(VertexMemory(_myType,theCapacity)),
            _myGfxMemAllocatedBytes(0)
        {}

        GLAlloc(const GLAlloc& theOther) throw()
            : _myType(theOther._myType),
            _myMem(theOther._myMem),
            _myGfxMemAllocatedBytes(theOther._myGfxMemAllocatedBytes)
        {}

#if _MSC_VER >= 1400 // VC 7 can't handle template members
        template <typename U>
            GLAlloc(const GLAlloc<U> & theOther) throw()
            : _myGfxMemAllocatedBytes(theOther._myGfxMemAllocatedBytes),
            _myType(theOther._myType),
             _myMem(theOther._myMem)
       {}
#endif

        GLAlloc & operator=(const GLAlloc & theOther) {
            _myType = theOther._myType;
            _myMem = theOther._myMem;
            _myGfxMemAllocatedBytes = theOther._myGfxMemAllocatedBytes;
            return *this;
        }

        // Destructor
        ~GLAlloc() throw() {}

        // Utility functions
        pointer address(reference r) const {
            return &r;
        }

        const_pointer address(const_reference c) const {
            return &c;
        }

        size_type max_size() const {
            //return VertexMemory(_myType).capacity();
            return std::numeric_limits<size_t>::max() / sizeof(T);
        }

        // In-place construction
        void construct(pointer p, const_reference c) {
            // placement new operator
            new(reinterpret_cast<void*>(p) ) T(c);
        }

        // In-place destruction
        void destroy(pointer p ) {
            // call destructor directly
            (p)->~T();
        }

        // Rebind to allocators of other types
        template <typename U>
        struct rebind {
            typedef GLAlloc<U> other;
        };

        // Allocate a piece of raw memory from the block; if the block is exhausted,
        // no  resize attempt will be made and an exception will be thrown
        pointer allocate( size_type n, const void* = NULL ) {
            DB2(std::cerr << "GLAlloc::allocate: allocate object count = " << n << std::endl);
            asl::AC_SIZE_TYPE myRequiredBytes = n * sizeof(T);
            DB2(std::cerr << "GLAlloc::allocate: allocate byte size = " << myRequiredBytes << std::endl);

            pointer myResult = reinterpret_cast<pointer>(
                _myMem->allocateChunk(myRequiredBytes));

            return myResult;

        }

        void deallocate( pointer p, size_type n) {
            if( p == NULL ) {
                return;
            }
            DB2(std::cerr << "GLAlloc::deallocate: deallocating " << n * sizeof(T) << " bytes gfx memory @" << (void*)p<<std::endl);
            _myMem->deallocateChunk(reinterpret_cast<unsigned char*>(p));
        }

        // Non-standard Dinkumware hack for Visual C++ 6.0 compiler.
        // VC 6 doesn't support template rebind.
        char* _Charalloc( size_type n ) {
            return reinterpret_cast<char*>(allocate(n, NULL));
        }

        GLMemory::MemoryType getMemoryType() const {
            return _myType;
        }
    private:
        GLMemory::MemoryType _myType;
        GLMemoryPtr _myMem;
        asl::AC_SIZE_TYPE _myGfxMemAllocatedBytes;
    }; // end of GLAlloc

    // Comparison
    template <typename T1, typename T2>
        bool operator==( const GLAlloc<T1>&,
        const GLAlloc<T2>& ) throw()
    {
        return true;
    }

    template <typename T1, typename T2>
        bool operator!=( const GLAlloc<T1>&,
        const GLAlloc<T2>& ) throw()
    {
        return false;
    }

} // end namespace GLAlloc

#undef DB2

#endif // Block_ALLOCATOR_H

