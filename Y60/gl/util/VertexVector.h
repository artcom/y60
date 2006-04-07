//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _asl_included_y60_VertexVector_h_
#define _asl_included_y60_VertexVector_h_

#include "GLUtils.h"

#include <limits>
#include <vector>

#include <asl/Exception.h>


#include <asl/Logger.h>
#include <asl/Auto.h>

namespace y60 {
    class Primitive;
}

namespace y60 {

    DEFINE_EXCEPTION(VertexVectorException, asl::Exception); 
    DEFINE_EXCEPTION(VertexObjectNotLocked,VertexVectorException); 
    DEFINE_EXCEPTION(TooManyVertexVectorUnlocks,VertexVectorException);  
    DEFINE_EXCEPTION(CanNotBindLockedVertexVector,VertexVectorException);  

    enum VertexBufferUsageType {
        VERTEX_USAGE_STREAM_DRAW  = GL_STREAM_DRAW_ARB,
        VERTEX_USAGE_STREAM_READ  = GL_STREAM_READ_ARB,
        VERTEX_USAGE_STREAM_COPY  = GL_STREAM_COPY_ARB,
        VERTEX_USAGE_STATIC_DRAW  = GL_STATIC_DRAW_ARB,
        VERTEX_USAGE_STATIC_READ  = GL_STATIC_READ_ARB,
        VERTEX_USAGE_STATIC_COPY  = GL_STATIC_COPY_ARB,
        VERTEX_USAGE_DYNAMIC_DRAW = GL_DYNAMIC_DRAW_ARB,
        VERTEX_USAGE_DYNAMIC_READ = GL_DYNAMIC_READ_ARB,
        VERTEX_USAGE_DYNAMIC_COPY = GL_DYNAMIC_COPY_ARB,
        VERTEX_USAGE_DISABLED = 1,
        VERTEX_USAGE_UNDEFINED = 0
    };

    //Y60_VBO_USAGE env variable
    static const char * VertexBufferUsageTypeNames[]= {
            "STREAM_DRAW",
            "STREAM_READ",
            "STREAM_COPY",
            "STATIC_DRAW",
            "STATIC_READ",
            "STATIC_COPY",
            "DYNAMIC_DRAW",
            "DYNAMIC_READ",
            "DYNAMIC_COPY",
            "DISABLED"
    };

    static const VertexBufferUsageType VertexBufferUsageTypeList[]= {
        VERTEX_USAGE_STREAM_DRAW,
        VERTEX_USAGE_STREAM_READ,
        VERTEX_USAGE_STREAM_COPY,
        VERTEX_USAGE_STATIC_DRAW,
        VERTEX_USAGE_STATIC_READ,
        VERTEX_USAGE_STATIC_COPY,
        VERTEX_USAGE_DYNAMIC_DRAW,
        VERTEX_USAGE_DYNAMIC_READ,
        VERTEX_USAGE_DYNAMIC_COPY,
        VERTEX_USAGE_DISABLED,
        VERTEX_USAGE_UNDEFINED
    };

    inline VertexBufferUsageType getVertexBufferUsageTypeFromString(const std::string & theUsageName) {
        for (int i = 0; VertexBufferUsageTypeList[i] != VERTEX_USAGE_UNDEFINED; ++i) {
            if (theUsageName == VertexBufferUsageTypeNames[i]) {
                return VertexBufferUsageTypeList[i];
            }
        }
        return VERTEX_USAGE_UNDEFINED;
    }

    template <class T>
    class VertexVector : public asl::Lockable {
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
        unsigned int _myId;
        unsigned int _mySize;
        unsigned int _myCapacity;
        mutable T *  _myLockedData;
        int          _myLockCount;
        VertexBufferUsageType _myUsageType;

        static unsigned int getNewId() {
            unsigned int myId = 0;
            glGenBuffersARB(1, &myId);
            AC_TRACE << "glGenBuffersARB created a new buffer " << myId << std::endl;
            CHECK_OGL_ERROR;
            return myId;
        };

        // TODO: exception safety

        void bindMe() const {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, _myId);
            CHECK_OGL_ERROR;
        }
        static void unbind() {
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            CHECK_OGL_ERROR;
        }
        static void resizeAnduploadAll(const T * theData, unsigned long theSize, VertexBufferUsageType theUsageType) {
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, theSize * sizeof(T), theData, theUsageType);
            CHECK_OGL_ERROR;
        }
        static void uploadPart(const T * theData, unsigned long theSize, unsigned long theOffset) {
            glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, theOffset * sizeof(T), theSize * sizeof(T), theData);
            CHECK_OGL_ERROR;
        }
        static void download(T * theData, unsigned long theSize, unsigned long theOffset) {
            glGetBufferSubDataARB(GL_ARRAY_BUFFER_ARB, theOffset * sizeof(T), theSize * sizeof(T), theData);
            CHECK_OGL_ERROR;
        }
        static void destructiveResize(unsigned long theSize, VertexBufferUsageType theUsageType) {
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, theSize * sizeof(T), 0, theUsageType);
            CHECK_OGL_ERROR;
        }

    public:
        std::ostream & print(std::ostream & theStream) const {
            if (_mySize) {
                std::vector<T> myBuffer;
                bindMe();
                if (_mySize) {
                    myBuffer.resize(_mySize);
                    download(&(*myBuffer.begin()), _mySize, 0);
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
        void lock() {
            if (_myLockCount == 0) {
                lockBuffer();
            }
            ++_myLockCount;
        }
        void unlock() {
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
        T * lockBuffer(bool forReading = true, bool forWriting = true) {
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
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, _myId);
            CHECK_OGL_ERROR;
            _myLockedData = (T*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_ONLY);
            CHECK_OGL_ERROR;
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
            CHECK_OGL_ERROR;
            return _myLockedData;
        }
        void unlockBuffer() const {
            bindMe();
            CHECK_OGL_ERROR;
            if (!glUnmapBufferARB(GL_ARRAY_BUFFER_ARB)) {
                CHECK_OGL_ERROR;
            }
            unbind();
            CHECK_OGL_ERROR;
            _myLockedData = 0;
        }

        void deleteBuffer() {
            if (_myId) {
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

        VertexVector() : _myId(0), _mySize(0), _myCapacity(0), _myLockCount(0), _myUsageType(VERTEX_USAGE_STATIC_DRAW) {}

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

        void setUsage(VertexBufferUsageType theUsageType) {
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
        void reserve(size_type n) {
            if (n > _myCapacity && n >= _mySize) {
                if (_myId == 0) {
                    _myId = getNewId();
                }
                std::vector<T> myBuffer;
                myBuffer.resize(n);
                bindMe();
                if (_mySize) {
                    download(&(*myBuffer.begin()), _mySize, 0);
                }
                _myCapacity = n;
                destructiveResize(n, _myUsageType);
                if (_mySize) {
                    //uploadPart( theData, theSize, theOffset)
                    uploadPart(&(*myBuffer.begin()), _mySize, 0);
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

