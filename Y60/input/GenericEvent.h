//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_INPUT_GENERIC_EVENT_INCLUDED_
#define _Y60_INPUT_GENERIC_EVENT_INCLUDED_

#include "Event.h"

#include <dom/Nodes.h>

namespace y60 {

class GenericEvent : public Event {
    public:
        GenericEvent(const std::string & theCallback = std::string(""));
        GenericEvent(const dom::NodePtr & theNode);

        virtual EventPtr copy() const {
            return EventPtr( new GenericEvent( * this ) );
        }

        dom::NodePtr getNode() const;
        virtual dom::NodePtr asNode() const;
    private:
        dom::NodePtr _myNode;
};

typedef asl::Ptr<GenericEvent> GenericEventPtr;

}
#endif // _Y60_INPUT_GENERIC_EVENT_INCLUDED_


