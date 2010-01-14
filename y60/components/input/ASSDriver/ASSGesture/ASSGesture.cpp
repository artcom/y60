//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "ASSGesture.h"
#include <y60/input/AssEventSchema.h>
#include <y60/base/SettingsParser.h>

using namespace asl;
using namespace y60;
using namespace dom;

#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.h>


namespace y60 {

const int DISTANCE_THRESHOLD = 120;
const int WIPE_DISTANCE_THRESHOLD = 100;
const int MAX_ZOOM_DISTANCE = 1000;
const int MAX_CLICK_DISTANCE = 25;
const double MAX_CLICK_TIME = 0.75;
const double MIN_CLICK_TIME =  0.01;

ASSGesture::ASSGesture(DLHandle theHandle) :
    asl::PlugInBase( theHandle ),
    ASSDriver( /*theHandle */), _myGestureSettings(0),
    _myEventSchema( new dom::Document( getASSSchema() ) ),
    _myGestureSchema( new dom::Document( getASSGestureSchema() ) ),
    _myValueFactory( new dom::ValueFactory() ),
    _myDistanceThreshold(DISTANCE_THRESHOLD),
    _myWipeDistanceThreshold(WIPE_DISTANCE_THRESHOLD),
    _myMaxZoomDistance(MAX_ZOOM_DISTANCE),
    _myMaxClickDistance(MAX_CLICK_DISTANCE),
    _myMaxClickTime(MAX_CLICK_TIME),
    _myMinClickTime(MIN_CLICK_TIME)

{
    registerStandardTypes( * _myValueFactory );
    registerSomTypes( * _myValueFactory );
}


y60::EventPtrList
ASSGesture::poll() {
    _myEvents.clear();

    processInput();

    return _myEvents;
}

dom::NodePtr
ASSGesture::addGestureEvent2Queue(int theID, const std::string & theType, const Vector3f & thePosition3D) {

    y60::GenericEventPtr myEvent( new GenericEvent("onASSGesture", _myGestureSchema, _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();
    myNode->appendAttribute<int>("id", _myEventCounter++);
    myNode->appendAttribute<int>("cursorid",theID);
    myNode->appendAttribute<std::string>("type", theType);
    myNode->appendAttribute<Vector3f>("position3D", thePosition3D);
    _myEvents.push_back( myEvent );
    return myNode;
}


void
ASSGesture::createEvent( int theID, const std::string & theType,
        const Vector2f & theRawPosition, const Vector3f & thePosition3D,
        const asl::Box2f & theROI, float theIntensity, const ASSEvent & theEvent)
{
        // the old event
        y60::GenericEventPtr myEvent( new GenericEvent("onASSEvent", _myEventSchema,
            _myValueFactory));
        dom::NodePtr myNode = myEvent->getNode();

        myNode->appendAttribute<int>("id",_myEventCounter++);
        myNode->appendAttribute<int>("cursorid",theID);
        myNode->appendAttribute<std::string>("type", theType);
        myNode->appendAttribute<Vector2f>("raw_position", theRawPosition);
        myNode->appendAttribute<Vector3f>("position3D", thePosition3D);
        myNode->appendAttribute<Box2f>("roi", theROI);
        myNode->appendAttribute<float>("intensity", theIntensity);
        myNode->appendAttribute<float>("frameno", float(theEvent.frameno));
        _myEvents.push_back( myEvent );


        if(theType == "add"){
            AC_INFO << "ASSGesture::createEvent -> add";

            _myCursorHistory[ theID ] = Position3fVector();
            _myCursorHistory[ theID ].push_back(thePosition3D);
            _myCursorList[ theID ] = true;

            _myVectorHistory[ theID ] =  Vector3f(0.0,0.0,0.0);
            _myFirstZoomDistance[ theID ] = 0.0f;
            _myLastZoomDistance[ theID ] = 0.0f;
            _myZoomPartner[ theID ] = -1;

            _myLastAverageList[ theID ] = Position3fVector();
            _myLastAverageList[ theID ].push_back(thePosition3D);

            _myInitialPosition[ theID ] = Position3fVector();
            _myInitialPosition[ theID ].push_back(thePosition3D);

            _myAddTime[ theID ] = asl::NanoTime().seconds();

            // add a first simple tap event
            dom::NodePtr myNode = addGestureEvent2Queue(theID, "tap", thePosition3D);
            myNode->appendAttribute<Vector3f>("position", thePosition3D);

        }
        //Check if theNode is valid - maybe its Cursor is not in the List
        if(_myCursorList.find(theID) == _myCursorList.end()){
                return;
        }
        if(theType == "move"){
            AC_INFO << "ASSGesture::createEvent -> move";

            _myCursorHistory[ theID ].push_back(thePosition3D);
            Vector3f myAveragePoint = getAverage(theID);

            // add a first simple move event
            NodePtr myNode = addGestureEvent2Queue(theID, "move", thePosition3D);
            myNode->appendAttribute<Vector3f>("lastPoint", _myLastAverageList[ theID ][0]);
            myNode->appendAttribute<Vector3f>("newPoint", myAveragePoint);


            //update Vector History
          _myVectorHistory[ theID ] = difference(myAveragePoint,_myLastAverageList[ theID ][0]);

            //register wipe event
            Vector3f myDiff = difference(myAveragePoint, _myInitialPosition[theID][0]);
            if( magnitude(myDiff) > _myWipeDistanceThreshold ) {
                NodePtr myNode = addGestureEvent2Queue(theID, "wipe", thePosition3D);
                myNode->appendAttribute<Vector3f>("startpoint", _myInitialPosition[theID][0]);
                myNode->appendAttribute<Vector3f>("direction", myDiff);

                _myInitialPosition[theID][0]= myAveragePoint;
            }
            //Average History update for move event
            _myLastAverageList[ theID ][0] = myAveragePoint;

            //Zooming Gesture
            if(_myCursorList.size() > 1) {
                //new pair finding
                float myCursorDistance = _myMaxZoomDistance;
                Vector3f myVector1(_myVectorHistory[ theID ]);
                //check if vector1 is too short for reliable orientation
                if(magnitude(myVector1) > 0.2){
                    CursorList::iterator theEndIt   = _myCursorList.end();
                    if(_myZoomPartner[theID] == -1 || !(_myCursorList.find(_myZoomPartner[theID]) != theEndIt)) {
                        _myZoomPartner[theID] = -1;
                        CursorList::iterator theBeginIt = _myCursorList.begin();
                        for(; theBeginIt !=  theEndIt; theBeginIt++){
                            Vector3f myVector2(_myVectorHistory[theBeginIt->first]);
                            if(!(almostEqual(myVector2[0], myVector1[0])) && magnitude(myVector2)>0.2){
                                //float myAngleBetween = acos(dot(normalized(myVector1),normalized(myVector2)))*180/PI;
                                float myAngleBetween = float(degFromRad(acos(dot(normalized(myVector1),normalized(myVector2)))));
                                float myActualDistance = distance(myAveragePoint, getAverage(theBeginIt->first));
                                if(myAngleBetween > 150.0f && myActualDistance < myCursorDistance)
                                {
                                    _myZoomPartner[theID] = theBeginIt->first;
                                    myCursorDistance = myActualDistance;
                                }
                            }
                        }
                    }
                    if(_myZoomPartner[theID] != -1){
                        Vector3f myZoomPartnerAverage = getAverage(_myZoomPartner[theID]);

                        float myDistance = distance(myAveragePoint,myZoomPartnerAverage);


                        if(_myFirstZoomDistance[theID] == 0)  {
                            _myFirstZoomDistance[theID] = myDistance;
                        }
                        if(_myLastZoomDistance[theID] == 0) {
                            _myLastZoomDistance[theID] = myDistance;
                        }

                        if(abs((myDistance - _myLastZoomDistance[theID]))>1){
                            dom::NodePtr myNode = addGestureEvent2Queue(theID, "zoom", thePosition3D);
                            myNode->appendAttribute<float>("distance", myDistance);
                            myNode->appendAttribute<float>("firstdistance", _myFirstZoomDistance[theID]);
                            myNode->appendAttribute<float>("lastdistance", _myLastZoomDistance[theID]);
                            Vector3f myAveragePoint2(myAveragePoint);
                            myAveragePoint2.add(myZoomPartnerAverage);
                            myNode->appendAttribute<Vector3f>("centerpoint", product(myAveragePoint2,0.5f));
                            _myLastZoomDistance[theID] = myDistance;
                        }
                    }
                }
            }
        }
        if(theType == "remove"){
            AC_INFO << "ASSGesture::createEvent -> remove";

            _myCursorHistory[ theID ].push_back( thePosition3D );
            Vector3f myAveragePoint = getAverage(theID);
            dom::NodePtr myNode = addGestureEvent2Queue(theID, "lift", thePosition3D);
            myNode->appendAttribute<Vector3f>("position", thePosition3D);

            if(_myCursorList.find(theID) == _myCursorList.end()){
                return;
            }

            //"Click"-Gesture
            if ( _myAddTime.find(theID) != _myAddTime.end()) {
                double myTime = asl::NanoTime().seconds() - _myAddTime[theID];
                if( distance(_myInitialPosition[theID][0], myAveragePoint) < _myMaxClickDistance &&
                    myTime < _myMaxClickTime &&
                    myTime > _myMinClickTime ) {
                        dom::NodePtr myNode = addGestureEvent2Queue(theID, "click", thePosition3D);
                        myNode->appendAttribute<Vector3f>("position", myAveragePoint);
                        AC_INFO << "ASSGesture::createEvent -> Click: " << myAveragePoint;
                }
            }

            // cleanup

            //Reset first Zooming
            _myFirstZoomDistance.erase(_myFirstZoomDistance.find(theID));
            _myLastZoomDistance.erase(_myLastZoomDistance.find(theID));
            _myZoomPartner.erase(_myZoomPartner.find(theID));

            //Remove Cursor and Cursor History on remove-event

            //_myCursorList.erase(_myCursorList.find(_myCursorList[theID ]));
            _myCursorList.erase(_myCursorList.find(theID ));
            //delete _myCursorList[_myCursorList[theID ] ];

            _myCursorHistory.erase(_myCursorHistory.find(theID));
            _myInitialPosition.erase(_myInitialPosition.find(theID));
            _myVectorHistory.erase(_myVectorHistory.find(theID));

            _myLastAverageList.erase(_myLastAverageList.find(theID));
            _myAddTime.erase(_myAddTime.find(theID));

        }
}

//Returns Weighted Average from Cursor History
Vector3f
ASSGesture::getAverage(int theCursorId) {
    Position3fVector myPositions = _myCursorHistory[theCursorId];
    Vector3f myPos(0,0,0 );

    float myDistance = 0;
    Vector3f myLastPosition(0,0,0);
    unsigned myCounter = 0;
    for (int i = myPositions.size(); i > 0 && myDistance < _myDistanceThreshold; --i)
    {
        myCounter+=i;
        myPos[0] += myPositions[i-1][0] * i;
        myPos[1] += myPositions[i-1][1] * i;
        if( i < myPositions.size()) {
            myDistance += distance( myPositions[i-1], myLastPosition );
        }
        myLastPosition = myPositions[i-1];
     }
     myPos.mult(float(1.0f)/(myCounter)) ;
     return myPos;
}

void
ASSGesture::createTransportLayerEvent( const std::string & theType)
{
    y60::GenericEventPtr myEvent( new GenericEvent("onASSEvent", _myEventSchema,
            _myValueFactory));
    dom::NodePtr myNode = myEvent->getNode();

    myNode->appendAttribute<int>("id", _myEventCounter++ );
    myNode->appendAttribute<std::string>("type", theType);
    if ( theType == "configure" ) {
        myNode->appendAttribute<Vector2i>("grid_size", _myGridSize);
    }
    _myEvents.push_back( myEvent );
}

void
ASSGesture::onUpdateSettings(dom::NodePtr theSettings) {
AC_DEBUG << "ASSGesture::onUpdateSettings";
    dom::NodePtr myAssGestureSettings =  theSettings->parentNode()->childNode("ASSGesture");
    if (myAssGestureSettings) {
        _myGestureSettings = getASSSettings( myAssGestureSettings , "ASSGesture");
    }
    if (_myGestureSettings) {
         getConfigSetting( _myGestureSettings, "DistanceThreshold", _myDistanceThreshold, DISTANCE_THRESHOLD );
         getConfigSetting( _myGestureSettings, "WipeDistanceThreshold", _myWipeDistanceThreshold, WIPE_DISTANCE_THRESHOLD );
         getConfigSetting( _myGestureSettings, "MaxZoomDistance", _myMaxZoomDistance, MAX_ZOOM_DISTANCE );
         getConfigSetting( _myGestureSettings, "MaxClickDistance", _myMaxClickDistance, MAX_CLICK_DISTANCE );
         getConfigSetting( _myGestureSettings, "MaxClickTime", _myMaxClickTime, MAX_CLICK_TIME );
         getConfigSetting( _myGestureSettings, "MinClickTime", _myMinClickTime, MIN_CLICK_TIME );
    }
    ASSDriver::onUpdateSettings(theSettings );
}

void
ASSGesture::onGetProperty(const std::string & thePropertyName,
        PropertyValue & theReturnValue) const
{
    if (thePropertyName == "eventSchema") {
        theReturnValue.set( _myEventSchema );
        return;
    }
    ASSDriver::onGetProperty( thePropertyName, theReturnValue );

}

void
ASSGesture::onSetProperty(const std::string & thePropertyName,
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
EXPORT asl::PlugInBase* ASSGesture_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::ASSGesture(myDLHandle);
}

