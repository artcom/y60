//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef Y60_ASS_DRIVER_INCLUDED
#define Y60_ASS_DRIVER_INCLUDED

#include "TransportLayer.h"
#include "Cursor.h"

#include <asl/serial/SerialDevice.h>
#include <asl/base/Enum.h>
#include <asl/math/Vector234.h>

#include <asl/dom/Nodes.h>

#include <y60/base/DataTypes.h>
#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jslib/IRendererExtension.h>
#include <y60/input/IEventSource.h>
#include <y60/scene/Scene.h>
#include <y60/input/GenericEvent.h>
#include <y60/image/ConnectedComponent.h>

#include <iostream>
#include <deque>

namespace y60 {

#define MAX_HISTORY_LENGTH 5


//#define ASS_LATENCY_TEST


class ASSDriver :
    public jslib::IScriptablePlugin,
    public y60::IRendererExtension
{
    public:
		ASSDriver();
		virtual ~ASSDriver();

        // IRendererExtension
        void onStartup(jslib::AbstractRenderWindow * theWindow) {}
        bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow);
        void handle(jslib::AbstractRenderWindow * theWindow, EventPtr theEvent) {}

        void onFrame(jslib::AbstractRenderWindow * theWindow, double theTime);
        void onPreRender(jslib::AbstractRenderWindow * theRenderer) {}
        void onPostRender(jslib::AbstractRenderWindow * theRenderer);

        // IScriptablePlugin
        virtual void onGetProperty(const std::string & thePropertyName,
                           PropertyValue & theReturnValue) const;
        virtual void onSetProperty(const std::string & thePropertyName,
                           const PropertyValue & thePropertyValue);
        virtual void onUpdateSettings(dom::NodePtr theSettings);
        JSFunctionSpec * Functions();

        void performTara();
        void callibrateTransmissionLevels();
        void queryConfigMode();
        void disconnect();
        void connect();

        void allocateGridBuffers(const asl::Vector2i & theGridSize);
        void processSensorValues(const ASSEvent & theEvent);

        virtual void createTransportLayerEvent( const std::string & theType) = 0;
    protected:
        void copyFrame(unsigned char * theData );
        void processInput();
        virtual void createEvent( int theID, const std::string & theType,
                const asl::Vector2f & theRawPosition, const asl::Vector3f & thePosition3D,
                const asl::Box2f & theROI, float intensity, const ASSEvent & theEvent) = 0;

        size_t getBytesPerFrame();

        asl::Vector2i  _myGridSize;
        asl::Vector2i  _myPoTSize;
        int            _myIDCounter;
    private:
        RasterHandle allocateRaster(const std::string & theName, int theWidth, int theHeight);
        void updateDerivedRasters();
        void updateCursors( double theDeltaT, const ASSEvent & theEvent);
        void findTouch(CursorMap::iterator & theCursorIt, double theDeltaT,
                       const ASSEvent & theEvent);
        void computeIntensity(CursorMap::iterator & theCursorIt, const y60::RasterOfGRAY & theRaster);
        float matchProximityPattern(const CursorMap::iterator & theCursorIt);
        //void computeCursorPositions( std::vector<asl::MomentResults> & theCurrentPositions,
        //                             const BlobListPtr & theROIs);

        
    template<class RESULTS> 
    void computeCursorPositions( std::vector<RESULTS> & theCurrentPositions,
                                 const BlobListPtr & theROIs)
    {
        AC_ERROR << "how could this happen - we need a specialization here";
    }

    template<class RESULTS> 
    void correlatePositions( const std::vector<RESULTS> & theCurrentPositions, 
                             const BlobListPtr theROIs, const ASSEvent & theEvent)
    {
        AC_ERROR << "how could this happen - we need a specialization here";
    }



        asl::Vector2f interpolateMaximum(const asl::Vector2f theCenter, float theMaximum);


        // void triggerUpload( const char * theRasterId );
        void queueCommand( const char * theCommand );

        asl::Vector3f applyTransform( const asl::Vector2f & theRawPosition,
                                      const asl::Matrix4f & theTransform );
        asl::Matrix4f getTransformationMatrix();

        void drawLabel(jslib::AbstractRenderWindow * theWindow, const std::string & theText);
        void drawGrid();
        void drawMarkers();
        void drawBox(const Cursor & theCursor);
        
        void drawCircle( const asl::Vector2f & thePosition, float theRadius,
                         unsigned theSubdivision, const asl::Vector4f & theColor);

        void setupDriver(dom::NodePtr theSettings);

        void captureSensorData();

        void cureBrokenElectrodes();
        void cureHLine(unsigned theLineNo);
        void cureVLine(unsigned theLineNo);
        void curePoint(unsigned theX, unsigned theY);

        RasterHandle _myRawRaster;
        RasterHandle _myDenoisedRaster;
        RasterHandle _myMomentRaster;
        RasterHandle _myResampledRaster;
        std::vector<std::string> _myRasterNames;
        
        std::vector<asl::Vector2f> _myTmpPositions;

        y60::ScenePtr _myScene;
        jslib::AbstractRenderWindow *  _myWindow;

        asl::Time   _myLastFrameTime;
        double       _myRunTime;

        int         _myComponentThreshold;
        int         _myNoiseThreshold;
        float       _myFirstDerivativeThreshold;
        float       _myGainPower;
        double      _myMinTouchInterval;
        float       _myMinTouchThreshold;
        int         _myInterpolationMethod;
        int         _myCureBrokenElectrodesFlag;
        int         _myCaputureSensorDataFlag;
        int         _myCapturedFrameCounter;

        CursorMap   _myCursors;

        struct TouchEvent {
            TouchEvent(double theBirthTime, const asl::Vector2f & thePosition) :
                    birthTime( theBirthTime ), position( thePosition ) {}
            double   birthTime;
            asl::Vector2f position;
        };
        std::vector<TouchEvent> _myTouchHistory;

        dom::NodePtr _myOverlay;

        int _myClampToScreenFlag;
        int _myDebugTouchEventsFlag;
        asl::Vector2f _myProbePosition;

        asl::Vector4f _myGridColor;
        asl::Vector4f _myCursorColor;
        asl::Vector4f _myTouchColor;
        asl::Vector4f _myTextColor;
        asl::Vector4f _myProbeColor;
        asl::Vector4f _myResampleColor;

        TransportLayerPtr _myTransportLayer;
        dom::NodePtr      _mySettings;
        
        int _myUseCCRegionForMomentumFlag;
        asl::Box2i _myUserDefinedMomentumBox;
        
        asl::Vector4f _myCureVLines;
        asl::Vector4f _myCureHLines;
        y60::VectorOfVector2f _myCurePoints;
        // XXX: shearing hack
        float _myShearX;
        float _myShearY;

#ifdef ASS_LATENCY_TEST
        void toggleLatencyTestPin();
        asl::SerialDevice * _myLatencyTestPort;
#endif
};

    template<>
    void ASSDriver::computeCursorPositions<asl::MomentResults>( std::vector<asl::MomentResults> & theCurrentPositions, 
                                                                const BlobListPtr & theROIs);
    template<>
    void ASSDriver::computeCursorPositions<MaximumResults>( std::vector<MaximumResults> & theCurrentPositions, 
                                                                const BlobListPtr & theROIs);

    template <>
    void ASSDriver::correlatePositions<asl::MomentResults>( const std::vector<asl::MomentResults> & theCurrentPositions,
                                                            const BlobListPtr theROIs, const ASSEvent & theEvent);
    template <>
    void ASSDriver::correlatePositions<MaximumResults>( const std::vector<MaximumResults> & theCurrentPositions,
                                                            const BlobListPtr theROIs, const ASSEvent & theEvent);


} // end of namespace

#endif // Y60_ASS_DRIVERS_INCLUDED
