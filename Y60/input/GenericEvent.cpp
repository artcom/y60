//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "GenericEvent.h"

namespace y60 {

GenericEvent::GenericEvent(const std::string & theCallback) :
    Event( GENERIC ),
    _myNode( new dom::Element("generic") )
{
    _myNode->appendAttribute("when", double( when ));
    if ( ! theCallback.empty()) {
        _myNode->appendAttribute("callback", theCallback);
    }
}

GenericEvent::GenericEvent(const dom::NodePtr & theNode) :
    Event( GENERIC ),
    _myNode( theNode )
{
    _myNode->appendAttribute("when", double( when ));
}

dom::NodePtr
GenericEvent::asNode() const {
    return _myNode;
}

dom::NodePtr 
GenericEvent::getNode() const {
    return _myNode;
}

}
