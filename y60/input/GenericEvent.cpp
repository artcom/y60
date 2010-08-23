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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "GenericEvent.h"


namespace y60 {

GenericEvent::GenericEvent(const std::string & theCallback,
                           const dom::NodePtr & theSchemaDoc,
                           const dom::ValueFactoryPtr & theValueFactory) :
    Event( GENERIC, asl::Time() ),
    _myDocument( new dom::Document() )
{


    if ( theValueFactory ) {
        _myDocument->setValueFactory( theValueFactory );
    }
    if ( theSchemaDoc ) {
        _myDocument->addSchema( * theSchemaDoc, "" );
    }

    _myDocument->appendChild( dom::NodePtr( new dom::Element("generic")) );
    _myDocument->firstChild()->appendAttribute("when", when.millis());
    if ( ! theCallback.empty()) {
        _myDocument->firstChild()->appendAttribute("callback", theCallback);
    }
}

GenericEvent::GenericEvent(const dom::NodePtr & theNode) :
    Event( GENERIC ),
    _myDocument( new dom::Document() )
{
    _myDocument->appendChild( theNode );
}

dom::NodePtr
GenericEvent::asNode() {
    if ( ! _myDocument->firstChild()->getAttribute("simulation_time") ) {
        _myDocument->firstChild()->appendAttribute("simulation_time", simulation_time);
    } else {
        _myDocument->firstChild()->getAttribute("simulation_time")->nodeValueAssign(simulation_time);
    }
    return _myDocument->firstChild();
}

dom::NodePtr
GenericEvent::getNode() const {
    return _myDocument->firstChild();
}

dom::NodePtr
GenericEvent::getDocument() const {
    return _myDocument;
}

}
