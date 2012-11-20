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

#ifndef Y60_ASS_UTILS_INCLUDED
#define Y60_ASS_UTILS_INCLUDED

#include "y60_asscore_settings.h"

#include <asl/dom/Nodes.h>
#include <asl/base/Exception.h>
#include <y60/base/DataTypes.h>

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

Y60_ASSCORE_DECL dom::NodePtr getASSSettings(dom::NodePtr theSettings);



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
    }
    throw asl::Exception(std::string("Node '") + myNode->nodeName() +
            "' does not have a text child." , PLUS_FILE_LINE);
}




void dumpBuffer(std::vector<unsigned char> & theBuffer);
void fillBufferWithString(std::vector<unsigned char> & theBuffer, const char * theString);

} // end of namespace

#endif // Y60_ASS_UTILS_INCLUDED
