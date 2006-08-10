
/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-97, ART+COM GmbH Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: XmlTypes.c++,v $
//
//   $Revision: 1.2 $
//
//      Author: 
//
// Description: 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _XmlTypes_h_
#define _XmlTypes_h_

#include "XmlTypes.h"
#include <sstream>
//#include <Performer/pr/pfLinMath.h>
#include <asl/string_functions.h>

using namespace std;  // automatically added!

bool fromString(const string & theString, bool & outValue) {
    if (theString == "true" || theString=="1") {
        outValue = true;
        return true;
    } 
    if (theString == "false" || theString=="0") {
        outValue = false;
        return true;
    }
    return false; 
}

bool fromString(const string & theString, int & outValue) {
    std::istringstream myStream(theString.c_str());
    return (myStream >> outValue);
}

template <class T>
bool fromNode(const dom::Node & elem, T & outValue, const char * theTypeName) {
    if (elem.nodeName() == theTypeName) {
        return fromString(elem("#text").nodeValue(), outValue);
    }
    return false;
}


bool fromNode(const dom::Node & elem, bool & outValue) {
    return fromNode(elem, outValue, "bool");
}
bool fromNode(const dom::Node & elem, char & outValue) {
    return fromNode(elem, outValue, "char");
}
bool fromNode(const dom::Node & elem, unsigned char & outValue) {
    return fromNode(elem, outValue, "uchar");
}
bool fromNode(const dom::Node & elem, short & outValue) {
    return fromNode(elem, outValue, "short");
}
bool fromNode(const dom::Node & elem, unsigned short & outValue) {
    return fromNode(elem, outValue, "ushort");
}
bool fromNode(const dom::Node & elem, int & outValue) {
    return fromNode(elem, outValue, "int");
}
bool fromNode(const dom::Node & elem, unsigned int & outValue) {
    return fromNode(elem, outValue, "uint");
}
bool fromNode(const dom::Node & elem, long & outValue) {
    return fromNode(elem, outValue, "long");
}
bool fromNode(const dom::Node & elem, unsigned long & outValue) {
    return fromNode(elem, outValue, "ulong");
}
bool fromNode(const dom::Node & elem, long long & outValue) {
    return fromNode(elem, outValue, "longlong");
}
bool fromNode(const dom::Node & elem, unsigned long long & outValue) {
    return fromNode(elem, outValue, "ulonglong");
}
bool fromNode(const dom::Node & elem, float & outValue) {
    return fromNode(elem, outValue, "float");
}
bool fromNode(const dom::Node & elem, double & outValue) {
    return fromNode(elem, outValue, "double");
}



dom::NodePtr asNode(bool b) {
    dom::NodePtr myResult(new dom::Element("bool"));
    (*myResult)("#text") = asl::as_string(b);
    return myResult;
}

dom::NodePtr asNode(char i) {
    dom::NodePtr myResult(new dom::Element("char"));
    (*myResult)("#text") = asl::as_string(i);
    return myResult;
}

dom::NodePtr asNode(unsigned char i) {
    dom::NodePtr myResult(new dom::Element("uchar"));
    (*myResult)("#text") = asl::as_string(i);
    return myResult;
}

dom::NodePtr asNode(int i) {
    dom::NodePtr myResult(new dom::Element("int"));
    (*myResult)("#text") = asl::as_string(i);
    return myResult;
}

dom::NodePtr asNode(unsigned int i) {
    dom::NodePtr myResult(new dom::Element("uint"));
    (*myResult)("#text") = asl::as_string(i);
    return myResult;
}


dom::NodePtr asNode(long i) {
    dom::NodePtr myResult(new dom::Element("long"));
    (*myResult)("#text") = asl::as_string(i);
    return myResult;
}

dom::NodePtr asNode(unsigned long i) {
    dom::NodePtr myResult(new dom::Element("ulong"));
    (*myResult)("#text") = asl::as_string(i);
    return myResult;
}

dom::NodePtr asNode(long long i) {
    dom::NodePtr myResult(new dom::Element("longlong"));
    (*myResult)("#text") = asl::as_string(i);
    return myResult;
}

dom::NodePtr asNode(unsigned long long i) {
    dom::NodePtr myResult(new dom::Element("ulonglong"));
    (*myResult)("#text") = asl::as_string(i);
    return myResult;
}

dom::NodePtr asNode(float f) {
    dom::NodePtr myResult(new dom::Element("float"));
    (*myResult)("#text") = asl::as_string(f);
    return myResult;
}

dom::NodePtr asNode(short s) {
    dom::NodePtr myResult(new dom::Element("short"));
    (*myResult)("#text") = asl::as_string(s);
    return myResult;
}

dom::NodePtr asNode(double d) {
    dom::NodePtr myResult(new dom::Element("double"));
    (*myResult)("#text") = asl::as_string(d);
    return myResult;
}

template <class VEC>
dom::NodePtr asNode(const VEC & v,
                    unsigned int myVectorSize,
                    const string & myVectorTypeName)
{
    dom::NodePtr myResult(new dom::Element(myVectorTypeName));
    for (int i = 0; i < myVectorSize; ++i) {
        myResult->appendChild(asNode(v[i]));
    }
    return myResult; 
}

dom::NodePtr asNode(const asl::Vector3f & v) {
    return asNode(v,3,"Vec3");
}

dom::NodePtr asNode(const asl::Vector3d & v) {
    return asNode(v,3,"Vec3d");
}

/*
dom::NodePtr asNode(const pfCoord & v) {
    dom::NodePtr myResult(new dom::Element("Coord"));
    (*myResult)("xyz").AddChild(asNode(v.xyz));
    (*myResult)("hpr").AddChild(asNode(v.hpr));
    return myResult;
}

dom::NodePtr asNode(const pfCoordd & v) {
    dom::NodePtr myResult(new dom::Element("Coordd"));
    (*myResult)("xyz").AddChild(asNode(v.xyz));
    (*myResult)("hpr").AddChild(asNode(v.hpr));
    return myResult;
}
*/

template <class VEC>
bool fromNode(const dom::Node & theElement,
        VEC & outVector,
        unsigned int desiredSize,
        const string & myVectorTypeName,
        const string & myValueTypeName)
{
    int i = 0;
    if (theElement.nodeName() == myVectorTypeName) {
        while (i < desiredSize) {
            if (!fromNode(theElement(myValueTypeName,i),outVector[i])) {
                return false;
            }
            ++i;
        }
    }
    return i == desiredSize;
}
bool fromNode(const dom::Node & elem, asl::Vector3f & vec) {
    return fromNode(elem, vec, 3, "Vec3", "float");
}

bool fromNode(const dom::Node & elem, asl::Vector3d & vec) {
    return fromNode(elem, vec, 3, "Vec3d", "double");
}

/*
bool fromNode(const dom::Node & elem, pfCoord & myCoord) 
{
    if (elem.Name() == "Coord") {
        return fromNode(elem("xyz")("Vec3"), myCoord.xyz) &&
               fromNode(elem("hpr")("Vec3"), myCoord.hpr);  
    }
    return false;
}

bool fromNode(const dom::Node & elem, pfCoordd & myCoord) {
    if (elem.Name() == "Coordd") {
        return fromNode(elem("xyz")("Vec3d"), myCoord.xyz) &&
               fromNode(elem("hpr")("Vec3d"), myCoord.hpr);  
    }
    return false;
}
*/

#endif

