//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef ASS_GESTURE_INCLUDED
#define ASS_GESTURE_INCLUDED

#include <vector>
#include <map>

#include <asl/math/Vector234.h>
#include <asl/base/PlugInBase.h>
#include <asl/base/Auto.h>
#include <asl/base/ThreadLock.h>

#include <y60/input/y60_input_settings.h>
#include <y60/input/GenericEventSourceFilter.h>
#include <y60/input/IEventSink.h>
#include <y60/input/IEventSource.h>
#include <y60/input/GenericEvent.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jslib/AbstractRenderWindow.h>


namespace y60 {

enum GESTURE_BASE_EVENT_TYPE{
    ASSEVENT,
    TUIOEVENT
};


typedef std::map<int, bool> CursorList;
typedef std::map<int, int> CursorPartnerList;

class Gesture : public asl::PlugInBase,
                       public IEventSink,
                       public jslib::IScriptablePlugin,
                       public IEventSource,
                       public GenericEventSourceFilter
{
    public:
        static const float WIPE_DISTANCE_THRESHOLD;
        static const float MAX_CURSOR_PAIR_DISTANCE;
        static const float ROTATE_ANGLE_THRESHOLD;
        static const float ZOOM_DISTANCE_THRESHOLD;
        static const float TAP_MAX_DISTANCE_THRESHOLD;			 
        static const unsigned int TAP_MIN_DURATION_THRESHOLD;		
        static const unsigned int TAP_MAX_DURATION_THRESHOLD;		

        Gesture(asl::DLHandle theHandle);
        virtual y60::EventPtrList poll();
        virtual void handle(EventPtr theEvent);
        const char * ClassName() {
            static const char * myClassName = "Gesture";
            return myClassName;
        }
        virtual void onGetProperty(const std::string & thePropertyName,
                           PropertyValue & theReturnValue) const;
        virtual void onSetProperty(const std::string & thePropertyName,
                           const PropertyValue & thePropertyValue);
        virtual void onUpdateSettings(dom::NodePtr theSettings);

    protected:
        void createEvent(GESTURE_BASE_EVENT_TYPE theBaseEvent, int theID, const std::string & theType,
                          const asl::Vector3f & thePosition3D, unsigned long long & theTimestamp);
    private:
        struct PositionInfo {
            PositionInfo() {};
            PositionInfo(const asl::Vector3f & thePosition, unsigned long long & theTimestamp):
                _myPosition(thePosition), 
                _myTimestamp(theTimestamp) {}
            asl::Vector3f _myPosition;
            unsigned long long _myTimestamp;
        };
        dom::NodePtr addGestureEvent2Queue(GESTURE_BASE_EVENT_TYPE theBaseEvent, int theID, 
                                           const std::string & theType, 
                                           const asl::Vector3f & thePosition3D);

        inline asl::Vector3f getCenter(const asl::Vector3f & thePosition, const asl::Vector3f & thePartnerPosition) const {
            asl::Vector3f myCenterPoint(thePosition);
            myCenterPoint.add(thePartnerPosition);
            return product(myCenterPoint,0.5f);
        }
        
                        
        CursorPartnerList::iterator getCursorPartner(int theId);
        void saveAllCursorPositions();

        dom::NodePtr                    _myGestureSchema;
        asl::Ptr<dom::ValueFactory>     _myValueFactory;
        y60::EventPtrList               _myEvents;

        CursorList                      _myCursorList;
        std::map<int, PositionInfo>     _myCurrentCursorPositions;
        std::map<int, PositionInfo>     _myLastCursorPositions;
        std::map<int, PositionInfo>     _myInitialCursorPositions;
        std::map<int, float>            _myInitialZoomDistance;
        CursorPartnerList               _myCursorPartner;

        float                           _myWipeDistanceThreshold;
        float                           _myMaxCursorPairDistance;
        float                           _myRotateAngleThreshold;
        float                           _myZoomDistanceThreshold;
        float                           _myTapMaxDistanceThreshold;
        unsigned int                    _myTapMinDurationThreshold;
        unsigned int                    _myTapMaxDurationThreshold;

        unsigned int                    _myEventCounter;
        asl::ThreadLock                 _myDeliveryMutex;
};

} // end of namespace y60

#endif // ASS_GESTURE_INCLUDED
