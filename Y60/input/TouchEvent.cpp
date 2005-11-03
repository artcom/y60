// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "TouchEvent.h"

#include <asl/Vector234.h>
#include <dom/Nodes.h>

namespace y60 {

    TouchEvent::TouchEvent(const std::string & theDevice,
            const asl::Vector2f & thePosition,
            const asl::Vector2i & theSize,
            float theIntensity) :
        Event(Event::TOUCH),
        device(theDevice),
        position(thePosition),
        size(theSize),
        intensity(theIntensity)
    {
    }

    TouchEvent::TouchEvent(const dom::NodePtr & theNode) :
        Event(TOUCH, theNode),
        device(theNode->getAttributeValue<std::string>("device")),
        position(theNode->getAttributeValue<asl::Vector2f>("position")),
        size(theNode->getAttributeValue<asl::Vector2i>("size")),
        intensity(theNode->getAttributeValue<float>("intensity"))
    {
    }

    dom::NodePtr TouchEvent::asNode() const {
        dom::NodePtr myNode = Event::asNode();
        myNode->appendAttribute("device", device);
        myNode->appendAttribute("position", position);
        myNode->appendAttribute("size", size);
        myNode->appendAttribute("intensity", intensity);
        return myNode;
    }
}
