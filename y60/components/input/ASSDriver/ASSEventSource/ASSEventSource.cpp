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

#include "ASSEventSource.h"
#include <y60/input/asseventxsd.h>

using namespace asl;
using namespace y60;

#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.h>
#include <y60/base/SettingsParser.h>


namespace y60 {

ASSEventSource::ASSEventSource(DLHandle theHandle) :
    asl::PlugInBase( theHandle ),
    ASSDriver( /*theHandle */),
    GenericEventSourceFilter(),
    _myEventSchema( new dom::Document( y60::ourasseventxsd ) ),
    _myValueFactory( new dom::ValueFactory() )

{
    registerStandardTypes( * _myValueFactory );
    registerSomTypes( * _myValueFactory );

    // add filter for deleting multiple update
    if (_myFilterMultipleMovePerCursorFlag) {
        addCursorFilter("move", "id");
    }
}


y60::EventPtrList
ASSEventSource::poll() {
    _myEvents.clear();

    processInput();

    // do the event filter in base class GenericEventSourceFilter
	//analyzeEvents(_myEvents, "id");
    applyFilter(_myEvents);
    clearCursorHistoryOnRemove(_myEvents);
    return _myEvents;
}


void
ASSEventSource::createEvent( int theID, const std::string & theType,
        const Vector2f & theRawPosition, const Vector3f & thePosition3D,
        const asl::Box2f & theROI, float theIntensity, const ASSEvent & theEvent)
{
    y60::GenericEventPtr myEvent( new GenericEvent("onASSEvent", _myEventSchema,
            _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();

    asl::Vector3f myPosition = calculateAveragePosition(theID, thePosition3D);
    myNode->appendAttribute<int>("id", theID);
    myNode->appendAttribute<std::string>("type", theType);
    myNode->appendAttribute<Vector2f>("raw_position", theRawPosition);
    myNode->appendAttribute<Vector3f>("position3D", myPosition);
    myNode->appendAttribute<Box2f>("roi", theROI);
    myNode->appendAttribute<float>("intensity", theIntensity);
    myNode->appendAttribute<float>("frameno", static_cast<float>(theEvent.frameno));
    _myEvents.push_back( myEvent );
}

void
ASSEventSource::createTransportLayerEvent( const std::string & theType)
{
    y60::GenericEventPtr myEvent( new GenericEvent("onASSEvent", _myEventSchema,
            _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();

    myNode->appendAttribute<int>("id", _myIDCounter++ );
    myNode->appendAttribute<std::string>("type", theType);
    if ( theType == "configure" ) {
        myNode->appendAttribute<Vector2i>("grid_size", _myGridSize);
    }
    _myEvents.push_back( myEvent );
}

void
ASSEventSource::onUpdateSettings(dom::NodePtr theSettings) {
    ASSDriver::onUpdateSettings(theSettings);
    AC_DEBUG << "updating ASSEventSource settings";

    dom::NodePtr mySettings = getASSSettings( theSettings );
    getConfigSetting( mySettings, "MaxCursorPositionsForAverage", _myMaxCursorPositionsInHistory, static_cast<unsigned int> (10) );
    
    int myFilterFlag = 0;
    getConfigSetting( mySettings, "FilterMultipleMovePerCursor", myFilterFlag, 1);
    _myFilterMultipleMovePerCursorFlag = (myFilterFlag == 1 ? true : false);
        
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

