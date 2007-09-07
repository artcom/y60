//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef Y60_ASS_UTILS_INCLUDED
#define Y60_ASS_UTILS_INCLUDED

#include <dom/Nodes.h>
#include <asl/Exception.h>
#include <y60/DataTypes.h>

namespace y60 {

typedef dom::ValueWrapper<y60::RasterOfGRAY>::Type::ACCESS_TYPE Raster;
typedef asl::Ptr<Raster, dom::ThreadingModel> RasterPtr;

struct RasterHandle {
    RasterHandle(dom::ValuePtr theValue) :
        value( theValue ),
        raster( dynamic_cast_Ptr<Raster>( theValue ) )
    {}
    dom::ValuePtr value;
    RasterPtr raster;
};

enum ASSEventType {
    ASS_FRAME,
    ASS_LOST_SYNC,
    ASS_LOST_COM,
    ASS_INVALID
};

static const char *ASSEventTypes[] = { "ASS_FRAME", "ASS_LOST_SYNC", "ASS_LOST_COM", "ASS_INVALID" };

struct ASSEvent {
    ASSEvent(ASSEventType theType) : type( theType ) {}
    ASSEvent() : type( ASS_INVALID ) {}

    ASSEvent(const asl::Vector2i & theSize, unsigned frame, unsigned char * theData) :
        type( ASS_FRAME ), frameno( frame), size( theSize ), data( theData ) {}

    ASSEventType type;
    unsigned     frameno;
    asl::Vector2i size;
    unsigned char * data;
};

inline std::ostream & operator<<(std::ostream & os, const ASSEvent &theEvent) {
    os << "ASSEvent " << ASSEventTypes[theEvent.type] << " with size = " << theEvent.size;
    return os;
}

DEFINE_EXCEPTION( ASSException, asl::Exception );

dom::NodePtr getASSSettings(dom::NodePtr theSettings);

// XXX: The config functions might be worth publishing [DS]
template <class T>
bool
getConfigSetting(dom::NodePtr theSettings, const std::string & theName, T & theValue,
                 const T & theDefault)
{
    dom::NodePtr myNode = theSettings->childNode( theName );
    if ( ! myNode ) {
        AC_WARNING << "No node named '" << theName << "' found in configuration. "
                   << "Adding default value '" << theDefault << "'";
        myNode = theSettings->appendChild( dom::NodePtr( new dom::Element( theName )));
        dom::NodePtr myTextNode = myNode->appendChild( dom::NodePtr( new dom::Text() ));
        myTextNode->nodeValue( asl::as_string( theDefault ));
    }

    if ( myNode->childNodesLength() != 1 ) {
        throw asl::Exception(std::string("Configuration node '") + theName +
            "' must have exactly one child.", PLUS_FILE_LINE);
    }
    if ( myNode->childNode("#text") ) {
        T myNewValue = asl::as<T>( myNode->childNode("#text")->nodeValue() );
        if (myNewValue != theValue) {
            theValue =  myNewValue;
            return true;
        } else {
            return false;
        }
    } else {
        throw asl::Exception(std::string("Node '") + myNode->nodeName() + 
                "' does not have a text child." , PLUS_FILE_LINE);
    }
}

template <class Enum>
bool
getConfigSetting(dom::NodePtr theSettings, const std::string & theName, Enum & theValue,
                 typename Enum::Native theDefault)
{
    return getConfigSetting( theSettings, theName, theValue, Enum( theDefault ));
}


template <class T>
bool
settingChanged(dom::NodePtr theSettings, const std::string & theName, T & theValue) {
    dom::NodePtr myNode = theSettings->childNode( theName );
    if ( ! myNode ) {
        throw ASSException(std::string("No node named '") + theName +
                "' found in configuration.", PLUS_FILE_LINE);
    }

    if ( myNode->childNodesLength() != 1 ) {
        throw asl::Exception(std::string("Configuration node '") + theName +
            "' must have exactly one child.", PLUS_FILE_LINE);
    }
    if ( myNode->childNode("#text") ) {
        T myNewValue = asl::as<T>( myNode->childNode("#text")->nodeValue() );
        if (myNewValue != theValue) {
            AC_PRINT << theName << " changed. was: " << theValue << " is: " << myNewValue;
            return true;
        } else {
            return false;
        }
    } else {
        throw asl::Exception(std::string("Node '") + myNode->nodeName() + 
                "' does not have a text child." , PLUS_FILE_LINE);
    }
    return true; // avoid a warning
}




void dumpBuffer(std::vector<unsigned char> & theBuffer);
void fillBufferWithString(std::vector<unsigned char> & theBuffer, const char * theString);

} // end of namespace

#endif // Y60_ASS_UTILS_INCLUDED
