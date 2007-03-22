//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "ASSEventSource.h"

using namespace asl;
using namespace y60;

extern std::string oureventxsd;

namespace y60 {

ASSEventSource::ASSEventSource(DLHandle theHandle) :
    ASSDriver( theHandle ),
    _myEventSchema( new dom::Document( oureventxsd ) ),
    _myValueFactory( new dom::ValueFactory() )

{
    registerStandardTypes( * _myValueFactory );
    registerSomTypes( * _myValueFactory );
}


y60::EventPtrList 
ASSEventSource::poll() {
    _myEvents.clear();

    processInput();

    return _myEvents;
}


void
ASSEventSource::createEvent( Unsigned64 theID, const std::string & theType,
        const Vector2f & theRawPosition, const Vector3f & thePosition3D)
{
    y60::GenericEventPtr myEvent( new GenericEvent("onASSEvent", _myEventSchema,
            _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();
    
    myNode->appendAttribute<Unsigned64>("id", theID);
    myNode->appendAttribute<std::string>("type", theType);
    myNode->appendAttribute<Vector2f>("raw_position", theRawPosition);
    myNode->appendAttribute<Vector3f>("position3D", thePosition3D);
    _myEvents.push_back( myEvent );
}

void
ASSEventSource::createTransportLayerEvent( Unsigned64 theID,
    const std::string & theType)
{
    y60::GenericEventPtr myEvent( new GenericEvent("onASSEvent", _myEventSchema,
            _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();

    myNode->appendAttribute<Unsigned64>("id", theID );
    myNode->appendAttribute<std::string>("type", theType);
    if ( theType == "configure" ) {
        myNode->appendAttribute<Vector2i>("grid_size", _myGridSize);
    }
    _myEvents.push_back( myEvent );
}

void
ASSEventSource::onGetProperty(const std::string & thePropertyName,
        PropertyValue & theReturnValue) const
{
    if (thePropertyName == "eventSchema") {
        theReturnValue.set( _myEventSchema );
        return;
    }
    ASSDriver::onGetProperty( thePropertyName, theReturnValue );

}

void 
ASSEventSource::onSetProperty(const std::string & thePropertyName,
        const PropertyValue & thePropertyValue)
{
    if (thePropertyName == "eventSchema") {
        AC_ERROR << "eventSchema property is read only";
        return;
    }
    ASSDriver::onSetProperty( thePropertyName, thePropertyValue );
}

} // end of namespace y60


extern "C"
EXPORT asl::PlugInBase* ASSEventSource_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::ASSEventSource(myDLHandle);
}

