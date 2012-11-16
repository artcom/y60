
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

#include "DataTypes.h"
#include "Y60xsd.h"

#include <asl/dom/Nodes.h>
#include <asl/dom/Schema.h>

#include <asl/base/begin_end.h>
#include <asl/base/UnitTest.h>
#include <asl/math/linearAlgebra.h>

#include <fstream>

#include <math.h>

#include <string>
#include <iostream>

using namespace std;

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

        dom::ValueFactory myValueFactory;
        dom::registerStandardTypes(myValueFactory);
        myValueFactory.registerPrototype(_myTypeName,dom::ValuePtr(new dom::SimpleValue<T>(0)));
        SUCCESS("Initialized myValueFactory");

        dom::ValuePtr myValue(myValueFactory.createValue(_myTypeName,asl::as_string(_someVariable), 0));
        ENSURE(myValue);
        ENSURE(dom::dynamic_cast_Value<T>(myValue.get()));
        ENSURE(*dom::dynamic_cast_Value<T>(myValue.get()) == _someVariable);

        dom::ValuePtr myBinValue(myValueFactory.createValue(_myTypeName,asl::FixedBlock<T>(_someVariable),0));
        ENSURE(myBinValue);
        ENSURE(dom::dynamic_cast_Value<T>(myBinValue.get()));
        ENSURE(*dom::dynamic_cast_Value<T>(myBinValue.get()) == _someVariable);

    }

private:
    T _someVariable;
    std::string _myTypeName;
};

template <class T>
class XmlVectorValueUnitTest : public TemplateUnitTest {
public:
    XmlVectorValueUnitTest(const char * theTemplateArgument, T testValue)
        : TemplateUnitTest("XmlVectorValueUnitTest",theTemplateArgument),
          _someVariable(testValue), _myTypeName(theTemplateArgument) {}
    void run() {

        dom::VectorValue<T> myDefaultConstructedValue(0);
        SUCCESS("myDefaultConstructedValue");

        dom::VectorValue<T> myInitializedValue(_someVariable, 0);
        ENSURE(myInitializedValue.getValue() == _someVariable);

        dom::VectorValue<T> myStringInitializedValue(asl::as_string(_someVariable),0);
        ENSURE(myStringInitializedValue.getValue() == _someVariable);

        dom::ValueFactory myValueFactory;
        myValueFactory.registerPrototype(_myTypeName,dom::ValuePtr(new dom::VectorValue<T>(0)));
        SUCCESS("Initialized myValueFactory");

        dom::ValuePtr myValue(myValueFactory.createValue(_myTypeName,asl::as_string(_someVariable), 0));
        ENSURE(myValue);
        ENSURE(dom::dynamic_cast_Value<T>(myValue.get()));
        ENSURE(*dom::dynamic_cast_Value<T>(myValue.get()) == _someVariable);

        dom::ValuePtr myBinValue(myValueFactory.createValue(_myTypeName,
            asl::ReadableBlockAdapter(begin_ptr(_someVariable),begin_ptr(_someVariable) + _someVariable.size()),0));
        ENSURE(myBinValue);
        ENSURE(dom::dynamic_cast_Value<T>(myBinValue.get()));
        ENSURE(*dom::dynamic_cast_Value<T>(myBinValue.get()) == _someVariable);
    }
private:
    T _someVariable;
    std::string _myTypeName;
};

template <class T>
bool operator==(const std::vector<T> & a, const std::vector<T> & b)
{
    if (a.size() == b.size()) {
        for (typename std::vector<T>::size_type i = 0; i < a.size(); ++i) {
            if (a[i] != b[i]) {
                return false;
            }
        }
        return true;
    }
    return false;
}
template <class T>
bool operator!=(const std::vector<T> & a, const std::vector<T> & b)
{
    return !(a==b);
}

using namespace asl;

template <class T>
class BinarizeVectorUnitTest : public TemplateUnitTest {
public:
    BinarizeVectorUnitTest(const char * theTemplateArgument, std::vector<T> testValue)
        : TemplateUnitTest("BinarizeVectorUnitTest-",theTemplateArgument),
          _someVariable(testValue), _myTypeName(theTemplateArgument) {}
    void run() {

        asl::Block myBlock;
        dom::binarize(_someVariable, myBlock);
        unsigned long mySize = myBlock.size();
        ENSURE(mySize);

        std::vector<T> myOtherVariable;
        ENSURE(myOtherVariable != _someVariable);
        unsigned long myResultEnd = dom::debinarize(myOtherVariable,myBlock);
        ENSURE(myResultEnd == mySize);
        ENSURE(myOtherVariable == _someVariable);
        DPRINT(myOtherVariable);

    }
private:
    std::vector<T> _someVariable;
    std::string _myTypeName;
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new XmlValueUnitTest<asl::Vector3d>("Vector3d",asl::Vector3d(1,2,3)));
        addTest(new XmlValueUnitTest<asl::Quaternionf>("Quaternionf",asl::Quaternionf(1.0f, 2.0f, 3.0f, 4.0f)));
        typedef std::vector<asl::Vector3d> VectorOfVector3d;
        VectorOfVector3d myVector;
        myVector.push_back(asl::Vector3d(1,2,3));
        myVector.push_back(asl::Vector3d(4,5,6));
        addTest(new XmlVectorValueUnitTest<VectorOfVector3d>("VectorOfVector3d",myVector));

        std::string myFeatureString = "[10[bla,blub],11[huhu],23[none]]";
        y60::VectorOfRankedFeature myFeatureVector = asl::as<y60::VectorOfRankedFeature>(myFeatureString);
        addTest(new BinarizeVectorUnitTest<y60::RankedFeature>("VectorOfRankedFeature",myFeatureVector));
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    if (argc == 2 && string("dumpschema") == argv[1]) {
        cerr << "dumping y60 schema to file 'y60.xsd'"<<endl;
        dom::Document mySchema(y60::ourY60xsd);
        ofstream myFile("y60.xsd");
        myFile << mySchema;
    }

    mySuite.run();

    std::cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

