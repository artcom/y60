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

#define DB(x) //x

namespace y60 {

    const float Gesture::WIPE_VELOCITY_THRESHOLD = 0.1f;     //min velocity of cursor that is needed for wipe
    const unsigned int Gesture::WIPE_MIN_HISTORY_LENGTH = 4;    //min length of history needed for wipe
    const float Gesture::MAX_CURSOR_PAIR_DISTANCE = 1000.0f;   //cursors with larger distance than this can not be pairs
    const float Gesture::ROTATE_ANGLE_THRESHOLD = 1.0f;        //fill me
    const float Gesture::ZOOM_DISTANCE_THRESHOLD = 0.0f;          //fill me
    const float Gesture::TAP_MAX_DISTANCE_THRESHOLD = 40.0f;      //max movement of cursor, larger distances can not be tap gestures
    const unsigned int Gesture::TAP_MIN_DURATION_THRESHOLD = 50;      //cursors with shorter durations will not lead to taps
    const unsigned int Gesture::TAP_MAX_DURATION_THRESHOLD = 500;      //cursors with longer durations will not lead to taps

Gesture::Gesture(DLHandle theHandle) :
    asl::PlugInBase( theHandle ),
    GenericEventSourceFilter(),
    _myGestureSchema( new dom::Document( y60::ourgestureeventxsd ) ),
    _myValueFactory( new dom::ValueFactory() ),
    _myWipeVelocityThreshold(WIPE_VELOCITY_THRESHOLD),
    _myWipeMinHistoryLength(WIPE_MIN_HISTORY_LENGTH),
    _myMaxCursorPairDistance(MAX_CURSOR_PAIR_DISTANCE),
    _myRotateAngleThreshold(ROTATE_ANGLE_THRESHOLD),
    _myZoomDistanceThreshold(ZOOM_DISTANCE_THRESHOLD),
    _myTapMaxDistanceThreshold(TAP_MAX_DISTANCE_THRESHOLD),
    _myTapMinDurationThreshold(TAP_MIN_DURATION_THRESHOLD),
    _myTapMaxDurationThreshold(TAP_MAX_DURATION_THRESHOLD),
    _myEventCounter(0)
{
    registerStandardTypes( * _myValueFactory );
    registerSomTypes( * _myValueFactory );
    EventDispatcher::get().addSink(this);
    addCursorFilter("rotate", "cursorid");
    addCursorFilter("zoom", "cursorid");
    addCursorFilter("wipe", "cursorid");
    addCursorFilter("tap", "cursorid");
}


y60::EventPtrList
Gesture::poll() {
    AutoLocker<ThreadLock> l(_myDeliveryMutex);
    // save all cursor positions (_myLastCursorPositions)
    saveAllCursorPositions();

    DB(AC_TRACE << "unfiltered gesture events # " << _myEvents.size());
    DB(analyzeEvents(_myEvents, "cursorid"));

    // do the event filter in base class GenericEventSourceFilter
    EventPtrList myFinalEvents(_myEvents);
    applyFilter(myFinalEvents);

    DB(AC_TRACE << "deliver gesture events # " << myFinalEvents.size());
    DB(analyzeEvents(myFinalEvents, "cursorid"));
    _myEvents.clear();

    DB(AC_TRACE << "-------");
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
            myTimestamp = asl::as<unsigned long long>(myNode->getAttribute("when")->nodeValue());
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
                    
                    myTimestamp = (unsigned long long)(asl::as<double>(myNode->getAttribute("value_time")->nodeValue())*1000);
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
    for (; myIt !=  _myCursorList.end();++myIt){
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
Gesture::createEvent(GESTURE_BASE_EVENT_TYPE theBaseEvent, int theID, const std::string & theType, const Vector3f & thePosition3D, unsigned long long & theTimestamp)
{
    MAKE_SCOPE_TIMER(Gesture_createEvent);
        addPositionToHistory(theID, thePosition3D, theTimestamp);
        if (theType == "add") {
            AC_DEBUG << "Gesture::createEvent -> add";

            // track cursor
            _myCursorList[ theID ] = true;
            _myCurrentCursorPositions[ theID ] = PositionInfo(thePosition3D, theTimestamp);
            _myInitialCursorPositions[ theID ] = PositionInfo(thePosition3D, theTimestamp);

        } else if(theType == "move" || theType == "update") {
            if (_myCursorList.find(theID) == _myCursorList.end()) {
                return;
            }
            AC_DEBUG << "Gesture::createEvent -> move";

            if (_myLastCursorPositions.find(theID) == _myLastCursorPositions.end()) {
                if (_myCurrentCursorPositions.find(theID) != _myCurrentCursorPositions.end()) {
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
            if (_myCursorList.size() > 1) {
                // if there is no cursor partner
                if (_myCursorPartner.find(theID) == _myCursorPartner.end()) {
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
                if ( _myWipeMinHistoryLength <= _myCursorPositionHistory[theID].size()) {
                    Vector3f myDifference = difference(thePosition3D, _myCursorPositionHistory[theID].front()._myPosition);
                    float myMagnitude = magnitude(myDifference);
                    float myDuration = static_cast<float>(_myCursorPositionHistory[theID].back()._myTimestamp - _myCursorPositionHistory[theID].front()._myTimestamp);
                    myDuration /= 1000.0f; //seconds
                    float myVelocity = myMagnitude/myDuration;
                    AC_INFO << "check for wipe with velocity " << myVelocity << " threshold " << _myWipeVelocityThreshold << " history size " << _myCursorPositionHistory[theID].size();
                    if (myVelocity > _myWipeVelocityThreshold) {
                        NodePtr myNode = addGestureEvent2Queue(theBaseEvent, theID, "wipe", thePosition3D);
                        myNode->appendAttribute<Vector3f>("direction", normalized(myDifference));
                        
                        myNode->appendAttribute<float>("velocity", myVelocity);
                        AC_INFO << "register wipe gesture, id " << theID << " direction " << normalized(myDifference) << " velocity " << myVelocity << " (duration: " << myDuration <<")";
                        //delete history to avoid additional wipes in next cursor moves
                        if (_myCursorPositionHistory.find(theID) != _myCursorPositionHistory.end()) {
                            _myCursorPositionHistory.erase(theID);
                        }
                    } 
                }
            }
        } else if (theType == "remove") {
            AC_DEBUG << "Gesture::createEvent -> remove";

            if (_myCursorList.find(theID) == _myCursorList.end()){
                return;
            }
            _myCurrentCursorPositions[ theID ] = PositionInfo(thePosition3D, theTimestamp);
            // is the removing cursor part of a existing multicursor gesture
            if (_myCursorPartner.find(theID) != _myCursorPartner.end()) {
                AC_DEBUG << "register cursor_pair_finish: " << theID << " partner: " <<  _myCursorPartner[theID];
                dom::NodePtr myNode = addGestureEvent2Queue(theBaseEvent, theID, "cursor_pair_finish", thePosition3D);

                CursorPartnerList::iterator myCursorPartnerIt = getCursorPartner(theID);
                if (myCursorPartnerIt != _myCursorPartner.end()) {
                    _myCursorPartner.erase(myCursorPartnerIt);
                }
                _myCursorPartner.erase(_myCursorPartner.find(theID));
                
            } else {
                Vector3f myDifference = difference(thePosition3D, _myInitialCursorPositions[theID]._myPosition);
                float myMagnitude = magnitude(myDifference);
                unsigned int myDuration = static_cast<unsigned int>(_myCurrentCursorPositions[theID]._myTimestamp - _myInitialCursorPositions[theID]._myTimestamp);
                AC_INFO << "check for tap with magnitude " << myMagnitude << " threshold " << _myTapMaxDistanceThreshold << " - duration: " << myDuration << ", >= threshold: " << _myTapMinDurationThreshold << " < threshold: " << _myTapMaxDurationThreshold;
                if (myMagnitude < _myTapMaxDistanceThreshold &&  
                    myDuration < _myTapMaxDurationThreshold &&
                    myDuration >= _myTapMinDurationThreshold) {
                    NodePtr myNode = addGestureEvent2Queue(theBaseEvent, theID, "tap", thePosition3D);
                    AC_INFO << "register tap gesture, id " << theID << " pos " << thePosition3D;
                }
            }
            if (_myCursorPositionHistory.find(theID) != _myCursorPositionHistory.end()) {
                _myCursorPositionHistory.erase(theID);
            }
            if (_myInitialZoomDistance.find(theID) != _myInitialZoomDistance.end()) {
                _myInitialZoomDistance.erase(theID);
            }
            if (_myInitialCursorPositions.find(theID) != _myInitialCursorPositions.end()) {
                _myInitialCursorPositions.erase(theID);
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
     _myWipeVelocityThreshold = getSetting( theSettings, "WipeVelocityThreshold", _myWipeVelocityThreshold);
     _myWipeMinHistoryLength = getSetting( theSettings, "WipeMinHistoryLength", _myWipeMinHistoryLength);
     _myMaxCursorPairDistance = getSetting( theSettings, "MaxCursorPairDistance", _myMaxCursorPairDistance);
     _myRotateAngleThreshold = getSetting( theSettings, "RotateAngleThreshold", _myRotateAngleThreshold);
     _myZoomDistanceThreshold = getSetting( theSettings, "ZoomDistanceThreshold", _myZoomDistanceThreshold);
     _myTapMaxDistanceThreshold = getSetting( theSettings, "TapMaxDistanceThreshold", _myTapMaxDistanceThreshold);
     _myTapMinDurationThreshold = getSetting( theSettings, "TapMinDurationThreshold", _myTapMinDurationThreshold);
     _myTapMaxDurationThreshold = getSetting( theSettings, "TapMaxDurationThreshold", _myTapMaxDurationThreshold);
     _myMaxCursorPositionsInHistory = getSetting( theSettings, "MaxCursorPositionsInHistory", _myMaxCursorPositionsInHistory);
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

