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
//
//   $RCSfile: VertexData.h,v $
//   $Author: martin $
//   $Revision: 1.12 $
//   $Date: 2005/04/21 16:25:03 $
//
//  Description:
//
//=============================================================================

#ifndef _ac_scene_VertexData_h_
#define _ac_scene_VertexData_h_

#include "y60_scene_settings.h"

#include <y60/base/TypeTraits.h>
#include <y60/base/DataTypes.h>

#include <asl/base/Auto.h>
#include <asl/base/Singleton.h>
#include <asl/base/Exception.h>
#include <asl/dom/Nodes.h>

namespace y60 {

    DEFINE_EXCEPTION(VertexDataException, asl::Exception);
    DEFINE_EXCEPTION(UnloadVertexArraySizeMismatch, VertexDataException);

    template <class T> struct VertexDataSizeTraits {
        static unsigned long size() {
            return T::size();
        }
    };

    template <>
    struct VertexDataSizeTraits<float> {
        static unsigned long size() {
            return 1;
        }
    };

    template <>
    struct VertexDataSizeTraits<double> {
        static unsigned long size() {
            return 1;
        }
    };
    template <class LOCKABLE, class ACCESSIBLE>
    class LockingAccessor /*: public asl::AutoLocker<LOCKABLE> */{
    public:
        LockingAccessor(LOCKABLE & theLockable, ACCESSIBLE & theAccessible,
                        bool forWriting, bool  forReading)
            : /*asl::AutoLocker<LOCKABLE>(theLockable),*/ _myAccessible(theAccessible)
        {
            _myAccessible.lock(forWriting, forReading);
        }
        ~LockingAccessor() {
            _myAccessible.unlock();
        }
        ACCESSIBLE & get() {
            return _myAccessible;
        }
        const ACCESSIBLE & get() const {
            return _myAccessible;
        }
    private:
        ACCESSIBLE & _myAccessible;
    };

    template <class T> class VertexData;

    class VertexDataBase;

    template <class T>
    struct VertexDataAccessor : public LockingAccessor<VertexDataBase, VertexData<T> > {
        typedef LockingAccessor<VertexDataBase, VertexData<T> > self;
        VertexDataAccessor(VertexDataBase & theLockable, VertexData<T> & theAccessible,
                           bool forWriting, bool  forReading)
            : LockingAccessor<VertexDataBase, VertexData<T> >(theLockable, theAccessible,
                                                              forWriting, forReading) {}
    };

    template <class T>
    struct ConstVertexDataAccessor : public LockingAccessor<VertexDataBase, const VertexData<T> > {
        typedef LockingAccessor<const VertexDataBase, VertexData<T> > self;
        ConstVertexDataAccessor(VertexDataBase & theLockable, const VertexData<T> & theAccessible,
                                bool forWriting, bool  forReading)
            : LockingAccessor<VertexDataBase, const VertexData<T> >(theLockable, theAccessible,
                                                                    forWriting, forReading) {}
    };

    class Y60_SCENE_DECL VertexDataBase  {
        public:
            typedef std::size_t size_type;
            VertexDataBase(const TypeId & theType) : _myType(theType)  {};
            virtual ~VertexDataBase() {};
            VertexDataBase() {};

            TypeId getType() {
                return _myType;
            }

            virtual size_type size() const = 0;
            virtual size_type getElementSize() const = 0;
#if 0
            virtual const void * getDataPtr() const = 0;
            virtual void * getDataPtr() = 0;

            template <class T>
            typename VertexData<T>::VertexDataVector & getVertexDataCast() {
                return dynamic_cast<VertexData<T>&>(*this).getData();
            }

            template <class T>
            const typename VertexData<T>::VertexDataVector & getVertexDataCast() const {
                return dynamic_cast<const VertexData<T>&>(*this).getData();
            }
#else
            /// returns an accessor object that automatically keeps a lock on the resource that it automatically
            /// released when the accessor object is destroyed
            template <class T>
            asl::Ptr<VertexDataAccessor<T> > getVertexDataAccessor(bool forWriting = true, bool  forReading = false) {
                return asl::Ptr<VertexDataAccessor<T> >(
                    new VertexDataAccessor<T>(*this, dynamic_cast<VertexData<T>&>(*this), forWriting, forReading));
            }

            template <class T>
            asl::Ptr<ConstVertexDataAccessor<T> > getVertexDataAccessor() const {
                return asl::Ptr<ConstVertexDataAccessor<T> >(
                    new ConstVertexDataAccessor<T>(const_cast<VertexDataBase&>(*this),
                                                   dynamic_cast<const VertexData<T>&>(*this), false, true)); // readonly access
            }
#endif
            virtual void unlock() const = 0;
            virtual void lock(bool forWriting, bool forReading) const = 0;

            virtual void useAsPosition() const = 0;
            virtual void useAsNormal() const = 0;
            virtual void useAsColor() const = 0;
            virtual void useAsTexCoord() const = 0;
            virtual const void * getDataPtr() const = 0;
            /// load vertex data from dom source arrays into gfx memory
            virtual void load(const VectorOfUnsignedInt & theIndices,
                              dom::NodePtr theVertexDataNode) = 0;

            /// write back vertex data from gfx memory to dom source arrays
            virtual void unload(const VectorOfUnsignedInt & theIndices,
                      dom::NodePtr theVertexDataNode) const = 0;

        private:
            VertexDataBase(const VertexDataBase &);
            VertexDataBase & operator=(const VertexDataBase &);
            TypeId _myType;

    };

    typedef asl::Ptr<VertexDataBase> VertexDataBasePtr;

    template <class T>
    class VertexData : public VertexDataBase {
    public:
        VertexData() : VertexDataBase(TypeIdTraits<T>::type_id())  {};
        virtual ~VertexData() {};

        virtual size_type getElementSize() const {
            return VertexDataSizeTraits<T>::size();
        }
        virtual void push_back(const T & theElement) = 0;
        virtual T & operator[](unsigned n) = 0;
        virtual const T & operator[](unsigned n) const = 0;
    private:
        VertexData(const VertexData &);
        VertexData & operator=(const VertexData &);
    };

    template <class T>
    class VertexDataFactory {
        public:
            typedef asl::Ptr<VertexData<T> > (*FactoryMethod)(const VertexBufferUsage &);

            VertexDataFactory() : _myFactoryMethod(0) {};

            asl::Ptr<VertexData<T> > create(const VertexBufferUsage & theUsage) {

                if (_myFactoryMethod) {
                    return (*_myFactoryMethod)(theUsage);
                } else {
                    return asl::Ptr<VertexData<T> >();
                    //throw asl::Exception("No Data Factories added yet!", PLUS_FILE_LINE);
                }
            }
            void setFactoryMethod(FactoryMethod theFactoryMethod) {
                _myFactoryMethod = theFactoryMethod;
            }
        private:
            FactoryMethod _myFactoryMethod;
    };

    typedef VertexData<float> VertexData1f;
    typedef VertexData<asl::Vector2f> VertexData2f;
    typedef VertexData<asl::Vector3f> VertexData3f;
    typedef VertexData<asl::Vector4f> VertexData4f;

    typedef asl::Ptr<VertexData1f> VertexData1fPtr;
    typedef asl::Ptr<VertexData2f> VertexData2fPtr;
    typedef asl::Ptr<VertexData3f> VertexData3fPtr;
    typedef asl::Ptr<VertexData4f> VertexData4fPtr;

    typedef VertexDataFactory<float> VertexDataFactory1f;
    typedef VertexDataFactory<asl::Vector2f> VertexDataFactory2f;
    typedef VertexDataFactory<asl::Vector3f> VertexDataFactory3f;
    typedef VertexDataFactory<asl::Vector4f> VertexDataFactory4f;
}


#endif // _ac_scene_VertexData_h_


