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

#include <y60/ASSCore/ASSDriver.h>


namespace y60 {

typedef std::vector< asl::Vector3f > Position3fVector;
typedef std::vector< asl::Vector2f > Position2fVector;
//typedef std::vector< int > CursorIdVector;
typedef std::map<int, bool> CursorList;
class ASSGesture : public asl::PlugInBase,
                       public ASSDriver,
                       public y60::IEventSource
{
    public:
        ASSGesture(asl::DLHandle theHandle);
        virtual y60::EventPtrList poll();


        const char * ClassName() {
            static const char * myClassName = "ASSGesture";
            return myClassName;
        }

        virtual void onGetProperty(const std::string & thePropertyName,
                           PropertyValue & theReturnValue) const;
        virtual void onSetProperty(const std::string & thePropertyName,
                           const PropertyValue & thePropertyValue);
        virtual void onUpdateSettings(dom::NodePtr theSettings);

        void createTransportLayerEvent(const std::string & theType );
    protected:
        void createEvent( int theID, const std::string & theType,
                const asl::Vector2f & theRawPosition, const asl::Vector3f & thePosition3D,
                const asl::Box2f & theROI, float intensity, const ASSEvent & theEvent);
    private:
        dom::NodePtr addGestureEvent2Queue(int theID, const std::string & theType, const asl::Vector3f & thePosition3D);
        asl::Vector3f getAverage(int theCursorId);
        dom::NodePtr                    _myGestureSettings;
        dom::NodePtr                    _myEventSchema;
        dom::NodePtr                    _myGestureSchema;
        asl::Ptr<dom::ValueFactory>     _myValueFactory;
        y60::EventPtrList               _myEvents;
        
        std::map<int, Position3fVector> _myCursorHistory;
        std::map<int, Position3fVector> _myLastAverageList;
        std::map<int, Position3fVector> _myInitialPosition;
        
        CursorList                      _myCursorList;
        std::map<int, asl::Vector3f>    _myVectorHistory;
        std::map<int, float>            _myFirstZoomDistance;
        std::map<int, float>            _myLastZoomDistance;
        std::map<int, int>              _myZoomPartner;
        std::map<int, double>           _myAddTime;

        int                             _myDistanceThreshold;
        int                             _myWipeDistanceThreshold;
        int                             _myMaxZoomDistance;
        int                             _myMaxClickDistance;
        double                          _myMaxClickTime;
        double                          _myMinClickTime;
        
        
        
};

} // end of namespace y60

#endif // ASS_GESTURE_INCLUDED
