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
#include <y60/base/SettingsParser.h>


using namespace asl;
using namespace y60;
using namespace dom;

#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.h>
#include <y60/base/DataTypes.h>
#include <y60/input/EventDispatcher.h>

#include "gestureeventxsd.h"

namespace y60 {

const float IGNORE_CURSOR_DISTANCE            = 120.0;
const float WIPE_DISTANCE_THRESHOLD           = 100.0;
const float MAX_CURSOR_PAIR_DISTANCE          = 1000.0;
const unsigned int MAX_POSITIONS_FROM_HISTORY = 10;


Gesture::Gesture(DLHandle theHandle) :
    asl::PlugInBase( theHandle ),
     _myGestureSettings(0),
     _myGestureSchema( new dom::Document( y60::ourgestureeventxsd ) ),
    _myValueFactory( new dom::ValueFactory() ),
    _myIgnoreCursorsInHistoryDistance(IGNORE_CURSOR_DISTANCE),
    _myMaxPositionsFromHistory(MAX_POSITIONS_FROM_HISTORY),
    _myWipeDistanceThreshold(WIPE_DISTANCE_THRESHOLD),
    _myMaxCursorPairDistance(MAX_CURSOR_PAIR_DISTANCE),
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

    AC_DEBUG << "unfiltered gesture events # " << _myEvents.size();
    //analyzeEvents(_myEvents, "cursorid");

    // do the event filter in base class GenericEventSourceFilter
    EventPtrList myFinalEvents(_myEvents);
    applyFilter(myFinalEvents);

    AC_DEBUG << "deliver gesture events # " << myFinalEvents.size();
    //analyzeEvents(myFinalEvents, "cursorid");
    _myEvents.clear();

    //AC_PRINT << "-------";
    return myFinalEvents;
}


void Gesture::handle(EventPtr theEvent) {
    AutoLocker<ThreadLock> l(_myDeliveryMutex);
    GenericEventPtr myGenericEventPtr;
    dom::NodePtr myNode;
    std::string myType;
    switch(theEvent->type) {
        case Event::MOUSE_MOTION:
            //createEvent(1312, "add", Vector3f(1,2,3));
            break;
        case Event::GENERIC:
            myGenericEventPtr = dynamic_cast_Ptr<GenericEvent>(theEvent);
            myNode = myGenericEventPtr->getNode();
            myType = myNode->getAttributeString("type");
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
                createEvent(myBaseEventType, asl::as<int>(myNode->getAttribute("id")->nodeValue()),  myType, myPosition);
            }
            break;
        default:
            AC_INFO << "event type " << theEvent->type << " not handled";
            break;
    }
}

void
Gesture::saveAllCursorPositions() {
    _myLastCursorPositions.clear();
    CursorList::iterator myIt = _myCursorList.begin();
    for(; myIt !=  _myCursorList.end();myIt++){
        _myLastCursorPositions[myIt->first] = _myCursorPositionHistory[myIt->first][_myCursorPositionHistory[myIt->first].size()-1];
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
        for(; myIt !=  _myEvents.end();myIt++){
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
Gesture::createEvent(GESTURE_BASE_EVENT_TYPE theBaseEvent,  int theID, const std::string & theType, const Vector3f & thePosition3D)
{
    MAKE_SCOPE_TIMER(Gesture_createEvent);
        if(theType == "add"){
            AC_DEBUG << "Gesture::createEvent -> add";

            // track cursor
            _myCursorList[ theID ] = true;

            // track first cursor position
            _myCursorPositionHistory[ theID ] = Position3fVector();
            _myCursorPositionHistory[ theID ].push_back(thePosition3D);

            // reset values
            _myInitialZoomDistance[ theID ] = 0.0f;
        }

        // check if theNode is valid - maybe its Cursor is not in the List
        if(_myCursorList.find(theID) == _myCursorList.end()){
                return;
        }

        if(theType == "move" || theType == "update"){
            AC_DEBUG << "Gesture::createEvent -> move";

            Vector3f myLastPoint = getCurrentPosition(theID); 

            if (_myCursorPositionHistory.size() >= _myMaxPositionsFromHistory) {
                _myCursorPositionHistory[ theID ].erase(_myCursorPositionHistory[ theID ].begin());
            }
            _myCursorPositionHistory[ theID ].push_back(thePosition3D);
            Vector3f myPos = getCurrentPosition(theID);

            //save vector betwenn last and current point
            bool myPartnerFoundFlag = false;

            // more than one cursor, check multicursor gesture (zoom / rotate)
            if(_myCursorList.size() > 1) {

                //new pair finding
                float myCursorDistance = _myMaxCursorPairDistance;
                
                
                CursorList::iterator myEndIt   = _myCursorList.end();

                // if there is no cursor partner
                if(_myCursorPartner.find(theID) == _myCursorPartner.end()) {

                    AC_DEBUG << theID << " has no cursor partner";
                    CursorList::iterator myIt = _myCursorList.begin();
                    for(; myIt !=  myEndIt; ++myIt){
                        float myDistance = distance(myPos, getCurrentPosition(myIt->first));
                        if(_myCursorPartner.find(myIt->first) == _myCursorPartner.end() && myIt->first != theID &&  myDistance < myCursorDistance) {
                            _myCursorPartner[theID] = myIt->first;
                            _myCursorPartner[myIt->first] = theID;
                            myCursorDistance = myDistance;
                            AC_DEBUG << "register cursor_pair_start " << theID << " partner: " <<  myIt->first;
                            dom::NodePtr myNode = addGestureEvent2Queue(theBaseEvent, theID, "cursor_pair_start", thePosition3D);
                            Vector3f myCursorPartnerPos = getCurrentPosition(_myCursorPartner[theID]);
                            Vector3f myCenterPoint(myPos);
                            myCenterPoint.add(myCursorPartnerPos);
                            myCenterPoint = product(myCenterPoint,0.5f);
                            myNode->appendAttribute<Vector3f>("centerpoint", myCenterPoint);
                            // if the cursor found a partner
                            myPartnerFoundFlag = true;
                            break;
                        }
                    }
                }

                // if there is already a cursor partner
                else if(_myCursorPartner.find(theID) != _myCursorPartner.end()) {

                    AC_DEBUG << theID << " has cursor partner with id " << _myCursorPartner[theID];
                    Vector3f myCursorPartnerPos = getCurrentPosition(_myCursorPartner[theID]);

                    // distance between the two partner cursors
                    float myDistance = distance(myPos,myCursorPartnerPos);

                    if(_myInitialZoomDistance[theID] == 0)  {
                        _myInitialZoomDistance[theID] = myDistance;
                    }

                    Vector3f myCenterPoint(myPos);
                    myCenterPoint.add(myCursorPartnerPos);
                    myCenterPoint = product(myCenterPoint,0.5f);

                    CursorPartnerList::iterator myCursorPartnerIt = getCursorPartner(theID);
                    
                    // register zoom event
                    float myLastDistance = distance(_myLastCursorPositions[myCursorPartnerIt->first], myLastPoint);
                    dom::NodePtr myNode = addGestureEvent2Queue(theBaseEvent, theID, "zoom", thePosition3D);
                    myNode->appendAttribute<float>("distance", myDistance);
                    myNode->appendAttribute<float>("lastdistance", myLastDistance);
                    myNode->appendAttribute<float>("initialdistance", _myInitialZoomDistance[theID]);
                    myNode->appendAttribute<Vector3f>("centerpoint", myCenterPoint);
                
                    if (myCursorPartnerIt != _myCursorPartner.end()) {
                        _myInitialZoomDistance[myCursorPartnerIt->first] = _myInitialZoomDistance[theID];
                    }
                    myPartnerFoundFlag = true;

                    Vector3f myVectorOld = normalized(difference(_myLastCursorPositions[theID],_myLastCursorPositions[myCursorPartnerIt->first]));
                    Vector3f myVectorNew = normalized(difference(myPos, myCursorPartnerPos));
                    float myAngle = float(degFromRad(atan2( myVectorNew[1], myVectorNew[0] ) - atan2( myVectorOld[1], myVectorOld[0] ) ));
                    if (isNaN(myAngle)) {
                        myAngle = 0;
                    }
                    AC_PRINT << "angle: " << myAngle;
                    if (abs(myAngle) > 1) {
                        dom::NodePtr myNode2 = addGestureEvent2Queue(theBaseEvent, theID, "rotate", thePosition3D);
                        myNode2->appendAttribute<float>("angle", myAngle);
                        myNode2->appendAttribute<Vector3f>("centerpoint", myCenterPoint);
                    } 
                } 
            }     
            if (!myPartnerFoundFlag) {
                Vector3f myDiff = difference(myPos, myLastPoint);
                if( magnitude(myDiff) > _myWipeDistanceThreshold ) {
                    // register wipe event
                    NodePtr myNode = addGestureEvent2Queue(theBaseEvent, theID, "wipe", thePosition3D);
                    myNode->appendAttribute<Vector3f>("direction", myDiff);
                }
            }
        }
        if(theType == "remove"){
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
                    _myInitialZoomDistance[myCursorPartnerIt->first] = 0;
                    _myCursorPartner.erase(myCursorPartnerIt);
                }
                _myCursorPartner.erase(_myCursorPartner.find(theID));
            } 
            
            // cleanup

            //Reset first Zooming
            _myInitialZoomDistance.erase(_myInitialZoomDistance.find(theID));

            //Remove Cursor and Cursor History on remove-event
            _myCursorList.erase(_myCursorList.find(theID ));
            _myCursorPositionHistory.erase(_myCursorPositionHistory.find(theID));
            
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
     _myIgnoreCursorsInHistoryDistance = getSetting( theSettings, "IgnoreCursorsInHistoryDistance", _myIgnoreCursorsInHistoryDistance);
     _myWipeDistanceThreshold = getSetting( theSettings, "WipeDistanceThreshold", _myWipeDistanceThreshold);
     _myMaxCursorPairDistance = getSetting( theSettings, "MaxCursorPairDistance", _myMaxCursorPairDistance);
     _myMaxPositionsFromHistory = getSetting( theSettings, "MaxPositionsFromHistory", _myMaxPositionsFromHistory);
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

