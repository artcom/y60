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

#ifndef _asl_included_y60_VertexVector_h_
#define _asl_included_y60_VertexVector_h_

#include "y60_glutil_settings.h"

#include "GLUtils.h"

#include <limits>
#include <vector>

#include <asl/base/begin_end.h>
#include <asl/base/Exception.h>
#include <asl/base/Logger.h>
#include <asl/base/Auto.h>

namespace y60 {
    class Primitive;
}

namespace y60 {

    DEFINE_EXCEPTION(VertexVectorException, asl::Exception);
    DEFINE_EXCEPTION(VertexObjectNotLocked,VertexVectorException);
    DEFINE_EXCEPTION(TooManyVertexVectorUnlocks,VertexVectorException);
    DEFINE_EXCEPTION(CanNotBindLockedVertexVector,VertexVectorException);

    inline GLenum asGLEnum(VertexBufferUsage & theEnum) {
        switch (theEnum) {
            case VERTEX_USAGE_STREAM_DRAW:
                return GL_STREAM_DRAW_ARB;
            case VERTEX_USAGE_STREAM_READ:
                return GL_STREAM_READ_ARB;
            case VERTEX_USAGE_STREAM_COPY:
                return GL_STREAM_COPY_ARB;
            case VERTEX_USAGE_STATIC_DRAW:
                return GL_STATIC_DRAW_ARB;
            case VERTEX_USAGE_STATIC_READ:
                return GL_STATIC_READ_ARB;
            case VERTEX_USAGE_STATIC_COPY:
                return GL_STATIC_COPY_ARB;
            case VERTEX_USAGE_DYNAMIC_DRAW:
                return GL_DYNAMIC_DRAW_ARB;
            case VERTEX_USAGE_DYNAMIC_READ:
                return GL_DYNAMIC_READ_ARB;
            case VERTEX_USAGE_DYNAMIC_COPY:
                return GL_DYNAMIC_COPY_ARB;
            default: break;
        }
        throw VertexVectorException(std::string("Unknown VBO usage flag '") + theEnum.asString() + "'", PLUS_FILE_LINE);
    }


    template <class T>
    class VertexVector {
    public:
        typedef ptrdiff_t D;
        typedef T value_type;
        typedef value_type * pointer;
        typedef value_type * iterator;
        typedef const value_type * const_iterator;
        typedef value_type& reference;
        typedef const value_type & const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;

        // VertexVector specific types:
        // typedef T* iterator;
        // typedef const T* const_iterator;
        typedef asl::AutoLocker<VertexVector<T> > Locker;
    protected:
        GLuint _myId;
        unsigned int _mySize;
        unsigned int _myCapacity;
        mutable T *  _myLockedData;
        mutable int          _myLockCount;
        VertexBufferUsage _myUsageType;

        static unsigned int getNewId() {
            GLuint myId = 0;
            glGenBuffersARB(1, &myId);
            AC_TRACE << "glGenBuffersARB created a new buffer " << myId << std::endl;
            CHECK_OGL_ERROR;
            return myId;
        };

        // TODO: exception safety

        void bindMe() const {
            AC_TRACE << "glBindBufferARB(" << _myId << ")";
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, _myId);
            CHECK_OGL_ERROR;
        }
        static void unbind() {
            AC_TRACE << "glBindBufferARB(0)";
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            CHECK_OGL_ERROR;
        }
        static void resizeAnduploadAll(const T * theData, unsigned long theSize, VertexBufferUsage theUsageType) {
            AC_TRACE << "glBufferDataARB " << theSize << "(all)";
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, theSize * sizeof(T), theData, asGLEnum(theUsageType));
            CHECK_OGL_ERROR;
        }
        static void uploadPart(const T * theData, unsigned long theSize, unsigned long theOffset) {
            AC_TRACE << "glBufferDataARB " << theSize << "(part)";
            glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, theOffset * sizeof(T), theSize * sizeof(T), theData);
            CHECK_OGL_ERROR;
        }
        static void download(T * theData, unsigned long theSize, unsigned long theOffset) {
            AC_TRACE << "glGetBufferSubDataARB " << theSize << "(get)";
            glGetBufferSubDataARB(GL_ARRAY_BUFFER_ARB, theOffset * sizeof(T), theSize * sizeof(T), theData);
            CHECK_OGL_ERROR;
        }
        static void destructiveResize(unsigned long theSize, VertexBufferUsage theUsageType) {
            AC_TRACE << "glBufferSubDataARB (destructiveResize)";
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, theSize * sizeof(T), 0, asGLEnum(theUsageType));
            CHECK_OGL_ERROR;
        }

    public:
        std::ostream & print(std::ostream & theStream) const {
            if (_mySize) {
                std::vector<T> myBuffer;
                bindMe();
                if (_mySize) {
                    myBuffer.resize(_mySize);
                    download(asl::begin_ptr(myBuffer), _mySize, 0);
                }
                theStream << myBuffer;
                unbind();
            }
            return theStream;
        }
        void bindUnlocked() const {
            if (_myLockCount != 0) {
                throw CanNotBindLockedVertexVector(JUST_FILE_LINE);
            }
            bindMe();
        }
        // Lockable implementation
        void lock(bool forWriting = true, bool forReading = false) const {
            if (_myLockCount == 0) {
                lockBuffer(forWriting, forReading);
            }
            ++_myLockCount;
        }
        void unlock() const {
            --_myLockCount;
            if (_myLockCount == 0) {
                unlockBuffer();
            }
            if (_myLockCount < 0) {
                throw TooManyVertexVectorUnlocks(JUST_FILE_LINE);
            }
        }
        bool isLocked() const {
            return _myLockCount != 0;
        }
    protected:
        T * lockBuffer( bool forWriting, bool forReading) const {
            unsigned myFlags = 0;
            if (forReading) {
                if (forWriting) {
                    myFlags = GL_READ_WRITE;
                } else {
                    myFlags = GL_READ_ONLY;
                }
            } else if (forWriting) {
                myFlags = GL_WRITE_ONLY;
            }
            AC_TRACE << "locking buffer";
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, _myId);
            CHECK_OGL_ERROR;
            _myLockedData = (T*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, myFlags);
            CHECK_OGL_ERROR;
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            CHECK_OGL_ERROR;
            return _myLockedData;
        }
        void unlockBuffer() const {
            bindMe();
            CHECK_OGL_ERROR;
            AC_TRACE << "unlocking buffer";
            if (!glUnmapBufferARB(GL_ARRAY_BUFFER_ARB)) {
                CHECK_OGL_ERROR;
            }
            unbind();
            CHECK_OGL_ERROR;
            _myLockedData = 0;
        }

        void deleteBuffer() {
            if (_myId != 0) {
                AC_TRACE << "glDeleteBuffersARB deleting buffer " << _myId << std::endl;
                glDeleteBuffersARB(1, &_myId);
                _myId = 0;
                _mySize = 0;
                _myCapacity = 0;
                _myLockedData = 0;
                if (_myLockCount) {
                    AC_WARNING << "deleteBuffer: deleting locked buffer with lockcount = "<<_myLockCount<<std::endl;
                }
                _myLockCount = 0;
            }
        }
    public:

        VertexVector() : _myId(0), _mySize(0), _myCapacity(0), _myLockCount(0), _myUsageType(getDefaultVertexBufferUsage()) {}

        VertexVector(const VertexVector<T> & x)
            : _myId(getNewId()), _mySize(x.size()), _myCapacity(x.size()), _myLockCount(0)
        {
            if (x.size()) {
                T * myData = x.lock(true, false);
                bindMe();
                upload(myData, _myCapacity);
                x.unlock();
            }
        }

        VertexVector(const_iterator theFirst, const_iterator theLast)
            : _myId(getNewId()), _mySize(theLast - theFirst), _myCapacity(_mySize), _myLockCount(0)
        {
            bindMe();
            upload(theFirst, _mySize, _myCapacity);
            unbind();
        }

        ~VertexVector() {
            deleteBuffer();
        }

        void setUsage(VertexBufferUsage theUsageType) {
            _myUsageType = theUsageType;
        };

        // same as std::vector:
        iterator begin() {
            if (_myLockCount || empty()) {
                return _myLockedData;
            } else {
                throw VertexObjectNotLocked(JUST_FILE_LINE);
            }
        }
        const_iterator begin() const {
            if (_myLockCount || empty()) {
                return _myLockedData;
            } else {
                throw VertexObjectNotLocked(JUST_FILE_LINE);
            }
        }
        iterator end() {
            if (_myLockCount || empty()) {
                return _myLockedData + _mySize;
            } else {
                throw VertexObjectNotLocked(JUST_FILE_LINE);
            }
        }
        const_iterator end() const {
            if (_myLockCount || empty()) {
                return _myLockedData + _mySize;
            } else {
                throw VertexObjectNotLocked(JUST_FILE_LINE);
            }
        }
        reverse_iterator rbegin() {
            return std::reverse_iterator<T*>(this->end());
        }
        const_reverse_iterator rbegin() const {
            return const_reverse_iterator(this->end());
        }
        reverse_iterator rend() {
            return std::reverse_iterator<T*>(this->begin());
        }
        const_reverse_iterator rend() const {
            return const_reverse_iterator(this->begin());
        }

        size_type size() const {
            return _mySize;
        }
        size_type max_size() const {
            return size_type(-1);
        }
        size_type capacity() const {
            return _myCapacity;
        }
        bool empty() const {
            return size() != 0;
        }
        reference operator[](size_type n) {
            return *(begin() + n);
        }
        const_reference operator[](size_type n) const {
            return *(begin() + n);
        }

        VertexVector<T> & operator=(const VertexVector<T> & x) {
            deleteBuffer();
            if (x.size()) {
                T * myData = x.lock(true, false);
                bindMe();
                upload(myData,x.size(), x.size());
                x.unlock();
            }
            return *this;
        };
        void swap(VertexVector<T> & x) {
            std::swap(_myId,x._myId);
            std::swap(_mySize,x._mySize);
            std::swap(_myCapacity,x._myCapacity);
            std::swap(_myLockedData,x._myLockedData);
        }
        void clear() {
            deleteBuffer();
        }
        void resize(size_type n) {
            reserve(n);
            _mySize = n;
        }
        void reserve(size_type n) {
            if (n > _myCapacity && n >= _mySize) {
                if (_myId == 0) {
                    _myId = getNewId();
                }
                std::vector<T> myBuffer;
                myBuffer.resize(n);
                bindMe();
                if (_mySize) {
                    download(asl::begin_ptr(myBuffer), _mySize, 0);
                }
                _myCapacity = n;
                destructiveResize(n, _myUsageType);
                if (_mySize) {
                    //uploadPart( theData, theSize, theOffset)
                    uploadPart(asl::begin_ptr(myBuffer), _mySize, 0);
                }
            }
        }
        reference front() {
            return *begin();
        }
        const_reference front() const {
            return *begin();
        }
        reference back() {
            return *(end() - 1);
        }
        const_reference back() const {
            return *(end() - 1);
        }
        void push_back(const T & x) {
            if (_myId == 0) {
                _myId = getNewId();
            }
            //std::cerr << "push_back: "<< x << std::endl;
            if (_mySize + 1 > _myCapacity) {
                reserve(_mySize + 1 + (_mySize+1)/2);
            } else {
                bindMe();
            }
          //uploadPart( theData, theSize, theOffset)
            uploadPart( &x,      1,       _mySize);
            ++_mySize;
            //print(std::cerr);
        }
    private:
        /*
        iterator insert(iterator position, const T& x) {
            return _data.insert(position,x);
        }
        iterator insert(iterator position) {
            return _data.insert(position, T());
        }
        void insert (iterator position, const_iterator first, const_iterator last) {
            _data.insert(position,first,last);
        };
        void insert (iterator position, size_type n, const T& x) {
            _data.insert(_data.begin() + (position-begin()),n,x);
        };
        void pop_back() {
            _mySize--;
        }
        void erase(iterator position) {
        _data.erase(position);
        }
        void erase(iterator first, iterator last) {
        _data.erase(_data.begin() + (first-begin()),
        _data.begin() + (last-begin()));

        }
        void resize_vec(size_type new_size, const T& x) {
        new_size = new_size / PackedRaster<T>::Factor;
        if (new_size < size())
        erase(begin() + new_size, end());
        else
        insert(end(), new_size - size(), x);
        }
        void resize_vec(size_type new_size) {
        resize_vec(new_size, T());
        }
        */
    };

} // end namespace asl

#undef DB2

#endif // Block_ALLOCATOR_H

