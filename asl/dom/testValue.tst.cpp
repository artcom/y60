
/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testValue.tst.cpp,v $
//
//   $Revision: 1.15 $
//
//      Author:
//
// Description:
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#define USE_FORMATTED_TEST_RASTER_OUTPUT_OPERATOR

#include "Nodes.h"
#include "Schema.h"
#include "Value.h"

#include <asl/base/UnitTest.h>
#include <asl/base/Enum.h>
#include <asl/base/string_functions.h>
#include <asl/math/linearAlgebra.h>
#include <asl/raster/raster.h>
#include <asl/raster/standard_pixel_types.h>

#include <fstream>

#include <math.h>

#include <string>
#include <iostream>
#include <vector>

using namespace std;
using namespace dom;

namespace dom {
    typedef asl::raster<asl::RGB> RasterOfRGB;
    typedef asl::raster<asl::gray<float> > RasterOfGRAYF;
    DEFINE_VALUE_WRAPPER_TEMPLATE3(RasterOfRGB, dom::ComplexValue, dom::MakeResizeableRaster, dom::SimpleValue<asl::RGB>);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::RGB, SimpleValue);
    DEFINE_VALUE_WRAPPER_TEMPLATE3(RasterOfGRAYF, dom::ComplexValue, dom::MakeResizeableRaster, dom::SimpleValue<asl::gray<float> >);
    DEFINE_VALUE_WRAPPER_TEMPLATE(asl::gray<float>, SimpleValue);
}

namespace asl {


#if 0
    template <class T>
    std::ostream & printVector(std::ostream &os, const T & v, bool oneElementPerLine = false) {
        int mySize = v.end() - v.begin();
        os << "[";
        for (int i = 0; i < mySize; ++i) {
            if (oneElementPerLine) {
                os << '\n';
            }
            os << v[i];
            if (i < mySize - 1) {
                os << ",";
            }
        }
        return os << "]";
    }
#endif

 template <class T>
    std::istream & parseVector(std::istream & is, std::vector<T> & v) {
        char myChar;
        is >> myChar;

        if (myChar != '[') {
            is.setstate(std::ios::failbit);
            return is;
        }

        is >> myChar;
        if (myChar != ']') {
            is.unget();
        }
        while (myChar != ']') {
            T myElement;
            is >> myElement;

            if (!is) {
                is.setstate(std::ios::failbit);
                return is;
            }
            is >> myChar;
            if (myChar != ',' && myChar != ']') {
                is.setstate(std::ios::failbit);
                return is;
            }

            v.push_back(myElement);
        }
        return is;
    }

} // namespace asl

// Note: clang requires these functions to be in a argument-dependent namespace to qualify for ADL
namespace std {
    template <class T>
    std::ostream & operator<<(std::ostream & os, const std::vector<T> & t) {
        return asl::printVector(os,t,t.size()>4);
    }
    template <class T>
    std::istream & operator>>(std::istream & is, std::vector<T> & t) {
        return asl::parseVector<T>(is, t);
    }

}

namespace asl {
    std::ostream &
    operator << (std::ostream & theStream, const std::vector<std::string> & theStringVector);

    std::istream &
    operator >> (std::istream & theStream, std::vector<std::string> & theStringVector);

    inline
    std::ostream &
    operator << (std::ostream & theStream, const std::vector<std::string> & theStringVector) {
        theStream << "[";
	for (unsigned i = 0; i < theStringVector.size(); ++i) {
		theStream << theStringVector[i];
		if (i < theStringVector.size() - 1) {
			theStream << ",";
		}
	}
	theStream << "]";
	return theStream;
    }

inline
std::istream &
operator >> (std::istream & theStream, std::vector<std::string> & theStringVector) {
        char myChar;
        theStream >> myChar;

        if (myChar != '[') {
            theStream.setstate(std::ios::failbit);
            return theStream;
        }


        std::string myElement;
        do {
            theStream >> myChar;
            if ((myChar == ',') || (myChar == ']')) {
                theStringVector.push_back(myElement);
                myElement.clear();
                if (myChar == ']') {
                    break;
                }
            } else {
                myElement += myChar;
            }
        } while ( ! theStream.eof());

        if (myChar != ']') {
            theStream.setstate(std::ios::failbit);
        }

        return theStream;
}


template <class T>
class XmlValueUnitTest : public TemplateUnitTest {
public:
    XmlValueUnitTest(const char * theTemplateArgument, T testValue)
        : TemplateUnitTest("XmlValueUnitTest",theTemplateArgument),
          _someVariable(testValue), _myTypeName(theTemplateArgument) {}
    void run() {

		dom::SimpleValue<T> myDefaultConstructedValue(0);
		SUCCESS("myDefaultConstructedValue");

		dom::SimpleValue<T> myInitializedValue(_someVariable, 0);
		ENSURE(myInitializedValue.getValue() == _someVariable);

		dom::SimpleValue<T> myStringInitializedValue(asl::as_string(_someVariable), 0);
		ENSURE(myStringInitializedValue.getValue() == _someVariable);

        asl::Block myBinaryRep(myInitializedValue.accessReadableBlock());
        asl::Block myCopiedRep(myBinaryRep);
        dom::SimpleValue<T> myBinaryInitializedValue(myCopiedRep, 0);
        ENSURE(myBinaryInitializedValue.getValue() == _someVariable);

		dom::ValueFactory myValueFactory;
		dom::registerStandardTypes(myValueFactory);
        myValueFactory.registerPrototype(_myTypeName,dom::ValuePtr(new dom::SimpleValue<T>(0)));
		SUCCESS("Initialized myValueFactory");

		dom::ValuePtr myValue(myValueFactory.createValue(_myTypeName,asl::as_string(_someVariable),0));
		ENSURE(myValue);
		ENSURE(dom::dynamic_cast_Value<T>(myValue.get()));
		ENSURE(*dom::dynamic_cast_Value<T>(myValue.get()) == _someVariable);

		dom::ValuePtr myBinValue(myValueFactory.createValue(_myTypeName,asl::FixedBlock<T>(_someVariable),0));
		ENSURE(myBinValue);
		ENSURE(dom::dynamic_cast_Value<T>(myBinValue.get()));
		ENSURE(*dom::dynamic_cast_Value<T>(myBinValue.get()) == _someVariable);

		{
        	dom::SimpleValue<T> myConfusedValue(_someVariable, 0);
        	//ENSURE(asl::as<T>(myConfusedValue.getStringReference()) == _someVariable); // getStringReference removed from value API
        	const T * myCastedValue = dom::dynamic_cast_Value<T>(&myConfusedValue);
            ENSURE(*myCastedValue == _someVariable);
            std::string myTestString = "1";
            myConfusedValue.setString(myTestString);
            const T * myCastedValue2 = dom::dynamic_cast_Value<T>(&myConfusedValue);
            cerr << "value is: " << asl::as_string(*myCastedValue2) << " but should be: " << myTestString << endl;
            ENSURE(*myCastedValue2 == asl::as<T>(myTestString));
        }
    }

private:
    T _someVariable;
	std::string _myTypeName;
};

template <template <class,
                    template<class,class,class> class,
                    class >
         class VALUE,
         class T,
         class ELEMENT_VALUE>
struct ValueTraits;

template<class T, class ElementValue> struct ValueTraits<ComplexValue,T, ElementValue> {
    typedef ComplexValue<T, MakeResizeableVector, ElementValue> ValueType;
};

template<class T, class ElementValue> struct ValueTraits<VectorValue,T,ElementValue> {
    typedef VectorValue<T, MakeResizeableVector, ElementValue> ValueType;
};


template <template <class,
                    template<class,class,class> class,
                    class >
         class VALUE,
         class T,
         class ELEMENT_VALUE>
class XmlVectorValueUnitTest : public TemplateUnitTest {
public:
    XmlVectorValueUnitTest(const char * theTemplateArgument, T testValue)
        : TemplateUnitTest("XmlValueUnitTest-",theTemplateArgument),
          _someVariable(testValue), _myTypeName(theTemplateArgument) {}
    void run() {

        typedef typename ValueTraits<VALUE,T,ELEMENT_VALUE>::ValueType ValueType;

		ValueType myDefaultConstructedValue(0);
		SUCCESS("myDefaultConstructedValue");

		ValueType myInitializedValue(_someVariable, 0);
		ENSURE(myInitializedValue.getValue() == _someVariable);
        ValueType myOriginalValue(_someVariable, 0);
        T myOriginalVector = _someVariable;

        std::string myValueString = asl::as_string(_someVariable);
		ValueType myStringInitializedValue(myValueString, 0);
		ENSURE(myStringInitializedValue.getValue() == _someVariable);

        T mySavedValue = _someVariable;
        asl::Block myBinaryRep(myInitializedValue.accessReadableBlock());
        DDUMP(myBinaryRep.size());
        asl::Block myCopiedRep(myBinaryRep);
        ValueType myBinaryInitializedValue(myCopiedRep, 0);
        _someVariable[0] = _someVariable[1];
        DDUMP(myCopiedRep.size());
        std::fill(myCopiedRep.begin(),myCopiedRep.end(),0);
        myInitializedValue.openWriteableValue() = _someVariable;
        myInitializedValue.closeWriteableValue();

        DPRINT(myInitializedValue.getValue());
        DPRINT(myBinaryInitializedValue.getValue());
        DPRINT(_someVariable);
        ENSURE(myInitializedValue.getValue() == _someVariable);
        ENSURE(myInitializedValue.accessReadableBlock() == ValueType(_someVariable, 0).accessReadableBlock());
        ENSURE(myBinaryInitializedValue.getValue() == mySavedValue);
        ENSURE(myBinaryInitializedValue.accessReadableBlock() == ValueType(mySavedValue, 0).accessReadableBlock());

        std::cerr << _someVariable << endl;
        std::cerr << myBinaryInitializedValue.getValue() << endl;

        ENSURE(myBinaryInitializedValue.getValue() != _someVariable);

        dom::ValueFactory myValueFactory;
        myValueFactory.registerPrototype(_myTypeName,dom::ValuePtr(new ValueType(0)));
		SUCCESS("Initialized myValueFactory");

		dom::ValuePtr myValue(myValueFactory.createValue(_myTypeName,asl::as_string(_someVariable), 0));
		ENSURE(myValue);
		ENSURE(dom::dynamic_cast_Value<T>(myValue.get()));
		ENSURE(*dom::dynamic_cast_Value<T>(myValue.get()) == _someVariable);

		dom::ValuePtr myBinValue(myValueFactory.createValue(_myTypeName,ValueType(_someVariable, 0).accessReadableBlock(),0));
		ENSURE(myBinValue);
		ENSURE(dom::dynamic_cast_Value<T>(myBinValue.get()));
		ENSURE(*dom::dynamic_cast_Value<T>(myBinValue.get()) == _someVariable);

        // now test vector operations
        ValueType myTestValue(myOriginalVector, 0); // should contain three value
        ENSURE(myTestValue.length() == myOriginalVector.size());
        typedef typename ValueType::ACCESS_TYPE::ELEMENT_VALUE_TYPE ELEMENT_VALUE_TYPE;
        //typedef Ptr<ELEMENT_VALUE_TYPE,dom::ThreadingModel> ELEMENT_VALUE_PTR;

        //DPRINT(myTestValue.getString());
        for (unsigned i = 0; i < myTestValue.length(); ++i) {
            ValuePtr myElement = myTestValue.getItem(i);
            ENSURE(myElement);
            DPRINT(myElement->getString());
            ENSURE(myElement->getString() == asl::as_string(myOriginalVector[i]));
        }
        ValuePtr myElement0 = myTestValue.getItem(0);
        ENSURE(myElement0);
        ValuePtr myElement1 = myTestValue.getItem(1);
        ENSURE(myElement1);
        ValuePtr myElement2 = myTestValue.getItem(2);
        ENSURE(myElement2);

        // test item append
        //DPRINT(myTestValue.getString());
        myTestValue.appendItem(*myTestValue.getItem(0));
        //DPRINT(myTestValue.getString());
        ENSURE(myTestValue.length() == myOriginalVector.size()+1);
        ENSURE(myTestValue.getItem(myTestValue.length()-1)->getString() == myTestValue.getItem(0)->getString());

        // test list get and append
        ValuePtr myList = myTestValue.getList(1,2);
        ENSURE(myList);
        //DPRINT(myList->getString());
        myTestValue.appendList(*myList);
        //DPRINT(myList->getString());
        //DPRINT(myTestValue.getString());
        ENSURE(myTestValue.length() == myOriginalVector.size()+3);
        ENSURE(myTestValue.getItem(myTestValue.length()-1)->getString() == myTestValue.getItem(2)->getString());
        ENSURE(myTestValue.getItem(myTestValue.length()-2)->getString() == myTestValue.getItem(1)->getString());
        //DPRINT(myTestValue.getString());
        ValuePtr myFullList = myTestValue.getList(0,myTestValue.length());
        ENSURE(myFullList);
        ENSURE(myTestValue.getString() == myFullList->getString());

        // test item insertion
        myTestValue.insertItemBefore(0, *myElement0);
        ENSURE(myTestValue.getItem(0)->getString() == myElement0->getString());
        ENSURE(myTestValue.length() == myOriginalVector.size()+4);
        //DPRINT(myTestValue.getString());

        myTestValue.insertItemBefore(1, *myElement1);
        ENSURE(myTestValue.getItem(0)->getString() == myElement0->getString());
        //DPRINT(myTestValue.getString());

        myTestValue.insertItemBefore(2, *myElement2);
        ENSURE(myTestValue.length() == myOriginalVector.size()+6);
        DPRINT(myTestValue.getString());

        ValuePtr myNewList = myTestValue.getList(3,myTestValue.length()-3);
        ENSURE(myNewList->getString() == myFullList->getString());
        //DPRINT(myFullList->getString());
        //DPRINT(myNewList->getString());

        // list insertion
        myFullList = myTestValue.getList(0,myTestValue.length());
        myTestValue.insertListBefore(0, *myList);
        //DPRINT(myTestValue.getString());
        myNewList = myTestValue.getList(2,myTestValue.length()-2);
        ENSURE(myNewList->getString() == myFullList->getString());
        ValuePtr myInsertedList = myTestValue.getList(0,2);
        ENSURE(myInsertedList->getString() == myList->getString());
        //DPRINT(myInsertedList->getString());
        //DPRINT(myTestValue.getString());

        // item erase
        myNewList = myTestValue.getList(1,myTestValue.length()-1);
        unsigned myCurLen = myTestValue.length();
        myTestValue.eraseItem(0);
        ENSURE_EXCEPTION(myTestValue.eraseItem(myTestValue.length()), AccessibleVector::IndexOutOfRange);
        ENSURE(myTestValue.length() == myCurLen - 1);
        myFullList = myTestValue.getList(0,myTestValue.length());
        //DPRINT(myTestValue.getString());
        ENSURE(myNewList->getString() == myFullList->getString());

        myNewList = myTestValue.getList(2,myTestValue.length()-2);
        ENSURE(myNewList);
        myTestValue.eraseItem(1);
        myFullList = myTestValue.getList(1,myTestValue.length()-1);
        ENSURE(myNewList->getString() == myFullList->getString());
        //DPRINT(myTestValue.getString());

        myTestValue.setItem(0,*myElement0); // make list nice

        myNewList = myTestValue.getList(0,myTestValue.length()-1);
        myTestValue.eraseItem(myTestValue.length()-1); // end
        myFullList = myTestValue.getList(0,myTestValue.length());
        ENSURE(myNewList->getString() == myFullList->getString());

        //DPRINT(myTestValue.getString());

        // range erase
        myNewList = myTestValue.getList(2,myTestValue.length()-2);
        myCurLen = myTestValue.length();
        myTestValue.eraseList(0,2);
        ENSURE_EXCEPTION(myTestValue.eraseList(myTestValue.length(),1), AccessibleVector::IndexOutOfRange);
        ENSURE(myTestValue.length() == myCurLen - 2);
        myFullList = myTestValue.getList(0,myTestValue.length());
        //DPRINT(myTestValue.getString());
        ENSURE(myNewList->getString() == myFullList->getString());

        myNewList = myTestValue.getList(3,myTestValue.length()-3);
        ENSURE(myNewList);
        myTestValue.eraseList(1,2);
        myFullList = myTestValue.getList(1,myTestValue.length()-1);
        ENSURE(myNewList->getString() == myFullList->getString());
        //DPRINT(myTestValue.getString());

        myNewList = myTestValue.getList(0,myTestValue.length()-2);
        myTestValue.eraseList(myTestValue.length()-2,2); // end
        myFullList = myTestValue.getList(0,myTestValue.length());
        ENSURE(myNewList->getString() == myFullList->getString());

        //DPRINT(myTestValue.getString());
        myTestValue.eraseList(0, myTestValue.length()); // all
        ENSURE(myTestValue.length() == 0);
    }
private:
    T _someVariable;
	std::string _myTypeName;
};

/*
    template <
        class T,
        template<class,class,class> class ACCESS = MakeAccessibleVector,
        class ELEMENT_VALUE = SimpleValue<typename T::value_type>
    >
*/

//#ifdef _SETTING_GCC_TEMPLATE_BUG_WORKAROUND_
//template <template <class> class VALUE,
//          class T>
//#else
template <template <class,template<class,class,class> class,class> class VALUE,
          class T>
//#endif
class XmlRasterValueUnitTest : public TemplateUnitTest {
public:
    XmlRasterValueUnitTest(const char * theTemplateArgument, T testValue)
        : TemplateUnitTest("XmlValueUnitTest-",theTemplateArgument),
          _someVariable(testValue), _myTypeName(theTemplateArgument) {}
        typedef VALUE<T, dom::MakeResizeableRaster, SimpleValue<typename T::value_type> >VALUE_T;
    void run() {

		VALUE_T myDefaultConstructedValue(0);
		SUCCESS("myDefaultConstructedValue");

		VALUE_T myInitializedValue(_someVariable, 0);
		ENSURE(myInitializedValue.getValue() == _someVariable);

		VALUE_T myStringInitializedValue(asl::as_string(_someVariable), 0);
		ENSURE(myStringInitializedValue.getValue() == _someVariable);

        T mySavedValue = _someVariable;
        asl::Block myBinaryRep(myInitializedValue.accessReadableBlock());
        asl::Block myCopiedRep(myBinaryRep);
        VALUE_T myBinaryInitializedValue(myCopiedRep, 0);
        _someVariable[0] = _someVariable[1];
        DDUMP(myCopiedRep.size());

        myInitializedValue.openWriteableValue() = _someVariable;
        myInitializedValue.closeWriteableValue();

        typedef VALUE_T MyComplexValue;
        ENSURE(myInitializedValue.getValue() == _someVariable);
        ENSURE(myInitializedValue.accessReadableBlock() == MyComplexValue(_someVariable, 0).accessReadableBlock());
        ENSURE(myBinaryInitializedValue.getValue() == mySavedValue);
        ENSURE(myBinaryInitializedValue.accessReadableBlock() == MyComplexValue(mySavedValue, 0).accessReadableBlock());

        std::cerr << _someVariable << endl;
        std::cerr << myBinaryInitializedValue.getValue() << endl;

        ENSURE(myBinaryInitializedValue.getValue() != _someVariable);

        dom::ValueFactory myValueFactory;
        myValueFactory.registerPrototype(_myTypeName,dom::ValuePtr(new VALUE_T(0)));
		SUCCESS("Initialized myValueFactory");

		dom::ValuePtr myValue(myValueFactory.createValue(_myTypeName,asl::as_string(_someVariable), 0));
		ENSURE(myValue);
		ENSURE(dom::dynamic_cast_Value<T>(myValue.get()));
		ENSURE(*dom::dynamic_cast_Value<T>(myValue.get()) == _someVariable);

		dom::ValuePtr myBinValue(myValueFactory.createValue(_myTypeName,VALUE_T(_someVariable, 0).accessReadableBlock(),0));
		ENSURE(myBinValue);
		ENSURE(dom::dynamic_cast_Value<T>(myBinValue.get()));
		ENSURE(*dom::dynamic_cast_Value<T>(myBinValue.get()) == _someVariable);

        ResizeableRaster & myRaster = dynamic_cast<ResizeableRaster&>(*myValue);
        myRaster.resize(20, 10);
#ifdef WIN32
        try {
            myRaster.pasteRaster(myInitializedValue);
        }
        catch(const asl::Exception theEx) {
            AC_WARNING << "XmlRasterValueUnitTest::pasteRaster test is disabled under WIN32";
        }
#else
        myRaster.pasteRaster(myInitializedValue);
#endif
    }
private:
    T _someVariable;
	std::string _myTypeName;
};

template <class T>
class BinarizeVectorUnitTest : public TemplateUnitTest {
public:
    BinarizeVectorUnitTest(const char * theTemplateArgument, T testValue)
        : TemplateUnitTest("BinarizeVectorUnitTest-",theTemplateArgument),
          _someVariable(testValue), _myTypeName(theTemplateArgument)
    {}

    void testString(std::string theString) {
        DPRINT(theString);
        std::vector<char> myVector(theString.begin(), theString.end());
        asl::Block myStringBlock;
        dom::binarize(myVector, myStringBlock);
        unsigned long myCompressedStringBlockSize = myStringBlock.size();
        DPRINT(myCompressedStringBlockSize);
        ENSURE(myCompressedStringBlockSize);
        cerr << myStringBlock << endl;
        std::vector<char> myOtherVector;
        unsigned long myEndOffset = dom::debinarize(myOtherVector,myStringBlock);
        ENSURE(myEndOffset == myStringBlock.size());
        std::string myOtherString(myOtherVector.begin(), myOtherVector.end());
        ENSURE(theString == myOtherString);
    }

    void testIntString(int * theString, int theSize) {
        std::vector<int> myVector(theString, theString + theSize);
        asl::Block myStringBlock;
        dom::binarize(myVector, myStringBlock);
        unsigned long myCompressedStringBlockSize = myStringBlock.size();
        DPRINT(myCompressedStringBlockSize);
        ENSURE(myCompressedStringBlockSize);
        for (int i = 0; i < theSize;++i) {
            cerr<<myVector[i]<<" ";
        }
        cerr << endl;
        std::vector<int> myOtherVector;
        unsigned long myEndOffset = dom::debinarize(myOtherVector,myStringBlock);
        for (int i = 0; i < theSize;++i) {
            cerr<<myOtherVector[i]<<" ";
        }
        cerr << endl;
        ENSURE(myEndOffset == myStringBlock.size());
        std::string myOtherString(myOtherVector.begin(), myOtherVector.end());
        ENSURE(std::equal(myOtherVector.begin(), myOtherVector.end(), myVector.begin()));

    }

    void run() {

        asl::Block myStringBlock;
        std::string myString = "blafasel";
        dom::binarize(myString, myStringBlock);
        unsigned long myStringBlockSize = myStringBlock.size();
        ENSURE(myStringBlockSize);
        std::string myOtherString;
        unsigned long myEndOffset = dom::debinarize(myOtherString,myStringBlock);
        ENSURE(myEndOffset == myStringBlock.size());
        ENSURE(myString == myOtherString);
        std::string secondString = "hallo";
        dom::binarize(secondString,myStringBlock);
        unsigned long myNewStringBlockSize = myStringBlock.size();
        ENSURE(myNewStringBlockSize);
        std::string myRebinSecondString;
        unsigned long mySecondStringEnd = dom::debinarize(myRebinSecondString,myStringBlock,myStringBlockSize);
        ENSURE(mySecondStringEnd == myNewStringBlockSize);
        ENSURE(myRebinSecondString == secondString);

        asl::Block myBlock;
        dom::binarize(_someVariable, myBlock);
        unsigned long mySize = myBlock.size();
        ENSURE(mySize);

        T myOtherVariable;
        ENSURE(myOtherVariable != _someVariable);
        unsigned long myResultEnd = dom::debinarize(myOtherVariable,myBlock);
        ENSURE(myResultEnd == mySize);
        ENSURE(myOtherVariable == _someVariable);

        testString("aaaaa");
        testString("abcde");
        testString("ababa");
        testString("aaaab");
        testString("aabaa");
        testString("baaab");
        testString("abbba");
        testString("");
        testString("ab");
        testString("a");
        testString("aa");
        testString("aaaaaabbbbbbbaaaaaaaaaabbbbbbbb");
        testString("abbbbbbbbabaaaaaaaabbbbbbbbaaaaaaaaaaaa");

        {
            int myInts[] = {0,1,2,3,4};
            testIntString(myInts,5);
        } {
            int myInts[] = {0,0,0,0,0};
            testIntString(myInts,5);
        } {
            int myInts[] = {1,0,0,0,0};
            testIntString(myInts,5);
        } {
            int myInts[] = {0,0,0,0,1};
            testIntString(myInts,5);
        } {
            int myInts[] = {1,0,0,0,1};
            testIntString(myInts,5);
        } {
            int myInts[] = {1,0,1,0,1};
            testIntString(myInts,5);
        } {
            int myInts[] = {1,1,1,0,1};
            testIntString(myInts,5);
        } {
            int myInts[] = {1,0,1,1,1};
            testIntString(myInts,5);
        } {
            int myInts[] = {0,0,1,0,1};
            testIntString(myInts,5);
        } {
            int myInts[] = {0,0,1,0,0};
            testIntString(myInts,5);
        } {
           int myInts[] = {0,0,0,0};
            testIntString(myInts,4);
        } {
           int myInts[] = {0,0,1,0};
            testIntString(myInts,4);
        } {
           int myInts[] = {0,0,1,1};
            testIntString(myInts,4);
        } {
           int myInts[] = {0,0,0,1};
            testIntString(myInts,4);
        } {
           int myInts[] = {1,0,1,1};
            testIntString(myInts,4);
        } {
           int myInts[] = {1,0,0,1};
            testIntString(myInts,4);
        } {
           int myInts[] = {1,0,1};
            testIntString(myInts,3);
        } {
           int myInts[] = {1,0,0};
            testIntString(myInts,3);
        } {
           int myInts[] = {0,0,1};
            testIntString(myInts,3);
        } {
           int myInts[] = {1,1};
            testIntString(myInts,2);
        } {
           int myInts[] = {1,0};
            testIntString(myInts,2);
        } {
           int myInts[] = {1};
            testIntString(myInts,1);
        } {
            int myInts[] = {0};
            testIntString(myInts,0);
        }
    }
private:
    T _someVariable;
	std::string _myTypeName;
};


enum AnimalPropertyEnum {
    EDIBLE,
    PETABLE,
    RIDEABLE,
    CRUSHABLE,
    AnimalPropertyEnum_MAX
};

static const char * AnimalPropertyStrings[] = {
    "edible",
    "petable",
    "ridable",
    "crushable",
    ""
};

#define NOPARAMETER
DEFINE_BITSET( AnimalProperties, AnimalProperty, AnimalPropertyEnum, NOPARAMETER);
IMPLEMENT_BITSET( AnimalProperties, AnimalProperty, AnimalPropertyStrings );
#undef NOPARAMETER

class BitsetValueUnitTest : public UnitTest {
    public:
        BitsetValueUnitTest() : UnitTest("BitsetValueUnitTest") {  }

        void testAnimal(Document & myDoc, AnimalProperties theProps) {
            dom::NodePtr  myNode = myDoc.childNode("animal")->getAttribute("properties");
            myNode->nodeValueRefOpen<AnimalProperties>() = theProps;
            myNode->nodeValueRefClose<AnimalProperties>();

            ENSURE(as_string(theProps) == myNode->nodeValue());
            ENSURE( theProps == myNode->nodeValueRef<AnimalProperties>());

            myNode->nodeValue(as_string(theProps));
            ENSURE(as_string(theProps) == myNode->nodeValue());
            ENSURE( theProps == myNode->nodeValueRef<AnimalProperties>());

        }

        void run()
        {
            dom::Document mySchema(
                    "<xs:schema xmlns:xs='http://www.w3.org/2001/XMLSchema'>"
                    "   <xs:simpleType name='AnimalProperties'>  "
                    "     <xs:restriction base='xs:string'/> "
                    "   </xs:simpleType> "
                    "   <xs:element name='animal'>\n"
                    "       <xs:complexType>\n"
                    "           <xs:attribute name='properties' type='AnimalProperties'/>\n"
                    "       </xs:complexType>\n"
                    "   </xs:element>\n"
                    "</xs:schema>\n"
                    );
            dom::Document myDocument;
            myDocument.setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
            myDocument.getValueFactory()->registerPrototype("AnimalProperties", ValuePtr(new SimpleValue<AnimalProperties>(AnimalProperties(),0)));
            dom::registerStandardTypes(*myDocument.getValueFactory());
            myDocument.addSchema(mySchema,"");
            myDocument.parse("<animal properties='[petable]'/>");
            ENSURE(myDocument);

            testAnimal(myDocument, AnimalProperties());
            testAnimal(myDocument, AnimalProperties(1 << PETABLE));
            testAnimal(myDocument, AnimalProperties((1 << PETABLE) | (1 << CRUSHABLE)));
            testAnimal(myDocument, AnimalProperties((1 << PETABLE) | (1 << CRUSHABLE) | (1 << RIDEABLE) | (1 << EDIBLE) ));
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message

        addTest(new XmlValueUnitTest<int>("int",23));
        addTest(new XmlValueUnitTest<float>("float",43.23f));

        float myNumber[3] = { 0.0f, 1.0f, 2.0f };
        std::vector<float> myVec(myNumber, myNumber + 3);
        addTest(new XmlVectorValueUnitTest<dom::VectorValue,std::vector<float>,SimpleValue<float> >("vector<float>",myVec));

        const char * myStrings[5] = { "onehunderedtwentyonethousendfivehunderes","",
            "two", "threemillionfourhundredthousandandfourbillion",
        ""};
        std::vector<DOMString> myStringVec(myStrings, myStrings + 4);
        addTest(new BinarizeVectorUnitTest<std::vector<DOMString> >("vector<string>",myStringVec));
        addTest(new XmlVectorValueUnitTest<dom::ComplexValue,std::vector<DOMString>,StringValue >("vector<string>",myStringVec));

        asl::raster<asl::RGB> myRaster(3,4,RGB(10,11,12));
        myRaster(0,0) = RGB(20,21,22);
        addTest(new XmlRasterValueUnitTest<dom::ComplexValue,asl::raster<asl::RGB> >("raster<RGB>",myRaster));
        asl::raster<gray<float> > myFRaster(3,4,gray<float>(10.0));
        myFRaster(0,0) = 1.0;
        addTest(new XmlRasterValueUnitTest<dom::ComplexValue,asl::raster<gray<float> > >("raster<gray<float> >",myFRaster));
        addTest(new BitsetValueUnitTest());
    }
};

}; // asl

int main(int argc, char *argv[]) {

    asl::MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    std::cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

