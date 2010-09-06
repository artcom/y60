//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "Gesture.h"


using namespace asl;
using namespace y60;
using namespace dom;

#include <y60/base/SettingsParser.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.h>
#include <y60/base/DataTypes.h>
#include <y60/input/EventDispatcher.h>
#include <y60/input/GenericEventSourceFilter.h>

#include "gestureeventxsd.h"

namespace y60 {

    const float Gesture::WIPE_DISTANCE_THRESHOLD = 100.0;
    const float Gesture::MAX_CURSOR_PAIR_DISTANCE = 1000.0;
    const float Gesture::ROTATE_ANGLE_THRESHOLD = 1.0;
    const float Gesture::ZOOM_DISTANCE_THRESHOLD = 0.0;

Gesture::Gesture(DLHandle theHandle) :
    asl::PlugInBase( theHandle ),
    GenericEventSourceFilter(),
    _myGestureSchema( new dom::Document( y60::ourgestureeventxsd ) ),
    _myValueFactory( new dom::ValueFactory() ),
    _myWipeDistanceThreshold(WIPE_DISTANCE_THRESHOLD),
    _myMaxCursorPairDistance(MAX_CURSOR_PAIR_DISTANCE),
    _myRotateAngleThreshold(ROTATE_ANGLE_THRESHOLD),
    _myZoomDistanceThreshold(ZOOM_DISTANCE_THRESHOLD),
    _myEventCounter(0)
{
    registerStandardTypes( * _myValueFactory );
    registerSomTypes( * _myValueFactory );
    EventDispatcher::get().addSink(this);
    addCursorFilter("rotate", "cursorid");
    addCursorFilter("zoom", "cursorid");
    addCursorFilter("wipe", "cursorid");
}


y60::EventPtrList
Gesture::poll() {
    AutoLocker<ThreadLock> l(_myDeliveryMutex);
    // save all cursor positions
    saveAllCursorPositions();

    AC_TRACE << "unfiltered gesture events # " << _myEvents.size();
    //analyzeEvents(_myEvents, "cursorid");

    // do the event filter in base class GenericEventSourceFilter
    EventPtrList myFinalEvents(_myEvents);
    applyFilter(myFinalEvents);

    AC_TRACE << "deliver gesture events # " << myFinalEvents.size();
    //analyzeEvents(myFinalEvents, "cursorid");
    _myEvents.clear();

    AC_TRACE << "-------";
    return myFinalEvents;
}


void Gesture::handle(EventPtr theEvent) {
    AutoLocker<ThreadLock> l(_myDeliveryMutex);
    GenericEventPtr myGenericEventPtr;
    dom::NodePtr myNode;
    std::string myType;
    unsigned long long myTimestamp;
    switch(theEvent->type) {
        case Event::MOUSE_MOTION:
            //createEvent(1312, "add", Vector3f(1,2,3));
            break;
        case Event::GENERIC:
            myGenericEventPtr = dynamic_cast_Ptr<GenericEvent>(theEvent);
            myNode = myGenericEventPtr->getNode();
            myType = myNode->getAttributeString("type");
            myTimestamp = (asl::as<unsigned long long>(myNode->getAttribute("when")->nodeValue()));
            // cursor events from tuio or ass driver
            if (myType == "add" || myType == "remove" || myType == "move" || myType == "update") {
                GESTURE_BASE_EVENT_TYPE myBaseEventType = ASSEVENT;
                Vector3f myPosition;
                if (myNode->getAttribute("position3D")) {
                    // ass event
                    myPosition = asl::as<Vector3f>(myNode->getAttribute("position3D")->nodeValue());
                    myBaseEventType = ASSEVENT;
                } else if (myNode->getAttribute("position")) {
                    // tuio event
                    Vector2f myTuIOPosition(asl::as<Vector2f>(myNode->getAttribute("position")->nodeValue()));
                    myPosition[0] = myTuIOPosition[0];
                    myPosition[1] = myTuIOPosition[1];
                    myPosition[2] = 0.0;
                    myBaseEventType = TUIOEVENT;
                }
                createEvent(myBaseEventType, asl::as<int>(myNode->getAttribute("id")->nodeValue()),  myType, myPosition, myTimestamp);
            }
            break;
        default:
            break;
    }
}

void
Gesture::saveAllCursorPositions() {
    _myLastCursorPositions.clear();
    CursorList::iterator myIt = _myCursorList.begin();
    for(; myIt !=  _myCursorList.end();++myIt){
        _myLastCursorPositions[myIt->first] = _myCurrentCursorPositions[myIt->first];
    }
}

dom::NodePtr
Gesture::addGestureEvent2Queue(GESTURE_BASE_EVENT_TYPE theBaseEvent, int theID, const std::string & theType, const Vector3f & thePosition3D) {

    MAKE_SCOPE_TIMER(Gesture_addGestureEvent2Queue);

    y60::GenericEventPtr myEvent( new GenericEvent("onGesture", _myGestureSchema, _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();
    myNode->appendAttribute<int>("id", _myEventCounter++);
    myNode->appendAttribute<int>("baseeventtype", theBaseEvent);
    myNode->appendAttribute<int>("cursorid",theID);
    myNode->appendAttribute<std::string>("type", theType);
    myNode->appendAttribute<Vector3f>("position3D", thePosition3D);
    if (theType == "zoom" || theType == "cursor_pair_start" || theType == "cursor_pair_finish" || theType == "rotate") {
        if(_myCursorPartner.find(theID) != _myCursorPartner.end()) {
            myNode->appendAttribute<int>("cursorpartnerid",_myCursorPartner[theID]);            
        }
    }
    if (theType == "zoom" || theType == "rotate") {
        EventPtrList::iterator myIt = _myEvents.begin();
        for(; myIt !=  _myEvents.end();++myIt){
            GenericEventPtr myGenericEvent(dynamic_cast_Ptr<GenericEvent>(*myIt));
            dom::NodePtr myNode = myGenericEvent->getNode();
            std::string myEventType = myNode->getAttributeString("type");
            if (myEventType == theType && (myEventType == "zoom" || myEventType == "cursor_pair_start" || myEventType == "cursor_pair_finish" || myEventType == "rotate")) {
                int myCursorPartnerId = asl::as<int>(myNode->getAttributeString("cursorpartnerid"));
                if (myCursorPartnerId == theID) {
                    _myEvents.erase(myIt);
                    break;
                }
            }
        }
    }
    _myEvents.push_back( myEvent );
    return myNode;
}


void
Gesture::createEvent(GESTURE_BASE_EVENT_TYPE theBaseEvent,  int theID, const std::string & theType, const Vector3f & thePosition3D, unsigned long long & theTimestamp)
{
    MAKE_SCOPE_TIMER(Gesture_createEvent);
        if(theType == "add") {
            AC_DEBUG << "Gesture::createEvent -> add";

            // track cursor
            _myCursorList[ theID ] = true;
            _myCurrentCursorPositions[ theID ] = PositionInfo(thePosition3D, theTimestamp);

        } else if(theType == "move" || theType == "update") {
            if(_myCursorList.find(theID) == _myCursorList.end()) {
                return;
            }
            AC_DEBUG << "Gesture::createEvent -> move";

            if(_myLastCursorPositions.find(theID) == _myLastCursorPositions.end()) {
                if(_myCurrentCursorPositions.find(theID) != _myCurrentCursorPositions.end()) {
                    // take the positioninfo from the add event
                    _myLastCursorPositions[theID] = _myCurrentCursorPositions[ theID ];
                } else {
                    // no event at all found
                    _myLastCursorPositions[theID] = PositionInfo(thePosition3D, theTimestamp);
                }
            }

            Vector3f myLastPosition = _myLastCursorPositions[theID]._myPosition; 
            _myCurrentCursorPositions[ theID ] = PositionInfo(thePosition3D, theTimestamp);
            bool myPartnerFoundFlag = false;

            // more than one cursor, check multicursor gesture (zoom / rotate)
            if(_myCursorList.size() > 1) {
                // if there is no cursor partner
                if(_myCursorPartner.find(theID) == _myCursorPartner.end()) {
                    AC_DEBUG << theID << " has no cursor partner";
            
                    int myCursorPartnerId = -1;
                    float myCursorDistance = _myMaxCursorPairDistance;
                    CursorList::iterator myIt = _myCursorList.begin();
                    CursorList::iterator myEndIt   = _myCursorList.end();
                    for(; myIt !=  myEndIt; ++myIt) {
                        float myDistance = distance(thePosition3D, _myCurrentCursorPositions[myIt->first]._myPosition);
                        // if the cursor found a partner
                        if(_myCursorPartner.find(myIt->first) == _myCursorPartner.end() && myIt->first != theID &&  myDistance < myCursorDistance) {
                            myCursorDistance = myDistance;
                            myCursorPartnerId = myIt->first;
                            myPartnerFoundFlag = true;
                        }
                    }
                    if (myPartnerFoundFlag) {
                        AC_DEBUG << "register cursor_pair_start " << theID << " partner: " <<  myCursorPartnerId;
                        _myCursorPartner[theID] = myCursorPartnerId;
                        _myCursorPartner[myCursorPartnerId] = theID;
                        dom::NodePtr myNode = addGestureEvent2Queue(theBaseEvent, theID, "cursor_pair_start", thePosition3D);
                        asl::Vector3f myCursorPartnerPosition = _myCurrentCursorPositions[myCursorPartnerId]._myPosition;
                        asl::Vector3f myCenterPoint = getCenter(thePosition3D, myCursorPartnerPosition);
                        myNode->appendAttribute<Vector3f>("centerpoint", myCenterPoint);
                        // distance between the two partner cursors
                        float myDistance = distance(thePosition3D,myCursorPartnerPosition);
                        myNode->appendAttribute<float>("distance", myDistance);
                        _myInitialZoomDistance[theID] = myDistance;
                        _myInitialZoomDistance[myCursorPartnerId] = myDistance;
                    }
                } else {  // if there is already a cursor partner
                    AC_DEBUG << theID << " has cursor partner with id " << _myCursorPartner[theID];
                    
                    myPartnerFoundFlag = true;
                    Vector3f myCursorPartnerPosition = _myCurrentCursorPositions[_myCursorPartner[theID]]._myPosition;
                    CursorPartnerList::iterator myCursorPartnerIt = getCursorPartner(theID);
                
                    asl::Vector3f myCenterPoint = getCenter(thePosition3D, myCursorPartnerPosition);

                    // register zoom event
                    float myDistance = distance(thePosition3D,myCursorPartnerPosition);
                    float myLastDistance = distance(_myLastCursorPositions[myCursorPartnerIt->first]._myPosition, myLastPosition);

                    if (myDistance >= _myZoomDistanceThreshold) {
                        dom::NodePtr myNode = addGestureEvent2Queue(theBaseEvent, theID, "zoom", thePosition3D);
                        myNode->appendAttribute<float>("distance", myDistance);
                        myNode->appendAttribute<float>("lastdistance", myLastDistance);
                        myNode->appendAttribute<float>("initialdistance", _myInitialZoomDistance[theID]);
                        myNode->appendAttribute<float>("zoomfactor", myDistance/myLastDistance);
                        myNode->appendAttribute<Vector3f>("centerpoint", myCenterPoint);
                    }
                
                    // register rotate event
                    Vector3f myVectorOld = normalized(difference(_myLastCursorPositions[theID]._myPosition,_myLastCursorPositions[myCursorPartnerIt->first]._myPosition));
                    Vector3f myVectorNew = normalized(difference(thePosition3D, myCursorPartnerPosition));
                    float myAngle = float(degFromRad(atan2( myVectorNew[1], myVectorNew[0] ) - atan2( myVectorOld[1], myVectorOld[0] ) ));
                    if (isNaN(myAngle)) {
                        myAngle = 0;
                    }
                    if (abs(myAngle) > _myRotateAngleThreshold) {
                        dom::NodePtr myNode = addGestureEvent2Queue(theBaseEvent, theID, "rotate", thePosition3D);
                        myNode->appendAttribute<float>("angle", myAngle);
                        myNode->appendAttribute<float>("distance", myDistance);
                        myNode->appendAttribute<Vector3f>("centerpoint", myCenterPoint);
                    } 
                } 
            }     
            if (!myPartnerFoundFlag) {
                Vector3f myDifference = difference(thePosition3D, myLastPosition);
                float myMagnitude = magnitude(myDifference);
                if( myMagnitude > _myWipeDistanceThreshold ) {
                    
                    /*unsigned long long myLastTimestamp = _myLastCursorPositions[theID]._myTimestamp; 
                    unsigned long long myTimeDifference = theTimestamp - myLastTimestamp;

                    if (myTimeDifference == 0) {
                        myTimeDifference  = 1000;
                    }
                    myMagnitude /= myTimeDifference/1000.0f;*/

                    // register wipe event
                    NodePtr myNode = addGestureEvent2Queue(theBaseEvent, theID, "wipe", thePosition3D);
                    myNode->appendAttribute<Vector3f>("direction", normalized(myDifference));
                    myNode->appendAttribute<float>("magnitude", myMagnitude);
                }
            }
        } else if(theType == "remove") {
            AC_DEBUG << "Gesture::createEvent -> remove";

            if(_myCursorList.find(theID) == _myCursorList.end()){
                return;
            }
            // is the removing cursor part of a existing multicursor gesture
            if(_myCursorPartner.find(theID) != _myCursorPartner.end()) {
                AC_DEBUG << "register cursor_pair_finish: " << theID << " partner: " <<  _myCursorPartner[theID];
                dom::NodePtr myNode = addGestureEvent2Queue(theBaseEvent, theID, "cursor_pair_finish", thePosition3D);

                CursorPartnerList::iterator myCursorPartnerIt = getCursorPartner(theID);
                if (myCursorPartnerIt != _myCursorPartner.end()) {
                    _myCursorPartner.erase(myCursorPartnerIt);
                }
                _myCursorPartner.erase(_myCursorPartner.find(theID));
                
            } 
            if(_myInitialZoomDistance.find(theID) != _myInitialZoomDistance.end()) {
                _myInitialZoomDistance.erase(theID);
            }
            _myCursorList.erase(_myCursorList.find(theID ));
        }
}

CursorPartnerList::iterator 
Gesture::getCursorPartner(int theId) {
    CursorPartnerList::iterator myEndIt   = _myCursorPartner.end();
    CursorPartnerList::iterator myIt = _myCursorPartner.begin();
    for(; myIt !=  myEndIt; ++myIt){
        if (myIt->second == theId) {
            return myIt;
        }
    }
    return myEndIt;
}

void
Gesture::onUpdateSettings(dom::NodePtr theSettings) {
     _myWipeDistanceThreshold = getSetting( theSettings, "WipeDistanceThreshold", _myWipeDistanceThreshold);
     _myMaxCursorPairDistance = getSetting( theSettings, "MaxCursorPairDistance", _myMaxCursorPairDistance);
     _myRotateAngleThreshold = getSetting( theSettings, "RotateAngleThreshold", _myRotateAngleThreshold);
     _myZoomDistanceThreshold = getSetting( theSettings, "ZoomDistanceThreshold", _myZoomDistanceThreshold);
}

void
Gesture::onGetProperty(const std::string & thePropertyName,
        PropertyValue & theReturnValue) const
{
    if (thePropertyName == "eventSchema") {
        theReturnValue.set( _myGestureSchema );
        return;
    }

}

void
Gesture::onSetProperty(const std::string & thePropertyName,
        const PropertyValue & thePropertyValue)
{
    if (thePropertyName == "eventSchema") {
        AC_ERROR << "eventSchema property is read only";
        return;
    }
}

} // end of namespace y60


extern "C"
EXPORT asl::PlugInBase* Gesture_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::Gesture(myDLHandle);
}

