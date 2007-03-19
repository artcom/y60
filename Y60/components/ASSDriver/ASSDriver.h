//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "ConnectedComponent.h"

#include <asl/SerialDevice.h>
#include <asl/Enum.h>
#include <asl/Vector234.h>

#include <dom/Nodes.h>

#include <asl/PlugInBase.h>
#include <y60/IScriptablePlugin.h>
#include <y60/IRendererExtension.h>
#include <y60/IEventSource.h>
#include <y60/Scene.h>
#include <y60/GenericEvent.h>

#include <iostream>

namespace y60 {

enum DriverStateEnum {
    SYNCHRONIZING,
    RUNNING,
    DriverStateEnum_MAX
};

DEFINE_ENUM( DriverState, DriverStateEnum );

typedef dom::ValueWrapper<y60::RasterOfGRAY>::Type::ACCESS_TYPE Raster;
typedef asl::Ptr<Raster, dom::ThreadingModel> RasterPtr;

struct RasterHandle {
    RasterHandle(dom::ValuePtr theValue) :
        value( theValue ),
        raster( dynamic_cast_Ptr<Raster>( theValue ) )
    {}
    dom::ValuePtr value;
    RasterPtr raster;
};

class ASSDriver :
    public asl::PlugInBase,
    public jslib::IScriptablePlugin,
    public y60::IRendererExtension/*,
    public y60::IEventSource // XXX move to derived
    */

{
    public:
		ASSDriver (asl::DLHandle theDLHandle);
		virtual ~ASSDriver();

//   		void initClasses(JSContext * theContext, JSObject *theGlobalObject);

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
        
        /*
        virtual const char * ClassName() { // XXX will be removed
            static const char * myClassName = "ASSDriver";
            return myClassName;
        }

        // IEventSource
        virtual y60::EventPtrList poll();
*/
    protected:
        void processInput();
        virtual void createEvent( asl::Unsigned64 theID, const std::string & theType,
                const asl::Vector2f & theRawPosition, const asl::Vector3f & thePosition3D) = 0;
        virtual void createSyncEvent(asl::Unsigned64 theID) = 0;

        asl::Vector2i  _myGridSize;
    private:
        void setState( DriverState theState );
        void synchronize();
        void allocateGridBuffers();
        RasterHandle allocateRaster(const std::string & theName);
        void readSensorValues();
        void processSensorValues( double theDeltaT);
        void createThresholdedRaster(RasterHandle & theInput, RasterHandle & theOutput,
                                     const unsigned char theThreshold);
        void computeCursorPositions( const BlobListPtr & theROIs);
        void correlatePositions( const std::vector<asl::Vector2f> & thePreviousPositions );

        asl::Vector3f applyTransform( const asl::Vector2f & theRawPosition,
                                      const asl::Matrix4f & theTransform );


        std::vector<unsigned char> _myBuffer;
        asl::SerialDevice * _mySerialPort;
        DriverState    _myState;

        unsigned       _mySyncLostCounter;

        RasterHandle _myRawRaster;
        RasterHandle _myDenoisedRaster;
        y60::ScenePtr _myScene;

        asl::Time   _myLastFrameTime;
        unsigned char _myComponentThreshold;
        unsigned char _myNoiseThreshold;
        int           _myPower;

        std::vector<asl::Vector2f>   _myPositions;
        std::vector<asl::Box2f>      _myRegions;
        std::vector<asl::Unsigned64> _myCursorIDs;
        asl::Unsigned64              _myIDCounter;

        dom::NodePtr                 _myTransform;

        /*
        // XXX move to derived class
        dom::NodePtr                 _myEventSchema;
        asl::Ptr<dom::ValueFactory>  _myValueFactory;
        y60::EventPtrList            _myEvents;
        */
};

}
