/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __ */


#ifndef _xml_Value_h_
#define _xml_Value_h_

#include "asl_dom_settings.h"

#include "typedefs.h"
#include "Exceptions.h"
#include "Dictionary.h"
#include "Field.h"

#include <asl/base/begin_end.h>
#include <asl/base/Block.h>
#include <asl/base/MemoryPool.h>
#include <asl/raster/raster.h>
#include <asl/raster/subraster.h>
#include <asl/raster/palgo.h>
#include <asl/raster/standard_pixel_types.h>
#include <asl/math/Vector234.h>
#include <asl/math/Box.h>

#include <asl/raster/pixels.h>
#include <asl/base/Logger.h>

#include <istream>
#include <map>
#define USE_SINGLE_ID_ATTRIB
#ifdef USE_SINGLE_ID_ATTRIB
    #ifdef USE_HASH_MAP
        #include <boost/unordered_map.hpp>
    #endif
#endif
#include <set>
#include <typeinfo>

#define DB(x) // x

/**
This file provides the facilities to box all C++ Types into a hierarchy of "Value"-objects with a common
base class name ValueBase. All objects can be unboxed and accessed as native types, or converted to and from
std::strings or binary representations, which can be used for persistence.

The dom::Node uses ValueBase-derived types as nodeValue, and using xml schema and factories, this mechanism
provides the dom with efficient binary serialization deserialization.

For string conversion the C++ stream operators are used.

There are basically four different Value-Wrappers that can be used for different families of C++ types:

StringValue - wraps std::string, all other values are derived from StringValue.
SimpleValue - wraps POD-types (int, float etc. and structs without member functions)
VectorValue - wraps std::vector<PODT> and other types that are consecutive arrays of POD-Types.
ComplexValue - wraps all other types, requires additional functions to be defined for serialization

Other special types include:
 IDValue, a value that registeres itself at the document root to provide fast element-by-id access
 RasterValue, a value that works with asl::raster to wrap image maps or other 2D-data

Conversion between native, text and binary representation is lazy, except when assigning a string value
because we want to know immediately when the conversion fails.

ValueBase is also derived from Field, which provides a dependency graph facility and automatic lazy
recomputation of dependent values. This is mainly used for FacadeAttributePlugs, which are non-persistent
attributes that can be added to a dom::Node using the Facade-mechanism.

Facade mainly provides fast and type-safe access to attributes and children of dom::Element nodes.

*/


namespace asl {

    //TODO: This stuff does not belong here, should be moved to raster
    template <class VALUE>
    VALUE getAlphaValue(const RGB_t<VALUE> & thePixel) {
    //    return thePixel.get(alpha<VALUE>()).get();
        return 0;
    }
    template <class VALUE, class SRC_VALUE>
    void setAlphaValue(RGB_t<VALUE> & thePixel, SRC_VALUE theValue) {
        //  thePixel.get(alpha<VALUE>()).set(theValue);
    }

    template <class VALUE>
    VALUE getAlphaValue(const BGR_t<VALUE> & thePixel) {
    //    return thePixel.get(alpha<VALUE>()).get();
        return 0;
    }
    template <class VALUE, class SRC_VALUE>
    void setAlphaValue(BGR_t<VALUE> & thePixel, SRC_VALUE theValue) {
        //  thePixel.get(alpha<VALUE>()).set(theValue);
    }

    template <class VALUE>
    VALUE getAlphaValue(const gray<VALUE> & thePixel) {
    //    return thePixel.get(alpha<VALUE>()).get();
        return 0;
    }
    template <class VALUE, class SRC_VALUE>
    void setAlphaValue(gray<VALUE> & thePixel, SRC_VALUE theValue) {
        //  thePixel.get(alpha<VALUE>()).set(theValue);
    }
 }

#if 0
inline
std::ostream & operator<<(std::ostream & os, const std::vector<unsigned char> & theBlock) {
    for (unsigned i = 0; i < theBlock.size(); ++i) {
        os << theBlock[i];
    }
    return os;
}

inline
std::istream & operator>>(std::istream & is, std::vector<unsigned char> & theBlock) {
    unsigned char c;
    while(is >> c) {
        theBlock.push_back(c);
    }
    return is;
}
#endif

#define DBE(x) x
namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */
    DEFINE_EXCEPTION(ValueNotBlockWriteable, asl::Exception);
    DEFINE_EXCEPTION(ValueAlreadyBlockWriteable, asl::Exception);

    class ValueBase;
    typedef asl::Ptr<ValueBase,ThreadingModel> ValuePtr;

    class ValueBase : protected asl::WriteableBlock, public Field {
    public:
        friend class Node;

        virtual const char * name() const = 0;
        virtual const std::type_info & getTypeInfo() const = 0;
        virtual const DOMString & getString() const = 0;
        virtual void setString(const DOMString & theValue) = 0;
        virtual void setStringWithoutNotification(const DOMString & theValue) = 0;
        virtual ValuePtr clone(Node * theNode) const = 0;
        /// create default constructed value
        virtual ValuePtr create(Node * theNode) const = 0;
        ///create Value from string
        virtual ValuePtr create(const DOMString & theValue,Node * theNode) const = 0;
        ///create Value from binary representation
        virtual ValuePtr create(const asl::ReadableBlock & theValue, Node * theNode) const = 0;
        virtual void bumpVersion() = 0;
        virtual void notifyValueChanged() = 0;

        virtual void set(const ReadableBlock & theBlock) {
            DB(AC_TRACE << "ValueBase::set(): this="<<(void*)this<<" , vtname="<<name());
            assign(theBlock);
            bumpVersion();
        }
        virtual void update() const = 0;
        virtual asl::WriteableBlock & openWriteableBlock() = 0;
        virtual void closeWriteableBlock() = 0;
        virtual bool isBlockWriteable() const = 0;
        virtual const asl::ReadableBlock & accessReadableBlock() const {
            onGetValue();
            return *this;
        }
        bool isVector() const {
            return false;
        }
        virtual void freeCaches() const = 0;
        virtual Node * getNodePtr() = 0;
        virtual void binarize(asl::WriteableStream & theDest) const = 0;
        virtual asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) = 0;
    protected:
        virtual void setNodePtr(Node * theNode) = 0;
        virtual void reparent() const {
            DB(AC_TRACE << "ValueBase::reparent(): this="<<(void*)this<<" , vtname="<<name());
            //const_cast<ValueBase*>(this)->bumpVersion();
        }
    };

    // traits type for automatic selection of appropriate Value types
    // is specialized for many standard C++ types here,
    // and must be specialized for all user-defined types that shall be
    // wrapped.
    template <class VALUE>
    struct ValueWrapper;

    // automatically writes every POD-Type into a binary stream
    template <class T>
    void binarize(const T & myValue, asl::WriteableStream & theDest) {
        typedef typename ValueWrapper<T>::Type CheckType; // make sure only wrapped native values get automatically binarized
        theDest.appendData(myValue);
    }

    // automatically reads every POD-Type from a binary stream
    template <class T>
    asl::AC_SIZE_TYPE debinarize(T & myValue, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0) {
        typedef typename ValueWrapper<T>::Type CheckType; // make sure only wrapped native values get automatically binarized
        return theSource.readData(myValue,thePos);
    }

    // specialization for std::string
    inline
    void binarize(const std::string & myValue, asl::WriteableStream & theDest) {
        theDest.appendCountedString(myValue);
    }
    inline
    asl::AC_SIZE_TYPE debinarize(std::string & myValue, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0) {
        return theSource.readCountedString(myValue,thePos);
    }

    // specialization for std::vector<T>
#ifdef UNCOMPRESSED_VECTORS
    // This version simply reads and writes the plain memory
    template <class T>
        void binarize(const std::vector<T> & myValue, asl::WriteableStream & theDest) {
            theDest.appendUnsigned(asl::AC_SIZE_TYPE(myValue.size()));
            for (int i = 0; i < myValue.size(); ++i) {
                binarize(myValue[i],theDest);
            }
        }
    template <class T>
        asl::AC_SIZE_TYPE debinarize(std::vector<T> & myValue, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0) {
            asl::AC_SIZE_TYPE myCount;
            thePos = theSource.readUnsigned(myCount,thePos);
            myValue.resize(myCount);
            for (int i = 0; i < myCount; ++i) {
                thePos = debinarize(myValue[i],theSource,thePos);
            }
            return thePos;
        }
#else
    // This version uses lossless run-length encoding to read and write vectors
    template <class T>
    void binarize(const std::vector<T> & theVector, asl::WriteableStream & theDest) {
        return ValueWrapper<T>::Type::binarizeVector(theVector, theDest);
    }
    template <class T>
    asl::AC_SIZE_TYPE debinarize(std::vector<T> & theVector, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0) {
        return ValueWrapper<T>::Type::debinarizeVector(theVector, theSource, thePos);
    }

    // This is the generic version that call "binarize" for every element
   template <class T>
        void binarizeGeneric(const std::vector<T> & theVector, asl::WriteableStream & theDest) {
            theDest.appendUnsigned(asl::AC_SIZE_TYPE(theVector.size()));
            unsigned int i = 0;
            bool equalSequence = true;
            while (i < theVector.size()) {
                unsigned int j = i + 1;
                const T & myValue = theVector[i];
                while (j < theVector.size() && ((theVector[j] == theVector[j-1]) == equalSequence)) {
                    ++j;
                }
                theDest.appendUnsigned(j - i);
                if (equalSequence) {
                    binarize(myValue, theDest);
                    i = j;
                    equalSequence = false;
                } else {
                    while (i < j) {
                        binarize(theVector[i++],theDest);
                    }
                    equalSequence = true;
                }
            }
        }
    // This is the generic version that call "debinarize" for every element
    template <class T>
        asl::AC_SIZE_TYPE debinarizeGeneric(std::vector<T> & theVector, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            asl::AC_SIZE_TYPE myCount;
            thePos = theSource.readUnsigned(myCount,thePos);
            theVector.resize(myCount);
            bool equalSequence = true;
            asl::AC_SIZE_TYPE i = 0;
            while (i < myCount) {
                asl::AC_SIZE_TYPE seqCount;
                thePos = theSource.readUnsigned(seqCount,thePos);
                if (equalSequence) {
                    T myValue;
                    thePos = debinarize(myValue, theSource, thePos);
                    while (seqCount--) {
                        theVector[i++] = myValue;
                    }
                    equalSequence = false;
                } else {
                    while (seqCount--) {
                        thePos = debinarize(theVector[i++],theSource,thePos);
                    }
                    equalSequence = true;
                }
            }
            return thePos;
        }

    // This is a faster version that reads/writes consecutive chunks of memory
    template <class T>
        asl::AC_SIZE_TYPE debinarizePODT(std::vector<T> & theVector, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            asl::AC_SIZE_TYPE myCount;
            thePos = theSource.readUnsigned(myCount,thePos);
            theVector.resize(myCount);
            bool equalSequence = true;
            asl::AC_SIZE_TYPE i = 0;
            while (i < myCount) {
                asl::AC_SIZE_TYPE seqCount;
                thePos = theSource.readUnsigned(seqCount,thePos);
                if (equalSequence) {
                    T myValue;
                    thePos = theSource.readData(myValue,thePos);
                    while (seqCount--) {
                        theVector[i++] = myValue;
                    }
                    equalSequence = false;
                } else {
                    thePos = theSource.readBytes(&theVector[i],seqCount * sizeof(T), thePos);
                    i+=seqCount;
                    equalSequence = true;
                }
            }
            return thePos;
        }
    // This is a faster version that reads/writes consecutive chunks of memory
    template <class T>
        void binarizePODT(const std::vector<T> & theVector, asl::WriteableStream & theDest) {
            theDest.appendUnsigned(asl::AC_SIZE_TYPE(theVector.size()));
            unsigned int i = 0;
            bool equalSequence = true;
            while (i < theVector.size()) {
                unsigned int j = i + 1;
                const T & myValue = theVector[i];
                while (j < theVector.size() && ((theVector[j] == theVector[j-1]) == equalSequence)) {
                    ++j;
                }
                asl::AC_SIZE_TYPE myCount = j - i;
                theDest.appendUnsigned(myCount);
                if (equalSequence) {
                    theDest.appendData(myValue);
                    equalSequence = false;
                } else {
                    theDest.append(&theVector[i], sizeof(T) * myCount);
                    equalSequence = true;
                }
                i = j;
            }
        }
#endif

    // specialization for raster
   template <class T, class Alloc, class D>
   void binarize(const asl::raster<T, Alloc, D> & myValue, asl::WriteableStream & theDest) {
       theDest.appendUnsigned(myValue.hsize());
       theDest.appendUnsigned(myValue.vsize());
       binarize(myValue.getDataVector(), theDest);
   }
   template <class T, class Alloc, class D>
   asl::AC_SIZE_TYPE debinarize(asl::raster<T, Alloc, D> & myValue, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0) {
       asl::AC_SIZE_TYPE hsize;
       asl::AC_SIZE_TYPE vsize;
       thePos = theSource.readUnsigned(hsize,thePos);
       thePos = theSource.readUnsigned(vsize,thePos);
       myValue.resize(hsize, vsize);
       thePos = debinarize(myValue.getDataVector(),theSource,thePos);
       return thePos;
   }

    /** StringValue is also a base class for all other values, and is used for nodeValues when no
        schema is present.
    */
    class StringValue : public ValueBase {
    public:
        StringValue(Node * theNode)
            : _myStringValue(""), _isBlockWriteable(false), _myNode(theNode)
        {
            // intentionally no call to update() or bumpversion() here
        }
        StringValue(const asl::ReadableBlock & theValue, Node * theNode)
            : _myStringValue(theValue.strbegin(),theValue.size()), _isBlockWriteable(false), _myNode(theNode)
        {
            //notifyValueChanged();
        }
        StringValue(const DOMString & theStringValue, Node * theNode)
            : _myStringValue(theStringValue), _isBlockWriteable(false), _myNode(theNode)
        {
            //notifyValueChanged();
        }
        virtual void notifyValueChanged() {
            update();
            bumpVersion();
            onSetValue();
        }

        virtual const char * name() const {
            return typeid(DOMString).name();
        }
        virtual const std::type_info & getTypeInfo() const {
            return typeid(DOMString);
        }
        ASL_DOM_DECL virtual void bumpVersion();
        virtual void binarize(asl::WriteableStream & theDest) const {
            onGetValue();
            theDest.appendCountedString(_myStringValue);
        }
        virtual asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            thePos = theSource.readCountedString(_myStringValue, thePos);
            notifyValueChanged();
            return thePos;
        }
        static void binarizeVector(const std::vector<DOMString> & theVector, asl::WriteableStream & theDest) {
            binarizeGeneric(theVector, theDest);
        }
        static asl::AC_SIZE_TYPE debinarizeVector(std::vector<DOMString> & theVector, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            return debinarizeGeneric(theVector, theSource, thePos);
        }
        virtual void freeCaches() const {};
    protected:
        virtual unsigned char * begin() {
            if (!this->isBlockWriteable()) {
                throw ValueNotBlockWriteable(JUST_FILE_LINE);
            }
            return reinterpret_cast<unsigned char*>(asl::begin_ptr(_myStringValue));
        }
        virtual unsigned char * end() {
            if (!this->isBlockWriteable()) {
                throw ValueNotBlockWriteable(JUST_FILE_LINE);
            }
            return reinterpret_cast<unsigned char*>(asl::end_ptr(_myStringValue));
        }
        virtual const unsigned char * begin() const {
            return reinterpret_cast<const unsigned char*>(asl::begin_ptr(_myStringValue));
        }
        virtual const unsigned char * end() const {
            return reinterpret_cast<const unsigned char*>(asl::end_ptr(_myStringValue));
        }
        operator const void*() const {
            return this;
        }
    public:
        virtual asl::AC_SIZE_TYPE size() const {
            onGetValue();
            return _myStringValue.size();
        }
        virtual const DOMString & getString() const {
            onGetValue();
            return _myStringValue;
        }
        virtual void setString(const DOMString & theValue) {
            _myStringValue = theValue;
            notifyValueChanged();
        }
        virtual void setStringWithoutNotification(const DOMString & theValue) {
            _myStringValue = theValue;
            update();
        }
         virtual ValuePtr clone(Node * theNode) const {
            onGetValue();
            ValuePtr clonedValue = ValuePtr(new StringValue(_myStringValue, theNode));
            clonedValue->notifyValueChanged();
            return clonedValue;
        }
        virtual ValuePtr create(Node * theNode) const {
            return ValuePtr(new StringValue(theNode));
        }
        virtual ValuePtr create(const DOMString & theValue, Node * theNode) const {
            return ValuePtr(new StringValue(theValue, theNode));
        }
        virtual ValuePtr create(const asl::ReadableBlock & theValue, Node * theNode) const {
            return ValuePtr(new StringValue(theValue, theNode));
        }
       virtual void assign(const asl::ReadableBlock & myOtherBlock) {
            _myStringValue.resize(myOtherBlock.size());
            std::copy(myOtherBlock.begin(),myOtherBlock.end(),&_myStringValue[0]);
            notifyValueChanged();
        }
        virtual asl::WriteableBlock & openWriteableBlock() {
            if (this->isBlockWriteable()) {
                throw ValueAlreadyBlockWriteable(JUST_FILE_LINE);
            }
            setBlockWriteable(true);
            onGetValue();
            return *this;
        }
        virtual void closeWriteableBlock() {
           if (!this->isBlockWriteable()) {
                throw ValueNotBlockWriteable(JUST_FILE_LINE);
            }
            setBlockWriteable(false);
            notifyValueChanged();
        }
        virtual bool isBlockWriteable() const {
            return _isBlockWriteable;
        }
    protected:
        // update is called internally when the value has changed so in case the value
        // is registered somewhere this registry can be also updated (e.g. in case of an IDValue)
        virtual void update() const {
        }
        virtual void clearMutableString() const {
            _myStringValue.resize(0);
            _myStringValue.reserve(0);
        }
        virtual void setMutableString(const DOMString & theValue) const {
            // no more const_cast here, instead declare
            // as mutable. this is an attempt to fix bug 470
            _myStringValue = theValue;
        }
        virtual void setBlockWriteable(bool theState) {
            _isBlockWriteable = theState;
        }
        virtual void setNodePtr(Node * theNode) {
            _myNode = theNode;
        }
        virtual const DOMString & getStringDirect() const {
            return _myStringValue;
        }
    public:
        virtual Node * getNodePtr() {
            return _myNode;
        }
    private:
        mutable DOMString _myStringValue;
        bool _isBlockWriteable;
        mutable Node * _myNode; // a pointer to the node is maintained to allow the value to interact with the dom
                                // currently only IDValue makes use of it
    };

    struct UniqueId {
        friend class Node;
        friend ASL_DOM_DECL std::ostream & operator<<(std::ostream& os, const UniqueId & uid);

        UniqueId() :  _ptrValue((ptrdiff_t)this), _myCount(_myCounter++)
        { }
        UniqueId(const UniqueId & theID) : _ptrValue(theID._ptrValue), _myCount(theID._myCount)
        { }
        UniqueId & operator=(const UniqueId & theOther) {
            _ptrValue = theOther._ptrValue;
            _myCount = theOther._myCount;
            return * this;
        }
        UniqueId(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE & thePos) {
            thePos = theSource.readUnsigned64(_ptrValue, thePos);
            thePos = theSource.readUnsigned32(_myCount, thePos);
        }
        void append(asl::WriteableStream & theDest) const {
            theDest.appendUnsigned64(_ptrValue);
            theDest.appendUnsigned32(_myCount);
        }
        bool operator==(const UniqueId & theOther) const {
            return (_ptrValue == theOther._ptrValue) && (_myCount == theOther._myCount);
        }
        bool operator!=(const UniqueId & theOther) const {
            return (_ptrValue != theOther._ptrValue) || (_myCount != theOther._myCount);
        }
        bool operator<(const UniqueId & theOther) const {
            return (_ptrValue == theOther._ptrValue) ? (_myCount < theOther._myCount) : (_ptrValue < theOther._ptrValue);
        }
     private:
        void set(const UniqueId & theOther) {
            _ptrValue = theOther._ptrValue;
            _myCount = theOther._myCount;
#ifdef KEEP_COUNT_SMALL_FOR_DEBUGGING
            if (_myCount > _myCounter) {
                _myCounter = _myCount;
            }
#endif
        }
        asl::Unsigned64 _ptrValue;
        asl::Unsigned32 _myCount;
        static asl::Unsigned32 _myCounter;
    };

    ASL_DOM_DECL std::ostream& operator<<(std::ostream& os, const UniqueId & uid);

    class NodeOffsetCatalog;

    // Registry that provides fast getElementById access on all attributes that are defined as ID in xml-schema
    class NodeIDRegistry {
        friend class NodeOffsetCatalog;
    public:
#ifndef USE_SINGLE_ID_ATTRIB
        typedef std::map<DOMString, Node *> IDMap;
        typedef std::map<DOMString, IDMap> IDMaps;
#else
    #ifdef USE_HASH_MAP
        typedef boost::unordered_map<DOMString, Node *> IDMap;
    #else
        typedef std::map<DOMString, Node *> IDMap;
    #endif
#endif
        typedef std::set<Node*> NodeSet;
        typedef std::map<DOMString, NodeSet> NodeNameMap;
    public:
        void registerID(const DOMString & theIDAttributeName, const DOMString & theIDAttributeValue, dom::Node * theElement);
        void unregisterID(const DOMString & theIDAttributeName, const DOMString & theIDAttributeValue);
        const NodePtr getElementById(const DOMString & theId, const DOMString & theIdAttribute) const;
        NodePtr getElementById(const DOMString & theId, const DOMString & theIdAttribute);
#ifndef USE_SINGLE_ID_ATTRIB
        const IDMap * getIDMap(const DOMString & theIDAttributeName) const {
            IDMaps::const_iterator myMap = _myIDMaps.find(theIDAttributeName);
            if (myMap == _myIDMaps.end()) {
                return 0;
            }
            return &myMap->second;
        }
#endif
        void registerNodeName(dom::Node * theNode);
        void unregisterNodeName(dom::Node * theNode);
        NodeNameMap & getNodeNames() {
            return _myNodeNames;
        }
        NodeSet * getNodes(const DOMString & theName) {
            NodeNameMap::iterator myNodes = _myNodeNames.find(theName);
            if (myNodes != _myNodeNames.end()) {
                return &myNodes->second;
            }
            return 0;
        }
        bool hasOffsetCatalog() const {
            return _myOffsets != 0;
        }
        NodeOffsetCatalog & getOffsetCatalog();
        const NodeOffsetCatalog & getOffsetCatalog() const;
        void clearOffsetCatalog() {
            _myOffsets = asl::Ptr<NodeOffsetCatalog>();
        }
        void setOffsetCatalog(asl::Ptr<NodeOffsetCatalog> theOffsets) {
            _myOffsets = theOffsets;
        }
        void setDictionaries(DictionariesPtr theDicts) {
            _myDicts = theDicts;
        }
        DictionariesPtr getDictionaries() {
            return _myDicts;
        }
        void setStorage(asl::Ptr<asl::ReadableStreamHandle> theStorage) {
            _myStorage = theStorage;
        }
        asl::Ptr<asl::ReadableStreamHandle> getStorage() {
            return _myStorage;
        }
     private:
#ifndef USE_SINGLE_ID_ATTRIB
        IDMaps _myIDMaps;
#else
        IDMap _myIDMap;
#endif
        NodeNameMap _myNodeNames;
        mutable asl::Ptr<NodeOffsetCatalog> _myOffsets;
        mutable DictionariesPtr _myDicts;
        mutable asl::Ptr<asl::ReadableStreamHandle> _myStorage;
    };

    // collects node offsets during serialization to provide fast random access by id into binarized doms (.b60 files)
    class ASL_DOM_DECL NodeOffsetCatalog {
        public:
            typedef std::map<DOMString, asl::Unsigned64> IDMap;
            typedef std::map<DOMString, IDMap> IDMaps;
            typedef std::map<UniqueId, asl::AC_SIZE_TYPE> UIDMap;
            enum { CatalogMagic = 0xb60ca1a1, CatalogEntriesMagic = 0xb60eca1a, UIDCatalogMagic = 0xb60ca1a2, CatalogEndMagic = 0xe60cafee };
            enum { InvalidIndex = asl::AC_SIZE_TYPE(-1) };

        public:
            NodeOffsetCatalog() {}
            NodeOffsetCatalog(const Node & theRootNode);
            void extractFrom(const Node & theRootNode);

            void binarize(asl::WriteableStream & theDest) const;

            asl::AC_SIZE_TYPE
            debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos);
            bool getElementOffsetById(const DOMString & theId, const DOMString & theIdAttribute,  asl::Unsigned64 & theOffset) const;
#ifndef USE_SINGLE_ID_ATTRIB
            const IDMap * getIDMap(const DOMString & theIDAttributeName) const {
                IDMaps::const_iterator myMap = _myIDMaps.find(theIDAttributeName);
                if (myMap == _myIDMaps.end()) {
                    return 0;
                }
                return &myMap->second;
            }
#endif
            asl::AC_SIZE_TYPE enterUID(const UniqueId & theUID, asl::Unsigned64 theOffset, asl::Unsigned64 theEndOffset, asl::AC_SIZE_TYPE theParentIndex) {
                    _myParentIndex.push_back(theParentIndex);
                    asl::AC_SIZE_TYPE myIndex = _myNodeOffsets.size();
                    _myNodeOffsets.push_back(theOffset);
                    _myNodeEndOffsets.push_back(theEndOffset);
                    return myIndex;
            }
            void debug() {
                for (std::vector<asl::AC_SIZE_TYPE>::size_type i = 0; i < _myParentIndex.size();++i) {
                    AC_PRINT<<"debug:" <<" theOffset="<<_myNodeOffsets[i]<<", theEndOffset="<<_myNodeEndOffsets[i]<<", theParentIndex="<<_myParentIndex[i];
                }
            }
            asl::Unsigned64 getNodeOffset(asl::AC_SIZE_TYPE theIndex) const {
                return _myNodeOffsets[theIndex];
            }
            asl::Unsigned64 getNodeEndOffset(asl::AC_SIZE_TYPE theIndex) const {
                return _myNodeEndOffsets[theIndex];
            }
            asl::AC_SIZE_TYPE getParentIndex(asl::AC_SIZE_TYPE theIndex) const {
                return _myParentIndex[theIndex];
            }
            bool findNodeOffset(asl::Unsigned64 theOffset, asl::AC_SIZE_TYPE & theIndex) const {
                std::vector<asl::Unsigned64>::const_iterator myIndexIt =
                    std::lower_bound(_myNodeOffsets.begin(), _myNodeOffsets.end(), theOffset);
                if (myIndexIt != _myNodeOffsets.end() && !(theOffset < *myIndexIt)) {
                    theIndex = myIndexIt - _myNodeOffsets.begin();
                    return true;
                }
                return false;
            }

            void clear() {
#ifndef USE_SINGLE_ID_ATTRIB
                _myIDMaps.clear();
#else
                _myIDMap.clear();
#endif
                _myNodeOffsets.clear();
                _myNodeEndOffsets.clear();
                _myParentIndex.clear();
            }
            asl::AC_SIZE_TYPE size() const {
                if (_myNodeOffsets.size() == _myNodeEndOffsets.size() && _myNodeOffsets.size() == _myParentIndex.size()) {
                    return _myNodeOffsets.size();
                }
                throw CatalogCorrupted(JUST_FILE_LINE);
            }
        private:
#ifndef USE_SINGLE_ID_ATTRIB
            IDMaps _myIDMaps;
#else
            IDMap _myIDMap;
#endif
            std::vector<asl::Unsigned64> _myNodeOffsets;
            std::vector<asl::Unsigned64> _myNodeEndOffsets;
            std::vector<asl::AC_SIZE_TYPE> _myParentIndex;
    };

    // Registrar that provides fast getElementById access on all attributes that are defined as ID in xml-schema
    class IDValue : public StringValue {
    public:
        IDValue(Node * theNode) : StringValue(theNode) {
            // intentionally no call to update() or bumpversion() here
        }
        IDValue(const asl::ReadableBlock & theValue, Node * theNode) : StringValue(theValue, theNode) { }
        IDValue(const DOMString & theValue, Node * theNode) : StringValue(theValue, theNode) { }
        ~IDValue();
        virtual const char * name() const {
            return typeid(IDValue).name();
        }
        virtual const std::type_info & getTypeInfo() const {
            return typeid(IDValue);
        }
        virtual void update() const;
        virtual void reparent() const;
        virtual ValuePtr clone(Node * theNode) const {
            onGetValue();
            ValuePtr clonedValue = ValuePtr(new IDValue(getString(), theNode));
            clonedValue->notifyValueChanged();
            return clonedValue;
        }
        virtual ValuePtr create(Node * theNode) const {
            return ValuePtr(new IDValue(theNode));
        }
        virtual ValuePtr create(const DOMString & theValue, Node * theNode) const {
            return ValuePtr(new IDValue(theValue, theNode));
        }
        virtual ValuePtr create(const asl::ReadableBlock & theValue, Node * theNode) const {
            return ValuePtr(new IDValue(theValue, theNode));
        }
        virtual void setNodePtr(Node * theNode);
    protected:
        virtual void registerID(const DOMString & theCurrentValue) const;
        virtual void unregisterID() const;
    private:
        mutable DOMString _myOldValue;
        mutable NodeIDRegistryWeakPtr _myRegistry;
    };

    // Registry that provides fast matching of referencend node to their references defined as IDRef in xml-schema
    class NodeIDRefRegistry {
        //friend class NodeOffsetCatalog;
    public:
        typedef std::multimap<DOMString, Node *> IDRefMap;
        typedef std::map<DOMString, IDRefMap> IDRefMaps;
    public:
        void registerIDRef(const DOMString & theIDRefAttributeName, const DOMString & theIDRefAttributeValue, dom::Node * theElement);
        void unregisterIDRef(const DOMString & theIDRefAttributeName, const DOMString & theIDRefAttributeValue, dom::Node * theElement);
        //void getElementsReferencingId(const DOMString & theId, const DOMString & theIdAttribute, std::vector<NodePtr const> & theResult) const;
        void getElementsReferencingId(const DOMString & theId, const DOMString & theIdAttribute, std::vector<NodePtr> & theResult);
        const IDRefMap * getIDRefMap(const DOMString & theIDRefAttributeName) const {
            IDRefMaps::const_iterator myMap = _myIDRefMaps.find(theIDRefAttributeName);
            if (myMap == _myIDRefMaps.end()) {
                return 0;
            }
            return &myMap->second;
        }
    private:
        IDRefMaps _myIDRefMaps;
    };

    class IDRefValue : public StringValue {
    public:
        IDRefValue(Node * theNode) : StringValue(theNode) {
            // intentionally no call to update() or bumpversion() here
        }
        IDRefValue(const asl::ReadableBlock & theValue, Node * theNode) : StringValue(theValue, theNode) { }
        IDRefValue(const DOMString & theValue, Node * theNode) : StringValue(theValue, theNode) { }
        ~IDRefValue();
        virtual const char * name() const {
            return typeid(IDRefValue).name();
        }
        virtual const std::type_info & getTypeInfo() const {
            return typeid(IDRefValue);
        }
        virtual void update() const;
        virtual void reparent() const;
        virtual ValuePtr clone(Node * theNode) const {
            onGetValue();
            ValuePtr clonedValue = ValuePtr(new IDRefValue(getString(), theNode));
            clonedValue->notifyValueChanged();
            return clonedValue;
        }
        virtual ValuePtr create(Node * theNode) const {
            return ValuePtr(new IDRefValue(theNode));
        }
        virtual ValuePtr create(const DOMString & theValue, Node * theNode) const {
            return ValuePtr(new IDRefValue(theValue, theNode));
        }
        virtual ValuePtr create(const asl::ReadableBlock & theValue, Node * theNode) const {
            return ValuePtr(new IDRefValue(theValue, theNode));
        }
        virtual void setNodePtr(Node * theNode);
        virtual NodePtr getReferencedElement() const;
    protected:
        virtual void registerIDRef(const DOMString & theCurrentValue) const;
        virtual void unregisterIDRef() const;
    private:
        mutable DOMString _myOldValue;
        mutable NodeIDRefRegistryWeakPtr _myRegistry;
    };

    DEFINE_EXCEPTION(SizeMismatch,asl::Exception);
    DEFINE_PARSE_EXCEPTION(ConversionFailed,ParseException);

    // common template base class adding unboxing operations for all wrapped
    // native types Note that no references to native types should be stored
    // in client code.  References can be stored temporarily on then stack
    // using the open/close interface.  but beware of exception safety. (You
    // should use the Node::WritableValue class when possible) A value can be
    // opened for writing only by one user, trying to open an already open
    // value will throw.  The open/close mechanism is not intented for
    // multi-threaded access, it serves as a safeguard and reminder not to
    // store references.  Internally it is reuqired to perform the lazy
    // synchronization between native, binary and text representation.
    // openWriteableValue() invalidates the cached binary and text
    // representation. Reading of the native type does not invalidate these
    // caches, so no open/close is required here.
    template <class T>
    class Value : public StringValue {
    protected:
        Value(Node * theNode) : StringValue(theNode) {}
    public:
        virtual const T & getValue() const = 0;
        virtual T & openWriteableValue() = 0;
        virtual void closeWriteableValue() = 0;
        virtual bool isValueWriteable() const = 0;
        virtual void setValue(const T & theValue) = 0;
    protected:
        static T asT(const DOMString & theValue) {
            T theResult;
            if (!asl::fromString(theValue, theResult)) {
                throw ConversionFailed(
                    "could not convert string value '" + theValue + 
                    "' to type '"+typeid(T).name()+"'", "Value<T>::asT()"
                );
            }
            return theResult;
        }
    };

    // helper functions to perform dynamic casts and unboxing
    template <class T>
    T * dynamic_cast_and_openWriteableValue(ValueBase * theValueBase) {
        Value<T> * myValuePtr = dynamic_cast<Value<T>*>(theValueBase);
        if (myValuePtr) {
            return &myValuePtr->openWriteableValue();
        }
        return 0;
    }
    template <class T>
    void dynamic_cast_and_closeWriteableValue(ValueBase * theValueBase) {
        Value<T> * myValuePtr = dynamic_cast<Value<T>*>(theValueBase);
        if (myValuePtr) {
            myValuePtr->closeWriteableValue();
        }
    }

    template <class T>
    const T * dynamic_cast_Value(const ValueBase * theValueBase) {
        const Value<T> * myValuePtr = dynamic_cast<const Value<T>*>(theValueBase);
        if (myValuePtr) {
            return &myValuePtr->getValue();
        }
        return 0;
    }

    DEFINE_EXCEPTION(ValueNotNativeWriteable, asl::Exception);
    DEFINE_EXCEPTION(ValueAlreadyNativeWriteable, asl::Exception);

    // Wrapper for all POD-Types - must not be used for other C++ types
    // TODO: provide a POD-type check
    template <class T>
    class SimpleValue : public Value<T>, public asl::PoolAllocator<SimpleValue<T> > {
    public:
        SimpleValue(Node * theNode) : Value<T>(theNode),
             _myValueHasChanged(true), _isValueWriteable(false)
        {}
        SimpleValue(const T & theValue, Node * theNode) : Value<T>(theNode),
            _myValue(theValue), _myValueHasChanged(true),  _isValueWriteable(false)
        {}
        SimpleValue(const DOMString & theStringValue, Node * theNode) : Value<T>(theNode),
            _myValue(Value<T>::asT(theStringValue)), _myValueHasChanged(true), _isValueWriteable(false)
        {}

        SimpleValue(const asl::ReadableBlock & theValue, Node * theNode) : Value<T>(theNode),
            _myValueHasChanged(true), _isValueWriteable(false)
        {
            if (sizeof(T) != theValue.size()) {
                throw SizeMismatch(
                    std::string("sizeof(")+typeid(T).name()+") is " + asl::as_string(sizeof(T)) + ", block size is "+asl::as_string(theValue.size()),
                    "SimpleValue<T>(const asl::ReadableBlock & theValue)"
                    );
            }
            std::copy(theValue.begin(),theValue.end(),reinterpret_cast<unsigned char*>(&_myValue));
        }
        virtual void binarize(asl::WriteableStream & theDest) const {
            theDest.appendData(getValue());
        }
        virtual asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            thePos = theSource.readData(openWriteableValue(), thePos);
            closeWriteableValue();
            return thePos;
        }
        static void binarizeVector(const std::vector<T> & theVector, asl::WriteableStream & theDest) {
            binarizePODT(theVector, theDest);
        }
        static asl::AC_SIZE_TYPE debinarizeVector(std::vector<T> & theVector, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            return debinarizePODT(theVector, theSource, thePos);
        }
    protected:
        // This initialization is used only when a derived VectorValue has instantiated this class
        // with a non-simple stl-container as T; we must make sure that _myValue is initialized as
        // a Vector then; this is a bit obscure but keeps us from repeating the state management code
        // in the derived class; fortunately the derived class controls which constructors of this
        // class will be called, and it should call only this one:
        template <class TV>
            SimpleValue(const TV * theBegin, const TV * theEnd, Node * theNode)
                : Value<T>(theNode),
                _myValue(theBegin, theEnd), _myValueHasChanged(true), _isValueWriteable(false)
            {}

    public:
        virtual const char * name() const {
            return typeid(*this).name();
        }
        virtual const std::type_info & getTypeInfo() const {
            return typeid(T);
        }

        virtual T & openWriteableValue() {
            if (isValueWriteable()) {
                throw ValueAlreadyNativeWriteable(JUST_FILE_LINE);
            }
            setValueWriteable(true);
            _myValueHasChanged = true;
            this->onGetValue();
            return _myValue;
        }
        virtual void closeWriteableValue() {
            if (!isValueWriteable()) {
                throw ValueNotNativeWriteable(JUST_FILE_LINE);
            }
            setValueWriteable(false);
            this->bumpVersion();
            this->onSetValue();
        }
        virtual bool isValueWriteable() const {
            return _isValueWriteable;
        }
    protected:
       virtual void setValueWriteable(bool theState) {
            _isValueWriteable = theState;
        }
    protected:
        virtual unsigned char * begin() {
            _myValueHasChanged = true;
            return reinterpret_cast<unsigned char*>(&_myValue);
        }
        virtual unsigned char * end() {
            return reinterpret_cast<unsigned char*>(&_myValue) + sizeof(T);
        }
        virtual const unsigned char * begin() const {
            return reinterpret_cast<const unsigned char*>(&_myValue);
        }
        virtual const unsigned char * end() const {
            return reinterpret_cast<const unsigned char*>(&_myValue) + sizeof(T);
        }
        virtual void assign(const asl::ReadableBlock & myOtherBlock) {
            if (size() == myOtherBlock.size()) {
                std::copy(myOtherBlock.begin(),myOtherBlock.end(),begin());
                _myValueHasChanged = true;
            } else {
                throw asl::BlockSizeMismatch("Blocks have different size","SimpleValue::Assign()");
            }
            this->bumpVersion();
        }
    public:
        virtual asl::AC_SIZE_TYPE size() const {
            return sizeof(T);
        }
        virtual const DOMString & getString() const {
            updateStringFromValue();
            return StringValue::getString();
        }
        virtual void setString(const DOMString & theValue) {
            _myValue = Value<T>::asT(theValue);
            _myValueHasChanged = true;
            this->notifyValueChanged();
        }
        virtual void setStringWithoutNotification(const DOMString & theValue) {
            _myValue = Value<T>::asT(theValue);
            _myValueHasChanged = true;
            this->update();
        }
        virtual const T & getValue() const {
            this->onGetValue();
            return _myValue;
        }
        virtual void setValue(const T & theValue) {
            _myValueHasChanged = true;
            _myValue = theValue;
             this->bumpVersion();
             this->onSetValue();
        }
        virtual ValuePtr clone(Node * theNode) const {
            this->onGetValue();
            ValuePtr clonedValue = ValuePtr(new SimpleValue<T>(_myValue, theNode));
            clonedValue->notifyValueChanged();
            return clonedValue;
        }
        virtual ValuePtr create(Node * theNode) const {
            return ValuePtr(new SimpleValue<T>(theNode));
        }
        virtual ValuePtr create(const DOMString & theValue, Node * theNode) const {
            return ValuePtr(new SimpleValue<T>(theValue, theNode));
        }
        virtual ValuePtr create(const asl::ReadableBlock & theValue, Node * theNode) const {
            return ValuePtr(new SimpleValue<T>(theValue, theNode));
        }
        virtual void update() const {
            updateStringFromValue();
        }
    protected:
        virtual void freeCaches() const {
            StringValue::clearMutableString();
            _myValueHasChanged = true;
        };
       void updateStringFromValue() const {
            if (_myValueHasChanged) {
                StringValue::setMutableString(asl::as_string(_myValue));
                _myValueHasChanged = false;
            }
        }
        void setValueHasChanged(bool theFlag) {
            _myValueHasChanged = theFlag;
        }
    private:
        mutable T _myValue;
        mutable bool _myValueHasChanged;
        bool _isValueWriteable;
    };

    DEFINE_EXCEPTION(IndexOutOfRange,asl::Exception);


    // interface for vector element access
    struct AccessibleVector {
        DEFINE_NESTED_EXCEPTION(AccessibleVector,IndexOutOfRange,asl::Exception);
        DEFINE_NESTED_EXCEPTION(AccessibleVector,TypeMismatch,asl::Exception);
        virtual const char * elementName() const = 0;
        virtual const char * vectorName() const = 0;
        virtual const char * valueName() const = 0;
        virtual asl::AC_SIZE_TYPE length() const = 0;
        virtual ValuePtr getItem(asl::AC_SIZE_TYPE theIndex) const = 0;
        virtual void setItem(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) = 0;
        virtual ~AccessibleVector() {};
    };

    // helper function to access the native accessible_vector type
    typedef asl::Ptr<AccessibleVector, dom::ThreadingModel> AccessibleVectorPtr;
    inline
    AccessibleVectorPtr accessible_vector_cast(ValuePtr & p) {
        return dynamic_cast_Ptr<AccessibleVector>(p);
    }
    inline
    const AccessibleVectorPtr accessible_vector_cast(const ValuePtr & p) {
        return dynamic_cast_Ptr<AccessibleVector>(p);
    }
    inline
    AccessibleVector & accessible_vector_cast(ValueBase & v) {
        return dynamic_cast<AccessibleVector &>(v);
    }
    inline
    const AccessibleVector & accessible_vector_cast(const ValueBase & v) {
        return dynamic_cast<const AccessibleVector &>(v);
    }

    // interface for vector element access and resizing
    struct ResizeableVector : public virtual AccessibleVector {
        virtual ValuePtr getItem(asl::AC_SIZE_TYPE theIndex) const = 0;
        virtual void setItem(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) = 0;
        virtual void resize(asl::AC_SIZE_TYPE newSize) = 0;
        virtual ValuePtr getList(asl::AC_SIZE_TYPE theIndex, asl::AC_SIZE_TYPE theCount) const = 0;
        virtual void setList(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValues) = 0;
        virtual void appendItem(const ValueBase & theValue) = 0;
        virtual void appendList(const ValueBase & theValues) = 0;
        virtual void eraseItem(asl::AC_SIZE_TYPE theIndex) = 0;
        virtual void eraseList(asl::AC_SIZE_TYPE theIndex, asl::AC_SIZE_TYPE theCount) = 0;
        virtual void insertItemBefore(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) = 0;
        virtual void insertListBefore(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) = 0;
    };
    // helper function to access the native resizeable_vector type
    typedef asl::Ptr<ResizeableVector, dom::ThreadingModel> ResizeableVectorPtr;
    inline
    ResizeableVectorPtr resizeable_vector_cast(ValuePtr & p) {
        return dynamic_cast_Ptr<ResizeableVector>(p);
    }
    inline
    const ResizeableVectorPtr resizeable_vector_cast(const ValuePtr & p) {
        return dynamic_cast_Ptr<ResizeableVector>(p);
    }
    inline
    ResizeableVector & resizeable_vector_cast(ValueBase & v) {
        return dynamic_cast<ResizeableVector &>(v);
    }
    inline
    const ResizeableVector & resizeable_vector_cast(const ValueBase & v) {
        return dynamic_cast<const ResizeableVector &>(v);
    }

    // interface for raster element access and resizing
    // TODO: this should be moved somewhere else
    struct ResizeableRaster {
        virtual asl::AC_SIZE_TYPE width() const = 0;
        virtual asl::AC_SIZE_TYPE height() const = 0;
        virtual asl::Vector2<asl::AC_SIZE_TYPE> getSize() const = 0;
        virtual const asl::ReadableBlock & pixels() const = 0;
        virtual asl::WriteableBlock & pixels() = 0;
        virtual void assign(asl::AC_SIZE_TYPE newWidth, asl::AC_SIZE_TYPE newHeight, const asl::ReadableBlock & thePixels) = 0;
        virtual void resize(asl::AC_SIZE_TYPE newWidth, asl::AC_SIZE_TYPE newHeight) = 0;
        virtual void resample(asl::AC_SIZE_TYPE newWidth, asl::AC_SIZE_TYPE newHeight) = 0;
        virtual void add(const ValueBase & theRasterArg) = 0;
        virtual void sub(const ValueBase & theRasterArg) = 0;
        virtual void clear() = 0;
        virtual asl::Vector4<float> getPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y) const = 0;
        virtual void setPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y, const asl::Vector4f & theColor) = 0;

        virtual void fillRect(asl::AC_SIZE_TYPE xmin, asl::AC_SIZE_TYPE ymin,
                              asl::AC_SIZE_TYPE xmax, asl::AC_SIZE_TYPE ymax,
                              const asl::Vector4<float> & theColor) = 0;

        virtual void fillRectAlpha(asl::AC_SIZE_TYPE xmin, asl::AC_SIZE_TYPE ymin,
                              asl::AC_SIZE_TYPE xmax, asl::AC_SIZE_TYPE ymax,
                              const float theAlpha) = 0;

        virtual void randomize(const asl::Vector4f & theMinColor, const asl::Vector4f & theMaxColor) = 0;
        virtual bool hasAlpha() const = 0;

        /*
        virtual ValuePtr getPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y) const = 0;
        virtual bool setPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y, const ValueBase & theValue) = 0;
        virtual RGBA readPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y) const = 0;
        virtual void writePixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y, const RGBA & thePixel) = 0;
        virtual ValuePtr makeScaled(asl::AC_SIZE_TYPE newWidth, asl::AC_SIZE_TYPE newHeight) const = 0;
        virtual ValuePtr makeSubRaster(asl::AC_SIZE_TYPE hOrigin, asl::AC_SIZE_TYPE vOrigin, asl::AC_SIZE_TYPE hSize, asl::AC_SIZE_TYPE vSize) const = 0;
*/
        virtual void pasteRaster(const ValueBase & theSource,
                asl::AC_OFFSET_TYPE sourceX = 0, asl::AC_OFFSET_TYPE sourceY = 0,
                asl::AC_OFFSET_TYPE sourceWidth = 0, asl::AC_OFFSET_TYPE sourceHeight = 0,
                asl::AC_OFFSET_TYPE targetX = 0, asl::AC_OFFSET_TYPE targetY = 0,
                asl::AC_OFFSET_TYPE targetWidth = 0, asl::AC_OFFSET_TYPE targetHeight = 0) = 0;
        virtual ~ResizeableRaster() {};
    };

    // helper function to access the native raster type
    typedef asl::Ptr<ResizeableRaster, dom::ThreadingModel> ResizeableRasterPtr;
    inline
    ResizeableRasterPtr raster_cast(ValuePtr & p) {
        return dynamic_cast_Ptr<ResizeableRaster>(p);
    }
    inline
    const ResizeableRasterPtr raster_cast(const ValuePtr & p) {
        return dynamic_cast_Ptr<ResizeableRaster>(p);
    }
    inline
    ResizeableRaster & raster_cast(ValueBase & v) {
        return dynamic_cast<ResizeableRaster &>(v);
    }
    inline
    const ResizeableRaster & raster_cast(const ValueBase & v) {
        return dynamic_cast<const ResizeableRaster &>(v);
    }

    DEFINE_EXCEPTION(RasterArgumentSizeMismatch, asl::Exception);
    DEFINE_EXCEPTION(RasterArgumentTypeMismatch, asl::Exception);

    // A Mixin-template that turns a VectorValue into a ResizableRaster
    template <class RASTER_VALUE, class T, class PIXEL_VALUE>
    struct MakeResizeableRaster : public ResizeableRaster {
        typedef typename T::value_type PIXEL;
        typedef typename asl::SumTraits<PIXEL>::type SumType;

        MakeResizeableRaster(RASTER_VALUE & theRasterValue) : _myRasterValue(theRasterValue) {}

        asl::Vector2<asl::AC_SIZE_TYPE> getSize() const {
            return asl::Vector2<asl::AC_SIZE_TYPE>(width(), height());
        }
        asl::AC_SIZE_TYPE width() const {
            return _myRasterValue.getValue().hsize();
        }
        asl::AC_SIZE_TYPE height() const {
            return _myRasterValue.getValue().vsize();
        }

        bool hasAlpha() const {
            return PIXEL::hasAlpha(); 
        }

        const asl::ReadableBlock & pixels() const {
            _myPixels.reference(asl::ReadableBlockAdapter((const unsigned char*)asl::begin_ptr(_myRasterValue.getValue()),
                                                          (const unsigned char*)asl::end_ptr  (_myRasterValue.getValue())));
            return _myPixels;
        }
        // [ch] This interface does not respect the open/close protocol, it therefore does not
        // trigger a bumpversion or an raster dependency update.
        asl::WriteableBlock & pixels() {
            asl::WriteableBlockAdapter myAdapter((unsigned char*)asl::begin_ptr(_myRasterValue.getValue()),
                                                 (unsigned char*)asl::end_ptr  (_myRasterValue.getValue()));
            _myWriteablePixels.reference(myAdapter);
            return _myWriteablePixels;
        }

        virtual void assign(asl::AC_SIZE_TYPE newWidth, asl::AC_SIZE_TYPE newHeight, const asl::ReadableBlock & thePixels) {
            resize(newWidth, newHeight);
            T & myNativeRaster = _myRasterValue.openWriteableValue();
            asl::WriteableBlockAdapter myPixels((unsigned char*)asl::begin_ptr(myNativeRaster),
                                                (unsigned char*)asl::end_ptr  (myNativeRaster));
            myPixels.assign(thePixels);
            _myRasterValue.closeWriteableValue();
        }

        virtual void clear() {
            T & myNativeRaster = _myRasterValue.openWriteableValue();
            std::fill((unsigned char*)myNativeRaster.begin(), (unsigned char*)myNativeRaster.end(), 0);
            _myRasterValue.closeWriteableValue();
        }

        virtual asl::Vector4<float> getPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y) const {
            const T & myNativeRaster = _myRasterValue.getValue();
            PIXEL myPixel = myNativeRaster(x,y);
            asl::Vector4<float> myColor(asl::getRedValue(myPixel)   / 255.f,
                                        asl::getGreenValue(myPixel) / 255.f,
                                        asl::getBlueValue(myPixel)  / 255.f,
                                        asl::getAlphaValue(myPixel) / 255.f);
            return myColor;
        }

        virtual void setPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y, const asl::Vector4f & theColor) {
            const T & myNativeReadOnlyRaster = _myRasterValue.getValue();
            if (x < static_cast<asl::AC_SIZE_TYPE>(myNativeReadOnlyRaster.hsize()) &&
                y < static_cast<asl::AC_SIZE_TYPE>(myNativeReadOnlyRaster.vsize()))
            {
                T & myNativeRaster = _myRasterValue.openWriteableValue();
                setPixel(myNativeRaster(x,y), theColor);
                _myRasterValue.closeWriteableValue();
            }
        }
        void setPixel(PIXEL & thePixel, const asl::Vector4<float> & theColor) {
            asl::setRedValue(thePixel, asl::pchar(theColor[0] * 255));
            asl::setGreenValue(thePixel, asl::pchar(theColor[1] * 255));
            asl::setBlueValue(thePixel, asl::pchar(theColor[2] * 255));
            asl::setAlphaValue(thePixel, asl::pchar(theColor[3] * 255));
        }
        virtual void fillRect(asl::AC_SIZE_TYPE xmin, asl::AC_SIZE_TYPE ymin,
                              asl::AC_SIZE_TYPE xmax, asl::AC_SIZE_TYPE ymax,
                              const asl::Vector4<float> & theColor)
        {
            T & myNativeRaster = _myRasterValue.openWriteableValue();
            PIXEL myPixel;
            setPixel(myPixel, theColor);
            asl::AC_OFFSET_TYPE cxmax = asl::minimum(asl::AC_SIZE_TYPE(myNativeRaster.hsize()), xmax);
            asl::AC_OFFSET_TYPE cymax = asl::minimum(asl::AC_SIZE_TYPE(myNativeRaster.vsize()), ymax);
            asl::AC_OFFSET_TYPE cxmin = asl::minimum(xmin, xmax);
            asl::AC_OFFSET_TYPE cymin = asl::minimum(ymin, ymax);
            asl::subraster<PIXEL> myRegion(myNativeRaster, cxmin, cymin, cxmax-cxmin, cymax-cymin);
            std::fill(myRegion.begin(), myRegion.end(), myPixel);
            _myRasterValue.closeWriteableValue();
        }

        virtual void fillRectAlpha(asl::AC_SIZE_TYPE xmin, asl::AC_SIZE_TYPE ymin,
                                   asl::AC_SIZE_TYPE xmax, asl::AC_SIZE_TYPE ymax,
                                   const float theAlpha)
        {
            T & myNativeRaster = _myRasterValue.openWriteableValue();

            for (unsigned x=xmin; x<xmax; ++x) {
                for (unsigned y=ymin; y<ymax; ++y) {
                    asl::setAlphaValue(myNativeRaster(x,y), asl::pchar(theAlpha * 255));
                }
            }

            _myRasterValue.closeWriteableValue();
        }

        virtual void randomize(const asl::Vector4f & theMinColor,
                               const asl::Vector4f & theMaxColor)
        {
            T & myNativeRaster = _myRasterValue.openWriteableValue();

            for (unsigned x=0; x<static_cast<unsigned>(myNativeRaster.hsize()); ++x) {
                for (unsigned y=0; y<static_cast<unsigned>(myNativeRaster.vsize()); ++y) {
                    asl::Vector4f myColor = asl::Vector4f(0.0, 0.0, 0.0, 0.0);

                    for (unsigned i=0; i<4; ++i) {
                        float myRange = theMaxColor[i] - theMinColor[i];
                        if (theMinColor[i] != theMaxColor[i]) {
                            float myRandomFloat = myRange * (float)(std::rand() % 1000) / 1000.f;
                            myColor[i] = myRandomFloat + theMinColor[i];
                        } else {
                            myColor[i] = theMaxColor[i];
                        }
                    }
                    setPixel(myNativeRaster(x,y), myColor);
                }
            }

            _myRasterValue.closeWriteableValue();
        }

        virtual void pasteRaster(const ValueBase & theSource,
                                 asl::AC_OFFSET_TYPE sourceX = 0, asl::AC_OFFSET_TYPE sourceY = 0,
                                 asl::AC_OFFSET_TYPE sourceWidth = 0, asl::AC_OFFSET_TYPE sourceHeight = 0,
                                 asl::AC_OFFSET_TYPE targetX = 0, asl::AC_OFFSET_TYPE targetY = 0,
                                 asl::AC_OFFSET_TYPE targetWidth = 0, asl::AC_OFFSET_TYPE targetHeight = 0)
        {
            T & myNativeTarget = _myRasterValue.openWriteableValue();

            // bounds checking ...
            if (targetWidth == 0) {
                targetWidth = myNativeTarget.hsize() - targetX;
            }

           if (targetHeight == 0) {
                targetHeight = myNativeTarget.vsize() - targetY;
            }

            asl::Box2<asl::AC_OFFSET_TYPE> myTargetRect(targetX, targetY, targetX+targetWidth, targetY+targetHeight);
            asl::Box2<asl::AC_OFFSET_TYPE> myTargetRasterRect(0, 0, myNativeTarget.hsize(), myNativeTarget.vsize());
            if (!myTargetRasterRect.contains(myTargetRect)) {
                AC_ERROR << "pasteRaster: target rectangle is outside target raster, target raster="<< myTargetRasterRect << ", target rect="<<myTargetRect;
                return;
            }
            //const ResizeableRaster & mySourceRaster = raster_cast( theSource );
            const T * myNativeSource = dynamic_cast_Value<T>( & theSource );
            if ( myNativeSource ) {
                if (sourceWidth == 0) {
                    sourceWidth = myNativeSource->hsize() - sourceX;
                }

                if (sourceHeight == 0) {
                    sourceHeight = myNativeSource->vsize() - sourceY;
                }

                if(sourceHeight == 0 || sourceWidth == 0) return;

                asl::Box2<asl::AC_OFFSET_TYPE> mySourceRect(sourceX, sourceY, sourceX+sourceWidth, sourceY+sourceHeight);
                asl::Box2<asl::AC_OFFSET_TYPE> mySourceRasterRect(0, 0, myNativeSource->hsize(), myNativeSource->vsize());
                if (!mySourceRasterRect.contains(mySourceRect)) {
                    AC_ERROR << "pasteRaster: source rectangle is outside source raster, source raster="<< mySourceRasterRect << ", source rect="<<mySourceRect;
                    return;
                }
                asl::subraster<PIXEL> myTargetRegion( myNativeTarget, targetX, targetY, targetWidth, targetHeight);


                const asl::const_subraster<PIXEL> mySourceRegion(*myNativeSource, sourceX, sourceY, sourceWidth, sourceHeight);
                if (myTargetRect.getSize() !=mySourceRect.getSize()) {
#ifndef _WIN32
                    asl::resample(mySourceRegion, myTargetRegion, SumType());
#else
                    throw asl::NotYetImplemented(JUST_FILE_LINE);
#endif
                } else {
                    std::copy(mySourceRegion.begin(), mySourceRegion.end(), myTargetRegion.begin());
                }
            } else {
                throw RasterArgumentTypeMismatch(JUST_FILE_LINE);
            }

            _myRasterValue.closeWriteableValue();
        }
#if 0
        virtual void pasteRaster(asl::AC_OFFSET_TYPE targetX, asl::AC_OFFSET_TYPE targetY,
                          const ValueBase & theSource)
        {
            const ResizeableRaster & mySourceRaster = raster_cast( theSource );
            pasteRaster(targetX, targetY, theSource, 0, 0,
                    asl::AC_OFFSET_TYPE(mySourceRaster.width()),
                    asl::AC_OFFSET_TYPE(mySourceRaster.height()));
        }
#endif
        virtual void resize(asl::AC_SIZE_TYPE newWidth, asl::AC_SIZE_TYPE newHeight) {
            _myRasterValue.openWriteableValue().resize(newWidth, newHeight);
            _myRasterValue.closeWriteableValue();
        }

       virtual void resample(asl::AC_SIZE_TYPE newWidth, asl::AC_SIZE_TYPE newHeight) {
            T & myNative = _myRasterValue.openWriteableValue();
            asl::raster<PIXEL> myTmp(newWidth, newHeight);
#ifdef _WIN32
            (void)myNative;
            throw asl::NotYetImplemented(JUST_FILE_LINE);
#else
            asl::resample(myNative, myTmp, SumType());
            std::swap(myTmp, myNative);
           _myRasterValue.closeWriteableValue();
#endif
        }

        template <class BINARY_FUNCTION>
        void transform(const ValueBase & theRasterArg, BINARY_FUNCTION theFunction) {
            const T * myNativeRasterArg = dynamic_cast_Value<T>(&theRasterArg);
            if (myNativeRasterArg) {
                if (myNativeRasterArg->size() == _myRasterValue.getValue().size()) {
                    T & myNativeRaster = _myRasterValue.openWriteableValue();
                    std::transform(myNativeRaster.begin(), myNativeRaster.end(),
                        myNativeRasterArg->begin(),myNativeRaster.begin(), theFunction);
                    _myRasterValue.closeWriteableValue();
                } else {
                    throw RasterArgumentSizeMismatch(JUST_FILE_LINE);
                }
            } else {
                throw RasterArgumentTypeMismatch(JUST_FILE_LINE);
            }
        }
        virtual void add(const ValueBase & theRasterArg) {
            transform(theRasterArg, std::plus<typename T::value_type>());
        }
        virtual void sub(const ValueBase & theRasterArg) {
            transform(theRasterArg, std::minus<typename T::value_type>());
        }



        template <class FUNC>
        void apply(asl::AC_SIZE_TYPE xmin, asl::AC_SIZE_TYPE ymin,
                   asl::AC_SIZE_TYPE xmax, asl::AC_SIZE_TYPE ymax,
                   FUNC & theFunction)
        {
            T & myNativeRaster = _myRasterValue.openWriteableValue();
            asl::AC_OFFSET_TYPE cxmax = asl::minimum(asl::AC_SIZE_TYPE(myNativeRaster.hsize()), xmax);
            asl::AC_OFFSET_TYPE cymax = asl::minimum(asl::AC_SIZE_TYPE(myNativeRaster.vsize()), ymax);
            asl::AC_OFFSET_TYPE cxmin = asl::minimum(xmin, xmax);
            asl::AC_OFFSET_TYPE cymin = asl::minimum(ymin, ymax);
            asl::subraster<PIXEL> myRegion(myNativeRaster, cxmin, cymin, cxmax-cxmin, cymax-cymin);
            //std::fill(myRegion.begin(), myRegion.end(), myPixel);
            theFunction( myRegion );
            _myRasterValue.closeWriteableValue();
        }
        template <class FUNC>
        void apply( FUNC & theFunction) {
            T & myNativeRaster = _myRasterValue.openWriteableValue();
            theFunction(myNativeRaster);
            _myRasterValue.closeWriteableValue();
        }


/*
        ValuePtr getElement(asl::AC_SIZE_TYPE theIndex) const {
            if (theIndex < length()) {
                ValuePtr myElem = ValuePtr(new PIXEL_VALUE(_myRasterValue.getValue()[theIndex],
                    const_cast<RASTER_VALUE &>(_myRasterValue).getNodePtr()));
                return myElem;
            }
            return ValuePtr(0);
        }
        bool setElement(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) {
            const PIXEL * myElem = dynamic_cast_Value<PIXEL>(&theValue);
            if (myElem && theIndex < length()) {
                _myRasterValue.openWriteableValue()[theIndex]=*myElem;
                _myRasterValue.closeWriteableValue();
                return true;
            }
            return false;
        }
    protected:
        void tryResize(asl::AC_SIZE_TYPE newSize) {}
*/
    private:
        RASTER_VALUE & _myRasterValue;
        mutable asl::ReadableBlockAdapter _myPixels;
        asl::WriteableBlockAdapter _myWriteablePixels;
    };


    // A Mixin-template to create an opaque VectorValue, which allows no element access
    // but has all the serialization/deserialization features
    template <class VECTOR_VALUE, class T, class ELEMENT_VALUE>
    struct MakeOpaqueVector {
        MakeOpaqueVector(VECTOR_VALUE & ) {}
    };

    template <class VECTOR_VALUE, class T, class ELEMENT_VALUE> struct MakeAccessibleVector;
    template <class VECTOR_VALUE, class T, class ELEMENT_VALUE> struct MakeResizeableVector;
    template < class T, template<class,class,class> class ACCESS = MakeAccessibleVector, class ELEMENT_VALUE = SimpleValue<typename T::value_type> > class VectorValue;
    template < class T, template<class,class,class> class ACCESS, class ELEMENT_VALUE> class VectorValue;

    // A Mixin-template that turns a VectorValue into a AccessibleVector
    template <class VECTOR_VALUE, class T, class ELEMENT_VALUE>
    struct MakeAccessibleVector : public virtual AccessibleVector {
        typedef typename T::value_type ELEM;

        MakeAccessibleVector(VECTOR_VALUE & theVectorValue) : _myVectorValue(theVectorValue) {}

        const char * elementName() const {
            return typeid(ELEM).name();
        }
        const char * vectorName() const {
            return typeid(T).name();
        }
        const char * valueName() const {
            return typeid(VECTOR_VALUE).name();
        }

        asl::AC_SIZE_TYPE length() const {
            return _myVectorValue.getValue().size();
        }
        ValuePtr getItem(asl::AC_SIZE_TYPE theIndex) const {
            if (theIndex < length()) {
                ValuePtr myElem = ValuePtr(new ELEMENT_VALUE(_myVectorValue.getValue()[theIndex],
                    const_cast<VECTOR_VALUE &>(_myVectorValue).getNodePtr()));
                return myElem;
            }
            throw IndexOutOfRange(JUST_FILE_LINE);
        }
        void setItem(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) {
            const ELEM * myElem = dynamic_cast_Value<ELEM>(&theValue);
            if (!myElem) {
                // fall back to string conversion
                ELEM myElement = asl::as<ELEM>(theValue.getString());
                _myVectorValue.openWriteableValue()[theIndex]=myElement;
                _myVectorValue.closeWriteableValue();
                return;
            }
            if (theIndex < length()) {
                _myVectorValue.openWriteableValue()[theIndex]=*myElem;
                _myVectorValue.closeWriteableValue();
                return;
            }
            throw IndexOutOfRange(JUST_FILE_LINE);
        }

    protected:
        void tryResize(asl::AC_SIZE_TYPE newSize) {}
        VECTOR_VALUE & _myVectorValue;
    };


    // A Mixin-template that turns a VectorValue into a ResizeableVector
    template <class VECTOR_VALUE, class T, class ELEMENT_VALUE>
    struct MakeResizeableVector : public virtual ResizeableVector, public MakeAccessibleVector<VECTOR_VALUE,T,ELEMENT_VALUE> {
        typedef typename T::value_type ELEM;
        typedef MakeAccessibleVector<VECTOR_VALUE,T,ELEMENT_VALUE> Base;
        typedef ELEMENT_VALUE ELEMENT_VALUE_TYPE;

        MakeResizeableVector(VECTOR_VALUE & theVectorValue) : MakeAccessibleVector<VECTOR_VALUE,T,ELEMENT_VALUE>(theVectorValue) {}
        const char * elementName() const {
            return typeid(ELEM).name();
        }
        const char * vectorName() const {
            return typeid(T).name();
        }
        const char * valueName() const {
            return typeid(VECTOR_VALUE).name();
        }

        asl::AC_SIZE_TYPE length() const {
            return Base::_myVectorValue.getValue().size();
        }
        ValuePtr getItem(asl::AC_SIZE_TYPE theIndex) const {
            return Base::getItem(theIndex);
        }

        ValuePtr getList(asl::AC_SIZE_TYPE theIndex, asl::AC_SIZE_TYPE theCount) const {
            if (theIndex+theCount <= length()) {
                ValuePtr myResultValue = Base::_myVectorValue.create(0);
                const T & myVector = Base::_myVectorValue.getValue();
                T * myResults = dynamic_cast_and_openWriteableValue<T>(&*myResultValue);
                myResults->resize(theCount);
                for (asl::AC_SIZE_TYPE i = theIndex; i < theIndex + theCount;++i) {
                    (*myResults)[i-theIndex] = myVector[i];
                }
                dynamic_cast_and_closeWriteableValue<T>(&*myResultValue);
                return myResultValue;
            }
            throw AccessibleVector::IndexOutOfRange(std::string("getList: index+coount out of bounds, vector size = ")+asl::as_string(length())+
                                 +", theIndex = "+asl::as_string(theIndex)+", theCount = "+asl::as_string(theCount),PLUS_FILE_LINE);
        }
        void setList(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) {
            const AccessibleVector * myArgument = dynamic_cast<const AccessibleVector *>(&theValue);
            if (myArgument) {
                asl::AC_SIZE_TYPE myArgumentLength = myArgument->length();
                asl::AC_SIZE_TYPE myLength = this->length();
                if (theIndex + myArgumentLength <= myLength) {
                    for (asl::AC_SIZE_TYPE i = 0; i <  myArgumentLength;++i) {
                        this->setItem(theIndex + i, *myArgument->getItem(i));
                    }
                    return;
                }
                throw AccessibleVector::IndexOutOfRange(std::string("setList: too many elements to fit into existing vector, vector size = ")+
                                                    asl::as_string(myLength)+", start index = "+asl::as_string(theIndex)+
                                                    ", argument element count = "+asl::as_string(myArgumentLength),PLUS_FILE_LINE);
            }
            throw AccessibleVector::TypeMismatch("setList: argument is not a vector",PLUS_FILE_LINE);
        }

        void setItem(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) {
            Base::setItem(theIndex, theValue);
        }

        void resize(asl::AC_SIZE_TYPE newSize) {
            Base::_myVectorValue.openWriteableValue().resize(newSize);
            Base::_myVectorValue.closeWriteableValue();
        }

       void appendItem(const ValueBase & theValue) {
           const ELEM * myElem = dynamic_cast_Value<ELEM>(&theValue);
           asl::AC_SIZE_TYPE myLength = length();
           if (!myElem) {
               // try fall back to string conversion, will throw on failure
               ELEM myElement = asl::as<ELEM>(theValue.getString());
               T & myValue = Base::_myVectorValue.openWriteableValue();
               myValue.resize(myLength+1);
               myValue[myLength]= myElement;
               Base::_myVectorValue.closeWriteableValue();
               return;
           } else {
               T & myValue = Base::_myVectorValue.openWriteableValue();
               myValue.resize(myLength+1);
               myValue[myLength]= *myElem;
               Base::_myVectorValue.closeWriteableValue();
               return;
           }
           throw AccessibleVector::TypeMismatch(std::string("appendItem: argument is can not be converted to type ")+elementName(), PLUS_FILE_LINE);
        }
        void appendList(const ValueBase & theValue) {
            try {
                const AccessibleVector & myArgument = accessible_vector_cast(theValue);
                asl::AC_SIZE_TYPE myArgumentLength = myArgument.length();
                asl::AC_SIZE_TYPE myLength = this->length();
                resize(myLength+myArgumentLength);
                setList(myLength, theValue);
                return;
            } catch (std::bad_cast &) {
            }
            throw AccessibleVector::TypeMismatch("appendList: argument is not a vector", PLUS_FILE_LINE);
        }
        void eraseItem(asl::AC_SIZE_TYPE theIndex) {
            if (theIndex < length()) {
                asl::AC_SIZE_TYPE myLength = length()-1;
                T & myValue = Base::_myVectorValue.openWriteableValue();
                for (asl::AC_SIZE_TYPE i = theIndex; i < myLength; ++i) {
                    myValue[i] = myValue[i+1];
                }
                myValue.resize(myLength);
                Base::_myVectorValue.closeWriteableValue();
                return;
            }
            throw AccessibleVector::IndexOutOfRange(std::string("eraseItem: index out of bounds, index =  ")+asl::as_string(theIndex)+
                                              ", vector size = "+asl::as_string(length()), PLUS_FILE_LINE);
        }
        void eraseList(asl::AC_SIZE_TYPE theIndex, asl::AC_SIZE_TYPE theCount) {
            if (theIndex+theCount <= length()) {
                asl::AC_SIZE_TYPE myLength = length()-theCount;
                T & myValue = Base::_myVectorValue.openWriteableValue();
                for (asl::AC_SIZE_TYPE i = theIndex; i < myLength; ++i) {
                    myValue[i] = myValue[i+theCount];
                }
                myValue.resize(myLength);
                Base::_myVectorValue.closeWriteableValue();
                return;
            }
            throw AccessibleVector::IndexOutOfRange(std::string("eraseList: index+count out of bounds, index =  ")+asl::as_string(theIndex)+
                                   ", count = "+asl::as_string(theCount)+", vector size = "+asl::as_string(length()),PLUS_FILE_LINE);
        }
        void insertItemBefore(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) {
            const ELEM * myElem = dynamic_cast_Value<ELEM>(&theValue);
            if (theIndex <= length()) {
                asl::AC_SIZE_TYPE myLength = length();
                if (!myElem) {
                    // try fall back to string conversion, will throw on failure
                    ELEM myElement = asl::as<ELEM>(theValue.getString());
                    T & myValue = Base::_myVectorValue.openWriteableValue();
                    myValue.resize(myLength+1);
                    for (asl::AC_SIZE_TYPE i = myLength; i > theIndex; --i) {
                        myValue[i] = myValue[i-1];
                    }
                    myValue[theIndex]=myElement;
                    Base::_myVectorValue.closeWriteableValue();
                    return;
                } else {
                    T & myValue = Base::_myVectorValue.openWriteableValue();
                    myValue.resize(myLength+1);
                    for (asl::AC_SIZE_TYPE i = myLength; i > theIndex; --i) {
                        myValue[i] = myValue[i-1];
                    }
                    myValue[theIndex]=*myElem;
                    Base::_myVectorValue.closeWriteableValue();
                    return;
                }
            }
            throw AccessibleVector::IndexOutOfRange(std::string("insertItemBefore: index out of bounds, index =  ")+asl::as_string(theIndex)+
                                              ", vector size = "+asl::as_string(length()), PLUS_FILE_LINE);
        }
        void insertListBefore(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) {
            const AccessibleVector * myArgument = dynamic_cast<const AccessibleVector *>(&theValue);
            if (myArgument) {
                asl::AC_SIZE_TYPE myArgumentLength = myArgument->length();
                asl::AC_SIZE_TYPE myLength = this->length();
                if (theIndex <= myLength && myArgumentLength > 0) {
                    // make gap
                    T & myValue = Base::_myVectorValue.openWriteableValue();
                    myValue.resize(myLength+myArgumentLength);
                    for (asl::AC_SIZE_TYPE i = myLength+myArgumentLength-1; i >= theIndex + myArgumentLength; --i) {
                        myValue[i] = myValue[i-myArgumentLength];
                    }
                    Base::_myVectorValue.closeWriteableValue();
                    // insert values
                    setList(theIndex, theValue);
                    return;
                }
                throw AccessibleVector::IndexOutOfRange(std::string("insertListBefore: index out of bounds, index =  ")+asl::as_string(theIndex)+
                                                    ", vector size = "+asl::as_string(length()), PLUS_FILE_LINE);
            }
            throw AccessibleVector::TypeMismatch("appendList: argument is not a vector", PLUS_FILE_LINE);
        }
    protected:
        void tryResize(asl::AC_SIZE_TYPE newSize) {
            resize(newSize);
        }
    };

    template<class T>
    struct VectorTraits {
        typedef typename T::value_type value_type;
    };


    // template class for all vector and raster types
    template <
        class T,
        template<class,class,class> class ACCESS,
        class ELEMENT_VALUE
    >
    class VectorValue :
        public SimpleValue<T>,
        public ACCESS<VectorValue<T,ACCESS,ELEMENT_VALUE>,T,ELEMENT_VALUE>
    {
    public:
        typedef typename T::value_type ELEM;
        typedef ACCESS<VectorValue,T,ELEMENT_VALUE> ACCESS_TYPE;
        typedef ELEMENT_VALUE ELEMENT_VALUE_TYPE;

        bool isVector() const {
            return true;
        }

        VectorValue(Node * theNode) : SimpleValue<T>(theNode), ACCESS_TYPE(*this), _myLockedValue(0) {}
        VectorValue(const T & theValue, Node * theNode) : SimpleValue<T>(theValue, theNode), ACCESS_TYPE(*this), _myLockedValue(0) {}
        VectorValue(const DOMString & theStringValue, Node * theNode) : SimpleValue<T>(theStringValue, theNode), ACCESS_TYPE(*this), _myLockedValue(0) {}
        VectorValue(const asl::ReadableBlock & theValue, Node * theNode)
            : SimpleValue<T>(reinterpret_cast<const typename VectorTraits<T>::value_type *>(theValue.begin()),
                             reinterpret_cast<const typename VectorTraits<T>::value_type *>(theValue.end()),
                             theNode),
            ACCESS_TYPE(*this), _myLockedValue(0)
        {}

        virtual void assign(const asl::ReadableBlock & myOtherBlock) {
            asl::AC_SIZE_TYPE myNewSize =
                reinterpret_cast<const typename VectorTraits<T>::value_type *>(myOtherBlock.end()) -
                reinterpret_cast<const typename VectorTraits<T>::value_type *>(myOtherBlock.begin());

            this->tryResize(myNewSize);

            if (size() == myOtherBlock.size()) {
                openWriteableBlock();
                std::copy(myOtherBlock.begin(),myOtherBlock.end(),begin());
                closeWriteableBlock();
            } else {
                throw asl::BlockSizeMismatch("Blocks have different size","VectorValue::assign()");
            }
            this->bumpVersion();
            this->onSetValue();
        }

        // to access our data as block, we need to access the value, so we have to lock it down
        virtual asl::WriteableBlock & openWriteableBlock() {
            if (this->isBlockWriteable()) {
                throw ValueAlreadyBlockWriteable(JUST_FILE_LINE);
            }
            _myLockedValue = &(this->openWriteableValue());
            this->setBlockWriteable(true);
            this->onGetValue();
            return *this;
        }
        virtual void closeWriteableBlock() {
            if (!this->isBlockWriteable()) {
                throw ValueNotBlockWriteable(JUST_FILE_LINE);
            }
            this->closeWriteableValue();
            _myLockedValue = 0;
            this->setBlockWriteable(false);
            this->bumpVersion();
            this->onSetValue();
        }
         virtual void binarize(asl::WriteableStream & theDest) const {
             dom::binarize(this->getValue(), theDest);
        }
        virtual asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            thePos = dom::debinarize(this->openWriteableValue(), theSource, thePos);
            this->closeWriteableValue();
            return thePos;
        }
        static void binarizeVector(const std::vector<T> & theVector, asl::WriteableStream & theDest) {
            binarizeGeneric(theVector, theDest);
        }
        static asl::AC_SIZE_TYPE debinarizeVector(std::vector<T> & theVector, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            return debinarizeGeneric(theVector, theSource, thePos);
        }
    protected:
        virtual unsigned char * begin() {
            return reinterpret_cast<unsigned char*>(doBegin());
        }
        virtual unsigned char * end() {
            return reinterpret_cast<unsigned char*>(doBegin() + _myLockedValue->size());
        }
        virtual const unsigned char * begin() const {
            return reinterpret_cast<const unsigned char*>(doBegin());
        }
        virtual const unsigned char * end() const {
            return reinterpret_cast<const unsigned char*>(doBegin() + this->getValue().size());
        }
    public:
        virtual asl::AC_SIZE_TYPE size() const {
            //return sizeof(typename T::value_type) * getValue().size();
           this->onGetValue();
           return end()-begin();
        }
        virtual ValuePtr clone(Node * theNode) const {
            this->onGetValue();
            ValuePtr clonedValue = ValuePtr(new VectorValue<T, ACCESS, ELEMENT_VALUE>(this->getValue(), theNode));
            clonedValue->notifyValueChanged();
            return clonedValue;
        }
        virtual ValuePtr create(Node * theNode) const {
            return ValuePtr(new VectorValue<T, ACCESS, ELEMENT_VALUE>(theNode));
        }
        virtual ValuePtr create(const DOMString & theValue, Node * theNode) const {
            return ValuePtr(new VectorValue<T, ACCESS, ELEMENT_VALUE>(theValue, theNode));
        }
        virtual ValuePtr create(const asl::ReadableBlock & theValue, Node * theNode) const {
            return ValuePtr(new VectorValue<T, ACCESS, ELEMENT_VALUE>(theValue, theNode));
        }
    private:
        T * _myLockedValue;
        virtual ELEM * doBegin() {
            return asl::begin_ptr(*_myLockedValue);
        }
        virtual const ELEM * doBegin() const {
            return asl::begin_ptr(this->getValue());
        }
    };


    inline
    void binarize(const asl::ReadableBlock & myValue, asl::WriteableStream & theDest) {
        theDest.append(myValue);
    }
    inline
    asl::AC_SIZE_TYPE debinarize(asl::WriteableBlock & myValue, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0) {
        return theSource.readBlock(myValue,thePos);
    }

    // template class for all other user-defined types with complex memory layout
    // You have to provide binarize(), debinarize(), operator<<() and operator>>()
    // in order to use your class wrapped by ComplexValue.
    // Note that already a std::vector<std::string> must be wrapped as ComplexValue.
    /*
    template <
    class T,
    template<class,class,class> class ACCESS = MakeOpaqueVector,
    class ELEMENT_VALUE = SimpleValue<typename T::value_type>
    >
     */
    template <
    class T,
    template<class,class,class> class ACCESS,
    class ELEMENT_VALUE
    >
    class ComplexValue :
        public Value<T>,
        public ACCESS<ComplexValue<T,ACCESS,ELEMENT_VALUE>,T,ELEMENT_VALUE>,
        public asl::PoolAllocator<ComplexValue<T,ACCESS,ELEMENT_VALUE> >
    {
    public:
        typedef typename T::value_type ELEM;
        typedef ACCESS<ComplexValue,T,ELEMENT_VALUE> ACCESS_TYPE;

        ComplexValue(Node * theNode)
            : Value<T>(theNode),
            ACCESS_TYPE(*this),
            _myValueVersion(1),
            _myBlockVersion(0),
            _myStringVersion(0),
            _isValueWriteable(false)
        {}

        ComplexValue(const T & theValue, Node * theNode)
            : Value<T>(theNode),
            ACCESS_TYPE(*this),
            _myValue(theValue),
            _myValueVersion(1),
            _myBlockVersion(0),
            _myStringVersion(0),
            _isValueWriteable(false)
        {}

        ComplexValue(const DOMString & theStringValue, Node * theNode)
            : Value<T>(theNode),
            ACCESS_TYPE(*this),
            _myValue(Value<T>::asT(theStringValue)),
            _myValueVersion(1),
            _myBlockVersion(0),
            _myStringVersion(0),
            _isValueWriteable(false)
        {}

        ComplexValue(const asl::ReadableBlock & theSource, Node * theNode)
            : Value<T>(theNode),
            ACCESS_TYPE(*this),
            _myValueVersion(1),
            _myBlockVersion(0),
            _myStringVersion(0),
            _isValueWriteable(false)
        {
            dom::debinarize(_myValue, theSource);
        }
        virtual const char * name() const {
            return typeid(*this).name();
        }
        virtual const std::type_info & getTypeInfo() const {
            return typeid(T);
        }
        virtual T & openWriteableValue() {
            if (isValueWriteable()) {
                throw ValueAlreadyNativeWriteable(JUST_FILE_LINE);
            }
            setValueWriteable(true);
            updateValue();
            this->onGetValue();
           return _myValue;
        }
        virtual void closeWriteableValue() {
           if (!isValueWriteable()) {
                throw ValueNotNativeWriteable(JUST_FILE_LINE);
            }
            setValueWriteable(false);
            setNewValue();
            this->bumpVersion();
            this->onSetValue();
       }
        virtual bool isValueWriteable() const {
            return _isValueWriteable;
        }
        virtual void binarize(asl::WriteableStream & theDest) const {
             dom::binarize(getValue(), theDest);
        }
        virtual asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            thePos = dom::debinarize(openWriteableValue(), theSource, thePos);
            closeWriteableValue();
            return thePos;
        }
        static void binarizeVector(const std::vector<T> & theVector, asl::WriteableStream & theDest) {
            binarizeGeneric(theVector, theDest);
        }
        static asl::AC_SIZE_TYPE debinarizeVector(std::vector<T> & theVector, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            return debinarizeGeneric(theVector, theSource, thePos);
        }
    protected:
       virtual void setValueWriteable(bool theState) {
            _isValueWriteable = theState;
        }
    protected:
        virtual unsigned char * begin() {
            updateBlock();
            setNewBlock();
            return _myBlock.begin();
        }
        virtual unsigned char * end() {
            return _myBlock.end();
        }
        virtual const unsigned char * begin() const {
            updateBlock();
            return _myBlock.begin();
        }
        virtual const unsigned char * end() const {
            return _myBlock.end();
        }
        virtual void assign(const asl::ReadableBlock & myOtherBlock) {
            _myBlock.assign(myOtherBlock);
            setNewBlock();
            this->bumpVersion();
        }
    public:
        virtual asl::AC_SIZE_TYPE size() const {
            updateBlock();
            setNewBlock();
            this->onGetValue();
            return _myBlock.size();
        }
        virtual const DOMString & getString() const {
            updateString();
            return StringValue::getString();
        }
        virtual void setString(const DOMString & theValue) {
            _myValue = Value<T>::asT(theValue);
            setNewValue();
            this->update();
            this->bumpVersion();
            this->onSetValue();
        }
        virtual void setStringWithoutNotification(const DOMString & theValue) {
            _myValue = Value<T>::asT(theValue);
            setNewValue();
            this->update();
        }
        const T & getValue() const {
            updateValue();
            this->onGetValue();
           return _myValue;
        }
        void setValue(const T & theValue) {
            _myValue = theValue;
            setNewValue();
            this->bumpVersion();
            this->onSetValue();
        }
        virtual ValuePtr clone(Node * theNode) const {
            updateValue();
            this->onGetValue();
            ValuePtr clonedValue = ValuePtr(new ComplexValue<T, ACCESS, ELEMENT_VALUE>(_myValue, theNode));
            clonedValue->notifyValueChanged();
            return clonedValue;
        }
        virtual ValuePtr create(Node * theNode) const {
            return ValuePtr(new ComplexValue<T, ACCESS, ELEMENT_VALUE>(theNode));
        }
        virtual ValuePtr create(const DOMString & theValue, Node * theNode) const {
            return ValuePtr(new ComplexValue<T, ACCESS, ELEMENT_VALUE>(theValue, theNode));
        }
        virtual ValuePtr create(const asl::ReadableBlock & theValue, Node * theNode) const {
            return ValuePtr(new ComplexValue<T, ACCESS, ELEMENT_VALUE>(theValue, theNode));
        }
        virtual void freeCaches() const {
            setNewValue();
            StringValue::clearMutableString();
            _myBlock.resize(0);
            _myBlock.reserve(0);
        };
    protected:
        void setNewBlock() const {
            _myStringVersion = 0;
            _myValueVersion = 0;
            _myBlockVersion = 1;
        }
#if 0
        void setNewString() const {
            _myStringVersion = 1;
            _myValueVersion = 0;
            _myBlockVersion = 0;
        }
#endif
        void setNewValue() const {
            _myStringVersion = 0;
            _myValueVersion = 1;
            _myBlockVersion = 0;
       }
        void updateString() const {
            this->onGetValue();
            if ((_myStringVersion >= _myValueVersion) &&
                (_myStringVersion >= _myBlockVersion))
            {
                return;
            }
            if ((_myBlockVersion > _myValueVersion) &&
                (_myBlockVersion > _myStringVersion))
            {
                dom::debinarize(_myValue, _myBlock);
                _myValueVersion = _myBlockVersion;
            }
            if (_myValueVersion > _myStringVersion) {
                StringValue::setMutableString(asl::as_string(_myValue));
                _myStringVersion = _myValueVersion;
                return;
            }
        }
        void updateValue() const {
            if ((_myValueVersion >= _myStringVersion) &&
                (_myValueVersion >= _myBlockVersion))
            {
                return;
            }
            if ((_myBlockVersion > _myValueVersion) &&
                (_myBlockVersion > _myStringVersion))
            {
                dom::debinarize(_myValue, _myBlock);
                _myValueVersion = _myBlockVersion;
                return;
            }
            if (_myStringVersion > _myValueVersion) {
                _myValue = Value<T>::asT(StringValue::getString());
                _myValueVersion = _myStringVersion;
                return;
            }
        }

        void updateBlock() const {
            if ((_myBlockVersion >= _myStringVersion) &&
                (_myBlockVersion >= _myValueVersion))
            {
                return;
            }
            if ((_myStringVersion > _myValueVersion) &&
                (_myStringVersion > _myBlockVersion))
            {
                _myValue = Value<T>::asT(StringValue::getString());
                _myValueVersion = _myStringVersion;
            }
            if (_myValueVersion > _myBlockVersion) {
                _myBlock.resize(0);
                dom::binarize(_myValue, _myBlock);
                _myBlockVersion = _myValueVersion;
                return;
            }
        }
    private:
        mutable T _myValue;
        mutable asl::Block _myBlock;
        mutable char _myValueVersion;
        mutable char _myBlockVersion;
        mutable char _myStringVersion;
        bool _isValueWriteable;
    };

    template <>
    struct ValueWrapper<std::string> {
        typedef StringValue Type;
    };

// macros to define traits types

// This one is for simple types
#define DEFINE_VALUE_WRAPPER_TEMPLATE(TYPE, WRAPPER)\
    template <>\
    struct ValueWrapper<TYPE > {\
        typedef WRAPPER<TYPE > Type;\
    };

// You can use this one to specify one template argument for the value class, e.g. simple vector wrapper
#define DEFINE_VALUE_WRAPPER_TEMPLATE2(TYPE, WRAPPER, WRAPPER_ARG)\
    template <>\
    struct ValueWrapper<TYPE > {\
        typedef WRAPPER<TYPE, WRAPPER_ARG > Type;\
    };

// You can use this one to specify two template arguments for the value class, e.g. raster types
#define DEFINE_VALUE_WRAPPER_TEMPLATE3(TYPE, WRAPPER, WRAPPER_ARG, ELEMENT_WRAPPER)\
    template <>\
    struct ValueWrapper<TYPE > {\
        typedef WRAPPER<TYPE, WRAPPER_ARG,ELEMENT_WRAPPER > Type;\
    };

    DEFINE_VALUE_WRAPPER_TEMPLATE(bool, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(char, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(int, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(long, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(long long, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(unsigned char, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(unsigned int, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(unsigned long, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(unsigned long long, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(float, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE(double, SimpleValue);

    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::Block, VectorValue);

    // A prototype-based factory to create Value-types based on a type name
    // and giving a text oder binary representation to initialize the boxed
    // native value.
    class ASL_DOM_DECL ValueFactory {
    public:
        ValueFactory();
        ValuePtr createValue(const DOMString & theType, Node * theNode) const;
        ValuePtr createValue(const DOMString & theType, const DOMString & theValue, Node * theNode) const;
        ValuePtr createValue(const DOMString & theType, const asl::ReadableBlock & theValue, Node * theNode) const;
        void registerPrototype(const DOMString & theType, ValuePtr thePrototype);
        const ValuePtr findPrototype(const DOMString & theType) const;

        template <class T>
        const DOMString getValueName() const {
            const std::type_info & myType = typeid(T);

            ProtoMap::const_iterator it = _myPrototypes.begin();
            while(it != _myPrototypes.end()) {
                if (it->second->getTypeInfo() == myType) {
                    return it->first;
                }
                ++it;
            }
            return "";
        }
        void dump() const;
    private:
        typedef std::map<DOMString,ValuePtr > ProtoMap;
        ProtoMap _myPrototypes;
    };

    typedef asl::Ptr<ValueFactory> ValueFactoryPtr;

    ASL_DOM_DECL extern void registerStandardTypes(ValueFactory & theFactory);

    /* @} */
} //Namespace dom


#undef DB
#endif
