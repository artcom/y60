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

#ifndef _included_XmlTypes_h_
#define _included_XmlTypes_h_

#include <dom/Nodes.h>

//using namespace std;

class pfVec3;
class pfVec3d;
class pfCoord;
class pfCoordd;

template <class T>
struct as {
    as(const std::string & theString) {
        if (!fromString(theString,_myValue)) {
//TODO: use unique exception here, and have a non throwing version as well
            throw dom::ParseException("as","could not convert to desired type",0);
        }; 
    }
    operator T() const {
        return _myValue;
    }
private:
    T _myValue;
};

dom::NodePtr asNode(bool b);
dom::NodePtr asNode(char c);
dom::NodePtr asNode(unsigned char c);
dom::NodePtr asNode(int i);
dom::NodePtr asNode(unsigned int i);
dom::NodePtr asNode(short s);
dom::NodePtr asNode(unsigned short s);
dom::NodePtr asNode(long i);
dom::NodePtr asNode(unsigned long i);
dom::NodePtr asNode(long long i);
dom::NodePtr asNode(unsigned long long i);
dom::NodePtr asNode(float f);
dom::NodePtr asNode(double d);
dom::NodePtr asNode(const pfVec3 & v);
dom::NodePtr asNode(const pfVec3d & v);
dom::NodePtr asNode(const pfCoord & v);
dom::NodePtr asNode(const pfCoordd & v);


template <class ALLOC>
#if ( ! defined( __USE_GNU_STL__ ) || __GNUC__==3 )
dom::NodePtr asNode(const std::basic_string<char,std::char_traits<char>,ALLOC> & s) 
#else
dom::NodePtr asNode(const basic_string<char,string_char_traits<char>,ALLOC> & s) 
#endif
{
    dom::NodePtr myResult(new dom::Element("string"));
    (*myResult)("#text") = asl::as_string(s);
    return myResult;
};

template <class T>
bool fromString(const std::string & theString, T & outValue) {
    std::istringstream myStream(theString.c_str());
    return myStream >> outValue;
}

bool fromString(const std::string & theString, bool & outValue);

inline
bool fromString(const std::string & theString, std::string & outValue) {
    outValue = theString;
    return true;
};


template <class ALLOC>
#if ( ! defined( __USE_GNU_STL__ ) || __GNUC__==3 )
bool fromString(const std::string & theString, 
                std::basic_string<char,std::char_traits<char>,ALLOC> & outValue)  
#else
bool fromString(const std::string & theString, 
                basic_string<char,string_char_traits<char>,ALLOC> & outValue)
#endif
{
    outValue = theString.c_str();
    return true; 
};

template <class ALLOC>
#if ( ! defined( __USE_GNU_STL__ ) || __GNUC__==3 )
bool fromNode(const dom::Node & elem,
             std::basic_string<char,std::char_traits<char>,ALLOC> & outValue) 
#else
bool fromNode(const dom::Node & elem,
                basic_string<char,string_char_traits<char>,ALLOC> & outValue)
#endif
{
    if (elem.Name() == "string") {
        outValue = elem("#text").Value().c_str();
        return true; 
        
    }
    return false;
};

namespace xml {
    struct Constructible {
        virtual bool fromNode(const dom::Node & elem) = 0; 
    };

    struct Representable {
        virtual dom::NodePtr asNode() const = 0; 
    };
}

/*
inline
std::ostream & operator<<(std::ostream & os, const dom::Representable & f) {
    os << *f.asNode();
    return os;
}
*/

/*
inline
bool fromNode(const dom::Node & elem, dom::Constructible & outValue) {
    return outValue.fromNode(elem); 
}
*/

inline
bool fromNode(const dom::Node & elem, dom::Node & outValue) {
    outValue = elem;
    return true;
}

/*
inline
dom::NodePtr asNode(const dom::Representable & theValue) {
    return theValue.asNode(); 
}
*/

inline
dom::NodePtr asNode(const dom::Node & theValue) {  
    return dom::NodePtr(new dom::Node(theValue));
}

bool fromNode(const dom::Node & elem, bool & outValue);
bool fromNode(const dom::Node & elem, char & outValue);
bool fromNode(const dom::Node & elem, unsigned char & outValue);
bool fromNode(const dom::Node & elem, short & outValue);
bool fromNode(const dom::Node & elem, unsigned short & outValue);
bool fromNode(const dom::Node & elem, int & outValue);
bool fromNode(const dom::Node & elem, unsigned int & outValue);
bool fromNode(const dom::Node & elem, long & outValue);
bool fromNode(const dom::Node & elem, unsigned long & outValue);
bool fromNode(const dom::Node & elem, long long & outValue);
bool fromNode(const dom::Node & elem, unsigned long long & outValue);
bool fromNode(const dom::Node & elem, float & outValue);
bool fromNode(const dom::Node & elem, double & outValue);

bool fromNode(const dom::Node & elem, pfVec3 & vec);
bool fromNode(const dom::Node & elem, pfVec3d & vec);
bool fromNode(const dom::Node & elem, pfCoord & myCoord);
bool fromNode(const dom::Node & elem, pfCoordd & myCoord);

#endif 
