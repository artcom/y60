/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: Value.h,v $
//
//   $Revision: 1.34 $
//
// Description: tiny fast XML-Parser and DOM
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _xml_Value_h_
#define _xml_Value_h_

#include "typedefs.h"
#include "Exceptions.h"
#include "Field.h"

#include <asl/Block.h>
#include <asl/MemoryPool.h>
#include <asl/raster.h>
#include <asl/subraster.h>
#include <asl/standard_pixel_types.h>
#include <asl/Vector234.h>

#include <asl/pixels.h>
#include <asl/Logger.h>

#include <map>
#include <typeinfo>

namespace asl {
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
		virtual ValuePtr clone(Node * theNode) const = 0;
        /// create default constructed value
		virtual ValuePtr create(Node * theNode) const = 0;
        ///create Value from string
		virtual ValuePtr create(const DOMString & theValue,Node * theNode) const = 0;
        ///create Value from binary representation
		virtual ValuePtr create(const asl::ReadableBlock & theValue, Node * theNode) const = 0;
		virtual void bumpVersion() = 0;

        virtual void set(const ReadableBlock & theBlock) {
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
        virtual Node * getNodePtr() = 0;
        virtual void binarize(asl::WriteableStream & theDest) const = 0;
        virtual asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) = 0;
    protected:
        virtual void setNodePtr(Node * theNode) = 0;
        virtual void reparent() const {
            const_cast<ValueBase*>(this)->bumpVersion();
        }
	};


    template <class VALUE>
    struct ValueWrapper;

    template <class T>
    void binarize(const T & myValue, asl::WriteableStream & theDest) {
        typedef typename ValueWrapper<T>::Type CheckType; // make sure only wrapped native values get automatically binarized
        theDest.appendData(myValue);
    };

    template <class T>
    asl::AC_SIZE_TYPE debinarize(T & myValue, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0) {
        typedef typename ValueWrapper<T>::Type CheckType; // make sure only wrapped native values get automatically binarized
        return theSource.readData(myValue,thePos);
    };

    inline
    void binarize(const std::string & myValue, asl::WriteableStream & theDest) {
        theDest.appendCountedString(myValue);
    };
    inline
    asl::AC_SIZE_TYPE debinarize(std::string & myValue, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0) {
        return theSource.readCountedString(myValue,thePos);
    };

#ifdef UNCOMPRESSED_VECTORS
    template <class T>
        void binarize(const std::vector<T> & myValue, asl::WriteableStream & theDest) {
            theDest.appendUnsigned(myValue.size());
            for (int i = 0; i < myValue.size(); ++i) {
                binarize(myValue[i],theDest);
            }
        };
    template <class T>
        asl::AC_SIZE_TYPE debinarize(std::vector<T> & myValue, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0) {
            asl::AC_SIZE_TYPE myCount;
            thePos = theSource.readUnsigned(myCount,thePos);
            myValue.resize(myCount);
            for (int i = 0; i < myCount; ++i) {
                thePos = debinarize(myValue[i],theSource,thePos);
            }
            return thePos;
        };
#else
    template <class T>
    void binarize(const std::vector<T> & theVector, asl::WriteableStream & theDest) {
        return ValueWrapper<T>::Type::binarizeVector(theVector, theDest);
    };
    template <class T>
    asl::AC_SIZE_TYPE debinarize(std::vector<T> & theVector, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0) {
        return ValueWrapper<T>::Type::debinarizeVector(theVector, theSource, thePos);
    };

   template <class T>
        void binarizeGeneric(const std::vector<T> & theVector, asl::WriteableStream & theDest) {
            theDest.appendUnsigned(theVector.size());
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
        };
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
        };

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
        };
    template <class T>
        void binarizePODT(const std::vector<T> & theVector, asl::WriteableStream & theDest) {
            theDest.appendUnsigned(theVector.size());
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
        };
#endif
   template <class T, class Alloc, class D>
   void binarize(const asl::raster<T, Alloc, D> & myValue, asl::WriteableStream & theDest) {
       theDest.appendUnsigned(myValue.hsize());
       theDest.appendUnsigned(myValue.vsize());
       binarize(myValue.getDataVector(), theDest);
   };
   template <class T, class Alloc, class D>
   asl::AC_SIZE_TYPE debinarize(asl::raster<T, Alloc, D> & myValue, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0) {
       asl::AC_SIZE_TYPE hsize;
       asl::AC_SIZE_TYPE vsize;
       thePos = theSource.readUnsigned(hsize,thePos);
       thePos = theSource.readUnsigned(vsize,thePos);
       myValue.resize(hsize, vsize);
       thePos = debinarize(myValue.getDataVector(),theSource,thePos);
       return thePos;
   };

    class StringValue : public ValueBase {
	public:
        StringValue(Node * theNode) : _isBlockWriteable(false), _myNode(theNode) {
            // intentionally no call to update() or bumpversion() here
        }
		StringValue(const asl::ReadableBlock & theValue, Node * theNode)
            : _myStringValue(theValue.strbegin(),theValue.size()), _isBlockWriteable(false), _myNode(theNode)
        {
            update();
            bumpVersion();
        }
		StringValue(const DOMString & theStringValue, Node * theNode)
            : _myStringValue(theStringValue), _isBlockWriteable(false), _myNode(theNode)
        {
            update();
            bumpVersion();
        }
		virtual const char * name() const {
			return typeid(DOMString).name();
		}
		virtual const std::type_info & getTypeInfo() const {
			return typeid(DOMString);
		}
		virtual void bumpVersion();
        virtual void binarize(asl::WriteableStream & theDest) const {
            onGetValue();
            theDest.appendCountedString(_myStringValue);
        }
        virtual asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            thePos = theSource.readCountedString(_myStringValue, thePos);
            update();
            bumpVersion();
            onSetValue();
            return thePos;
        }
        static void binarizeVector(const std::vector<DOMString> & theVector, asl::WriteableStream & theDest) {
            binarizeGeneric(theVector, theDest);
        };
        static asl::AC_SIZE_TYPE debinarizeVector(std::vector<DOMString> & theVector, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            return debinarizeGeneric(theVector, theSource, thePos);
        };
    protected:
		virtual unsigned char * begin() {
            if (!this->isBlockWriteable()) {
                throw ValueNotBlockWriteable(JUST_FILE_LINE);
            }
			return reinterpret_cast<unsigned char*>(&(*_myStringValue.begin()));
		}
        virtual unsigned char * end() {
            if (!this->isBlockWriteable()) {
                throw ValueNotBlockWriteable(JUST_FILE_LINE);
            }
            return reinterpret_cast<unsigned char*>(&(*_myStringValue.end()));
        }
		virtual const unsigned char * begin() const {
			return reinterpret_cast<const unsigned char*>(&(*_myStringValue.begin()));
		}
		virtual const unsigned char * end() const {
			return reinterpret_cast<const unsigned char*>(&(*_myStringValue.end()));
		}
        operator bool() const {
            return true;
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
            update();
            bumpVersion();
            onSetValue();
	    }
		virtual ValuePtr clone(Node * theNode) const {
            onGetValue();
			return ValuePtr(new StringValue(_myStringValue, theNode));
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
        virtual void update() const {
        }
		virtual void assign(const asl::ReadableBlock & myOtherBlock) {
			_myStringValue.resize(myOtherBlock.size());
			std::copy(myOtherBlock.begin(),myOtherBlock.end(),&_myStringValue[0]);
            update();
            bumpVersion();
            onSetValue();
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
            update();
            bumpVersion();
            onSetValue();
        }
        virtual bool isBlockWriteable() const {
            return _isBlockWriteable;
        }
	protected:
		virtual void setMutableString(const DOMString & theValue) const {
			const_cast<DOMString&>(_myStringValue) = theValue;
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
		DOMString _myStringValue;
        bool _isBlockWriteable;
        mutable Node * _myNode;
	};

    class NodeIDRegistry {
    public:
        typedef std::map<DOMString, Node *> IDMap;
        typedef std::map<DOMString, IDMap> IDMaps;
    public:
        void registerID(const DOMString & theIDAttributeName, const DOMString & theIDAttributeValue, dom::Node * theElement);
        void unregisterID(const DOMString & theIDAttributeName, const DOMString & theIDAttributeValue);
        const NodePtr getElementById(const DOMString & theId, const DOMString & theIdAttribute) const;
        NodePtr getElementById(const DOMString & theId, const DOMString & theIdAttribute);
        const IDMap * getIDMap(const DOMString & theIDAttributeName) const {
            IDMaps::const_iterator myMap = _myIDMaps.find(theIDAttributeName);
            if (myMap == _myIDMaps.end()) {
                return 0;
            }
            return &myMap->second;
        }
    private:
        IDMaps _myIDMaps;
    };

    class IDValue : public StringValue {
    public:
        IDValue(Node * theNode) : StringValue(theNode) {
            // intentionally no call to update() or bumpversion() here
        }
		IDValue(const asl::ReadableBlock & theValue, Node * theNode) : StringValue(theValue, theNode)
        {
            update();
        }
		IDValue(const DOMString & theValue, Node * theNode) : StringValue(theValue, theNode)
        {
            update();
        }
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
			return ValuePtr(new IDValue(getString(), theNode));
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

	DEFINE_EXCEPTION(SizeMismatch,asl::Exception);
	DEFINE_PARSE_EXCEPTION(ConversionFailed,ParseException);

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
            if (!asl::fromString(theValue,theResult)) {
                throw ConversionFailed("could not convert string value '"+theValue+"' to type '"+typeid(T).name()+"'",
                    "Value<T>::asT()");
            };
            return theResult;
        }

    };

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

    template <class T>
    class SimpleValue : public Value<T>, public asl::PoolAllocator<SimpleValue<T> > {
    public:
        SimpleValue(Node * theNode) : Value<T>(theNode),
              _myValueHasChanged(true), _myStringHasChanged(false), _isValueWriteable(false) {}
        SimpleValue(const T & theValue, Node * theNode) : Value<T>(theNode),
            _myValueHasChanged(true), _myStringHasChanged(false), _myValue(theValue), _isValueWriteable(false) {}
        SimpleValue(const DOMString & theStringValue, Node * theNode) : Value<T>(theNode),
            _myValue(Value<T>::asT(theStringValue)),
            _myValueHasChanged(true), _myStringHasChanged(false), _isValueWriteable(false) {}
        SimpleValue(const asl::ReadableBlock & theValue, Node * theNode) : Value<T>(theNode),
            _myValueHasChanged(true), _myStringHasChanged(false), _isValueWriteable(false)
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
        };
        static asl::AC_SIZE_TYPE debinarizeVector(std::vector<T> & theVector, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            return debinarizePODT(theVector, theSource, thePos);
        };
    protected:
        // This initialization is used only when a derived VectorValue has instantiated this class
        // with a non-simple stl-container as T; we must make sure that _myValue is initialized as
        // a Vector then; this is a bit obscure but keeps us from repeating the state management code
        // in the derived class; fortunately the derived class controls which constructors of this
        // class will be called, and it should call only this one:
        template <class TV>
            SimpleValue(const TV * theBegin, const TV * theEnd, Node * theNode)
                : Value<T>(theNode),
                _myValueHasChanged(true), _myStringHasChanged(false), _myValue(theBegin, theEnd), _isValueWriteable(false)
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
            updateValueFromString();
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
            updateValueFromString();
            _myValueHasChanged = true;
            return reinterpret_cast<unsigned char*>(&_myValue);
        }
        virtual unsigned char * end() {
            return reinterpret_cast<unsigned char*>(&_myValue) + sizeof(T);
        }
        virtual const unsigned char * begin() const {
            updateValueFromString();
            return reinterpret_cast<const unsigned char*>(&_myValue);
        }
        virtual const unsigned char * end() const {
            updateValueFromString();
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
            _myStringHasChanged = false;
             this->bumpVersion();
             this->onSetValue();
       }
        virtual const T & getValue() const {
            updateValueFromString();
            this->onGetValue();
            return _myValue;
        }
        virtual void setValue(const T & theValue) {
            _myValueHasChanged = true;
            _myStringHasChanged = false;
            _myValue = theValue;
             this->bumpVersion();
             this->onSetValue();
        }
        virtual ValuePtr clone(Node * theNode) const {
            updateValueFromString();
            this->onGetValue();
            return ValuePtr(new SimpleValue<T>(_myValue, theNode));
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
            updateValueFromString();
            updateStringFromValue();
        };
    protected:
            void updateValueFromString() const {
                if (_myStringHasChanged) {
                    _myValue = Value<T>::asT(StringValue::getString());
                    _myValueHasChanged = false;
                    _myStringHasChanged = true; // make sure we get a fresh formatted string
                }
            }
            void updateStringFromValue() const {
                if (_myValueHasChanged) {
                    StringValue::setMutableString(asl::as_string(_myValue));
                    _myValueHasChanged = false;
                    _myStringHasChanged = false;
                }
            }
            void setValueHasChanged(bool theFlag) {
                _myValueHasChanged = theFlag;
            }
    private:
        mutable T _myValue;
        mutable bool _myValueHasChanged;
        mutable bool _myStringHasChanged;
        bool _isValueWriteable;
    };

    struct AccessibleVector {
		virtual const char * elementName() const = 0;
        virtual asl::AC_SIZE_TYPE length() const = 0;
        virtual ValuePtr getElement(asl::AC_SIZE_TYPE theIndex) const = 0;
        virtual bool setElement(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) = 0;
    };

    struct ResizeableVector : public AccessibleVector {
        virtual void resize(asl::AC_SIZE_TYPE newSize) = 0;
        virtual bool append(const ValueBase & theValue) = 0;
        virtual bool erase(asl::AC_SIZE_TYPE theIndex) = 0;
        virtual bool insertBefore(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) = 0;
    };

    struct ResizeableRaster {
        virtual ~ResizeableRaster() {};
        virtual asl::AC_SIZE_TYPE width() const = 0;
        virtual asl::AC_SIZE_TYPE height() const = 0;
        virtual const asl::ReadableBlock & pixels() const = 0;
        virtual asl::WriteableBlock & pixels() = 0;
        virtual void assign(asl::AC_SIZE_TYPE newWidth, asl::AC_SIZE_TYPE newHeight, const asl::ReadableBlock & thePixels) = 0;
        virtual void resize(asl::AC_SIZE_TYPE newWidth, asl::AC_SIZE_TYPE newHeight) = 0;
        virtual void add(const ValueBase & theRasterArg) = 0;
        virtual void sub(const ValueBase & theRasterArg) = 0;
        virtual void clear() = 0;
        virtual asl::Vector4<float> getPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y) const = 0;
        virtual void setPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y, float r, float g, float b, float a) = 0;

//        virtual void setPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y, const asl::Vector4<float> & theColor) = 0;
        virtual void fillRect(asl::AC_SIZE_TYPE xmin, asl::AC_SIZE_TYPE ymin,
                              asl::AC_SIZE_TYPE xmax, asl::AC_SIZE_TYPE ymax,
                              float r, float g, float b, float a) = 0;
//        virtual void fillRect(asl::AC_SIZE_TYPE xmin, asl::AC_SIZE_TYPE ymin,
//                              asl::AC_SIZE_TYPE xmax, asl::AC_SIZE_TYPE ymax,
//                              const asl::Vector4<float> & theColor) = 0;

        /*
        virtual ValuePtr getPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y) const = 0;
        virtual bool setPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y, const ValueBase & theValue) = 0;
        virtual RGBA readPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y) const = 0;
        virtual void writePixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y, const RGBA & thePixel) = 0;
        virtual ValuePtr makeScaled(asl::AC_SIZE_TYPE newWidth, asl::AC_SIZE_TYPE newHeight) const = 0;
        virtual ValuePtr makeSubRaster(asl::AC_SIZE_TYPE hOrigin, asl::AC_SIZE_TYPE vOrigin, asl::AC_SIZE_TYPE hSize, asl::AC_SIZE_TYPE vSize) const = 0;
        virtual ValuePtr pasteRaster(asl::AC_SIZE_TYPE hOrigin, asl::AC_SIZE_TYPE vOrigin, const ValueBase & theRaster) = 0;
*/
        virtual void pasteRaster(asl::AC_OFFSET_TYPE targetX, asl::AC_OFFSET_TYPE targetY, const ValueBase & theSource,
                                 asl::AC_OFFSET_TYPE sourceX, asl::AC_OFFSET_TYPE sourceY, 
                                 asl::AC_OFFSET_TYPE sourceWidth, asl::AC_OFFSET_TYPE sourceHeight) = 0;
        virtual void pasteRaster(asl::AC_OFFSET_TYPE targetX, asl::AC_OFFSET_TYPE targetY, const ValueBase & theSource) = 0;

    };
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

    template <class RASTER_VALUE, class T, class PIXEL_VALUE>
    struct MakeResizeableRaster : public ResizeableRaster {
        typedef typename T::value_type PIXEL;

        MakeResizeableRaster(RASTER_VALUE & theRasterValue) : _myRasterValue(theRasterValue) {}

        asl::AC_SIZE_TYPE width() const {
            return _myRasterValue.getValue().hsize();
        }
        asl::AC_SIZE_TYPE height() const {
            return _myRasterValue.getValue().vsize();
        }
        const asl::ReadableBlock & pixels() const {
            _myPixels.reference(asl::ReadableBlockAdapter((const unsigned char*)&(*_myRasterValue.getValue().begin()),
                                              (const unsigned char*)&(*_myRasterValue.getValue().end())));
            return _myPixels;
        }
        asl::WriteableBlock & pixels() {
            asl::WriteableBlockAdapter myAdapter((unsigned char*)&(*_myRasterValue.getValue().begin()),
                                                 (unsigned char*)&(*_myRasterValue.getValue().end()));
            _myWriteablePixels.reference(myAdapter);
            return _myWriteablePixels;
        }

        virtual void assign(asl::AC_SIZE_TYPE newWidth, asl::AC_SIZE_TYPE newHeight, const asl::ReadableBlock & thePixels) {
            resize(newWidth, newHeight);
            T & myNativeRaster = _myRasterValue.openWriteableValue();
            asl::WriteableBlockAdapter myPixels((unsigned char*)&(*myNativeRaster.begin()),
                                                (unsigned char*)&(*myNativeRaster.end()));
            myPixels.assign(thePixels);
            _myRasterValue.closeWriteableValue();
        }

        virtual void clear() {
            std::fill(pixels().begin(), pixels().end(), 0);
        }

        virtual asl::Vector4<float> getPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y) const {
            const T & myNativeRaster = _myRasterValue.getValue();
            PIXEL myPixel = myNativeRaster(x,y);
            asl::Vector4<float> myColor((float)asl::getRedValue(myPixel), (float)asl::getGreenValue(myPixel),
                                        (float)asl::getBlueValue(myPixel), (float)asl::getAlphaValue(myPixel));
            return myColor;
        }
        virtual void setPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y, float r, float g, float b, float a) {
//        virtual void setPixel(asl::AC_SIZE_TYPE x, asl::AC_SIZE_TYPE y, const asl::Vector4<float> & thePixel) {
            const T & myNativeReadOnlyRaster = _myRasterValue.getValue();
            if (x < myNativeReadOnlyRaster.hsize() &&
                y < myNativeReadOnlyRaster.vsize()) {
                T & myNativeRaster = _myRasterValue.openWriteableValue();
                asl::Vector4<float> thePixel(r,g,b,a);
                setPixel(myNativeRaster(x,y), thePixel);
                _myRasterValue.closeWriteableValue();
            }
        }
        void setPixel(PIXEL & thePixel, const asl::Vector4<float> & theColor) {
            asl::setRedValue(thePixel, asl::pchar(theColor[0]));
            asl::setGreenValue(thePixel, asl::pchar(theColor[1]));
            asl::setBlueValue(thePixel, asl::pchar(theColor[2]));
            asl::setAlphaValue(thePixel, asl::pchar(theColor[3]));
        }
        virtual void fillRect(asl::AC_SIZE_TYPE xmin, asl::AC_SIZE_TYPE ymin,
                              asl::AC_SIZE_TYPE xmax, asl::AC_SIZE_TYPE ymax,
                              float r, float g, float b, float a)
//        virtual void fillRect(asl::AC_SIZE_TYPE xmin, asl::AC_SIZE_TYPE ymin,
//                              asl::AC_SIZE_TYPE xmax, asl::AC_SIZE_TYPE ymax,
//                              const asl::Vector4<float> & theColor)
        {
            asl::Vector4<float> theColor(r,g,b,a);
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

        virtual void pasteRaster(asl::AC_OFFSET_TYPE targetX, asl::AC_OFFSET_TYPE targetY,
                                 const ValueBase & theSource,
                                 asl::AC_OFFSET_TYPE sourceX, asl::AC_OFFSET_TYPE sourceY,
                                 asl::AC_OFFSET_TYPE sourceWidth, asl::AC_OFFSET_TYPE sourceHeight)
        {
            T & myNativeTarget = _myRasterValue.openWriteableValue();
            // TODO: bounds checking ...
            const ResizeableRaster & mySourceRaster = raster_cast( theSource );
            asl::subraster<PIXEL> myTargetRegion( myNativeTarget, targetX, targetY,
                    sourceWidth, sourceHeight);

            const T * myNativeSource = dynamic_cast_Value<T>( & theSource );
            if ( myNativeSource ) {
                const asl::const_subraster<PIXEL> mySourceRegion( *myNativeSource, sourceX, sourceY, 
                        sourceWidth, sourceHeight);
                std::copy(mySourceRegion.begin(), mySourceRegion.end(), myTargetRegion.begin());
            } else {
                throw RasterArgumentTypeMismatch(JUST_FILE_LINE);
            }

            _myRasterValue.closeWriteableValue();
        }

        virtual void pasteRaster(asl::AC_OFFSET_TYPE targetX, asl::AC_OFFSET_TYPE targetY,
                          const ValueBase & theSource)
        {
            const ResizeableRaster & mySourceRaster = raster_cast( theSource );
            pasteRaster(targetX, targetY, theSource, 0, 0, 
                    asl::AC_OFFSET_TYPE(mySourceRaster.width()), 
                    asl::AC_OFFSET_TYPE(mySourceRaster.height()));
        }

        virtual void resize(asl::AC_SIZE_TYPE newWidth, asl::AC_SIZE_TYPE newHeight) {
            _myRasterValue.openWriteableValue().resize(newWidth, newHeight);
            _myRasterValue.closeWriteableValue();
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


    template <class VECTOR_VALUE, class T, class ELEMENT_VALUE>
    struct MakeOpaqueVector {
        MakeOpaqueVector(VECTOR_VALUE & ) {}
    };

    template <class VECTOR_VALUE, class T, class ELEMENT_VALUE>
    struct MakeAccessibleVector : public AccessibleVector {
        typedef typename T::value_type ELEM;

        MakeAccessibleVector(VECTOR_VALUE & theVectorValue) : _myVectorValue(theVectorValue) {}
		const char * elementName() const {
            return typeid(ELEM).name();
        }

        asl::AC_SIZE_TYPE length() const {
            return _myVectorValue.getValue().size();
        }
        ValuePtr getElement(asl::AC_SIZE_TYPE theIndex) const {
            if (theIndex < length()) {
                ValuePtr myElem = ValuePtr(new ELEMENT_VALUE(_myVectorValue.getValue()[theIndex],
                    const_cast<VECTOR_VALUE &>(_myVectorValue).getNodePtr()));
                return myElem;
            }
            return ValuePtr(0);
        }
        bool setElement(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) {
            const ELEM * myElem = dynamic_cast_Value<ELEM>(&theValue);
            if (!myElem) {
                // fall back to string conversion
                ELEM myElement = asl::as<ELEM>(theValue.getString());
                _myVectorValue.openWriteableValue()[theIndex]=myElement;
                _myVectorValue.closeWriteableValue();
                return true;
            }
            if (theIndex < length()) {
                _myVectorValue.openWriteableValue()[theIndex]=*myElem;
                _myVectorValue.closeWriteableValue();
                return true;
            }
            return false;
        }
    protected:
        void tryResize(asl::AC_SIZE_TYPE newSize) {}
    private:
        VECTOR_VALUE & _myVectorValue;
    };

    template <class VECTOR_VALUE, class T, class ELEMENT_VALUE>
    struct MakeResizeableVector : public ResizeableVector {
        typedef typename T::value_type ELEM;

        MakeResizeableVector(VECTOR_VALUE & theVectorValue) : _myVectorValue(theVectorValue) {}
		const char * elementName() const {
            return typeid(ELEM).name();
        }

        asl::AC_SIZE_TYPE length() const {
            return _myVectorValue.getValue().size();
        }
        ValuePtr getElement(asl::AC_SIZE_TYPE theIndex) const {
            if (theIndex < length()) {
                ValuePtr myElem = ValuePtr(new ELEMENT_VALUE(_myVectorValue.getValue()[theIndex],
                                    const_cast<VECTOR_VALUE &>(_myVectorValue).getNodePtr()));
                return myElem;
            }
            return ValuePtr(0);
        }
        bool setElement(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) {
            const ELEM * myElem = dynamic_cast_Value<ELEM>(&theValue);
            if (!myElem) {
                // fall back to string conversion
                ELEM myElement = asl::as<ELEM>(theValue.getString());
                _myVectorValue.openWriteableValue()[theIndex]=myElement;
                _myVectorValue.closeWriteableValue();
                return true;
            }
            if (theIndex < length()) {
                _myVectorValue.openWriteableValue()[theIndex]=*myElem;
                _myVectorValue.closeWriteableValue();
                return true;
            }
            return false;
        }

        void resize(asl::AC_SIZE_TYPE newSize) {
            _myVectorValue.openWriteableValue().resize(newSize);
            _myVectorValue.closeWriteableValue();
        }

       bool append(const ValueBase & theValue) {
           const ELEM * myElem = dynamic_cast_Value<ELEM>(&theValue);
            if (myElem) {
                asl::AC_SIZE_TYPE myLength = length();
                T & myValue = _myVectorValue.openWriteableValue();
                myValue.resize(myLength+1);
                myValue[myLength]=*myElem;
                _myVectorValue.closeWriteableValue();
                return true;
            }
            return false;
        }
        bool erase(asl::AC_SIZE_TYPE theIndex) {
            if (theIndex < length()) {
                asl::AC_SIZE_TYPE myLength = length()-1;
                T & myValue = _myVectorValue.openWriteableValue();
                for (int i = theIndex; i < myLength; ++i) {
                    myValue[i] = myValue[i+1];
                }
                myValue.resize(myLength);
                _myVectorValue.closeWriteableValue();
                return true;
            }
            return false;
        }
        bool insertBefore(asl::AC_SIZE_TYPE theIndex, const ValueBase & theValue) {
            const ELEM * myElem = dynamic_cast_Value<ELEM>(&theValue);
            if (myElem && theIndex <= length()) {
                asl::AC_SIZE_TYPE myLength = length();
                T & myValue = _myVectorValue.openWriteableValue();
                myValue.resize(myLength+1);
                for (int i = theIndex; i < myLength; ++i) {
                    myValue[i+1] = myValue[i];
                }
                myValue[theIndex]=*myElem;
                _myVectorValue.closeWriteableValue();
                return true;
            }
            return false;
        }
    protected:
        void tryResize(asl::AC_SIZE_TYPE newSize) {
            resize(newSize);
        }
    private:
        VECTOR_VALUE & _myVectorValue;
    };

    template<class T>
    struct VectorTraits {
        typedef typename T::value_type value_type;
    };

    template <
        class T,
        template<class,class,class> class ACCESS = MakeAccessibleVector,
        class ELEMENT_VALUE = SimpleValue<typename T::value_type>
    >
    class VectorValue :
        public SimpleValue<T>,
#ifdef _SETTING_GCC_TEMPLATE_BUG_WORKAROUND_
        public ACCESS<VectorValue<T,ACCESS,ELEMENT_VALUE>,T,ELEMENT_VALUE>
#else
        public ACCESS<VectorValue,T,ELEMENT_VALUE>
#endif
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
        };
        static asl::AC_SIZE_TYPE debinarizeVector(std::vector<T> & theVector, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            return debinarizeGeneric(theVector, theSource, thePos);
        };
    protected:
        virtual unsigned char * begin() {
            return reinterpret_cast<unsigned char*>(&(*_myLockedValue->begin()));
        }
        virtual unsigned char * end() {
            return reinterpret_cast<unsigned char*>(&(*_myLockedValue->end()));
        }
        virtual const unsigned char * begin() const {
            return reinterpret_cast<const unsigned char*>(&(*(this->getValue()).begin()));
        }
        virtual const unsigned char * end() const {
            return reinterpret_cast<const unsigned char*>(&(*(this->getValue()).end()));
        }
    public:
        virtual asl::AC_SIZE_TYPE size() const {
            //return sizeof(typename T::value_type) * getValue().size();
           this->onGetValue();
           return end()-begin();
        }
        virtual ValuePtr clone(Node * theNode) const {
            this->updateValueFromString();
            this->onGetValue();
            return ValuePtr(new VectorValue<T, ACCESS, ELEMENT_VALUE>(this->getValue(), theNode));
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
    };


    inline
    void binarize(const asl::ReadableBlock & myValue, asl::WriteableStream & theDest) {
        theDest.append(myValue);
    };
    inline
    asl::AC_SIZE_TYPE debinarize(asl::WriteableBlock & myValue, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0) {
        return theSource.readBlock(myValue,thePos);
    };

    template <
        class T,
        template<class,class,class> class ACCESS = MakeOpaqueVector,
        class ELEMENT_VALUE = SimpleValue<typename T::value_type>
    >
    class ComplexValue :
        public Value<T>,
        public ACCESS<ComplexValue<T,ACCESS,ELEMENT_VALUE>,T,ELEMENT_VALUE>,
        public asl::PoolAllocator<ComplexValue<T> >
    {
    public:
        typedef typename T::value_type ELEM;
        typedef ACCESS<ComplexValue,T,ELEMENT_VALUE> ACCESS_TYPE;

        ComplexValue(Node * theNode)
            : Value<T>(theNode), ACCESS_TYPE(*this), _myValueVersion(1),  _myStringVersion(0), _myBlockVersion(0), _isValueWriteable(false)
        {}

        ComplexValue(const T & theValue, Node * theNode)
            : Value<T>(theNode),
            ACCESS_TYPE(*this),
            _myValueVersion(1),
            _myStringVersion(0),
            _myBlockVersion(0),
            _myValue(theValue),
            _isValueWriteable(false)
        {}

        ComplexValue(const DOMString & theStringValue, Node * theNode)
            : Value<T>(theNode),
            ACCESS_TYPE(*this),
            _myValue(Value<T>::asT(theStringValue)),
            _myValueVersion(1),
            _myStringVersion(0),
            _myBlockVersion(0),
            _isValueWriteable(false)
        {}

        ComplexValue(const asl::ReadableBlock & theSource, Node * theNode)
            : Value<T>(theNode),
            ACCESS_TYPE(*this),
            _myValueVersion(1), _myStringVersion(0), _myBlockVersion(0), _isValueWriteable(false)
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
        };
        static asl::AC_SIZE_TYPE debinarizeVector(std::vector<T> & theVector, const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            return debinarizeGeneric(theVector, theSource, thePos);
        };
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
            this->bumpVersion();
            setNewValue();
            this->onSetValue();
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
            return ValuePtr(new ComplexValue<T, ACCESS, ELEMENT_VALUE>(_myValue, theNode));
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
    protected:
        void setNewBlock() const {
            _myStringVersion = 0;
            _myValueVersion = 0;
            _myBlockVersion = 1;
        }
        void setNewString() const {
            _myStringVersion = 1;
            _myValueVersion = 0;
            _myBlockVersion = 0;
        }
        void setNewValue() const {
            _myStringVersion = 0;
            _myValueVersion = 1;
            _myBlockVersion = 0;
       }
        void updateString() const {
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


#define DEFINE_VALUE_WRAPPER_TEMPLATE(TYPE, WRAPPER)\
    template <>\
    struct ValueWrapper<TYPE > {\
        typedef WRAPPER<TYPE > Type;\
    };

#define DEFINE_VALUE_WRAPPER_TEMPLATE2(TYPE, WRAPPER, WRAPPER_ARG)\
    template <>\
    struct ValueWrapper<TYPE > {\
        typedef WRAPPER<TYPE, WRAPPER_ARG > Type;\
    };

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

    class ValueFactory {
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

	extern void registerStandardTypes(ValueFactory & theFactory);

    /* @} */
} //Namespace dom



#endif
