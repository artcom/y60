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
#include <deque>

namespace y60 {

enum DriverStateEnum {
    NO_SERIAL_PORT,
    SYNCHRONIZING,
    RUNNING,
    DriverStateEnum_MAX
};

DEFINE_ENUM( DriverState, DriverStateEnum );

DEFINE_EXCEPTION( ASSException, asl::Exception );

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

#define MAX_HISTORY_LENGTH 5

struct Cursor {
    Cursor(const asl::Vector2f & thePos, const asl::Box2f & theBox) :
            position( thePos ), 
            roi( theBox),
            previousSum(0.0),
            firstDerivative(0.0),
            lastTouchTime(0.0),
            intensity(0.0)
    {
        previousRoi.makeEmpty();
    }

    float getMinIntensity() {
        if ( ! intensityHistory.empty()) {
            float myMin = INT_MAX;
            for (unsigned i = 0; i < intensityHistory.size(); ++i) {
                myMin = asl::minimum(myMin, intensityHistory[i]);
            }
            return myMin;
        }
        return intensity;

    }

    float getAverageFirstDerivative() const {
        if ( ! firstDerivativeHistory.empty()) {
            float mySum = 0.0;
            for (unsigned i = 0; i < firstDerivativeHistory.size(); ++i) {
                mySum += fabs(firstDerivativeHistory[i]) / (firstDerivativeHistory.size() - i);
            }
            return mySum / firstDerivativeHistory.size();
        }
        return 0.0;
    }
 
    float getAverageMaxGradient() const {
        return getAverageFromHistory(maxGradientHistory);
    }
    
    float getAverageSecondDerivative() const {
        return getAverageFromHistory(secondDerivativeHistory);
    }
    
    float getAverageFromHistory(const std::deque<float> & theHistory) const {
        if ( ! theHistory.empty()) {
            float mySum = 0.0;
            for (unsigned i = 0; i < theHistory.size(); ++i) {
                mySum += fabs(theHistory[i]);// / (theHistory.size() - i);
            }
            return mySum / theHistory.size();
        }
        return 0.0;

    }
    
    float getAbsoluteDerivativeSum() {
        float mySum = 0.0;
        for (unsigned i = 0; i < firstDerivativeHistory.size(); ++i) {
            mySum += fabs(firstDerivativeHistory[i]);// / (theHistory.size() - i);
        }
        return mySum;
    }
    
    float getDerivativeSum() {
        float mySum = 0.0;
        for (unsigned i = 1; i < firstDerivativeHistory.size(); ++i) {
            mySum += (firstDerivativeHistory[i]<firstDerivativeHistory[i-1])? -1:1;// / (theHistory.size() - i);
        }
        return mySum;
    }

    asl::Vector2f position;
    asl::Box2f    roi;
    asl::Box2f    previousRoi;
    float         intensity;
    float         previousIntensity;
    float         firstDerivative;
    double        lastTouchTime;


    std::deque<float> maxGradientHistory;
    std::deque<float> firstDerivativeHistory;
    std::deque<float> secondDerivativeHistory;
    std::deque<float> intensityHistory;


    float previousSum;
};

typedef std::map<int, Cursor> CursorMap;

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
        
    protected:
        void processInput();
        virtual void createEvent( int theID, const std::string & theType,
                const asl::Vector2f & theRawPosition, const asl::Vector3f & thePosition3D,
                const asl::Box2f & theROI) = 0;
        virtual void createTransportLayerEvent(int theID,
                                               const std::string & theType) = 0;

        size_t getBytesPerFrame();

        asl::Vector2i  _myGridSize;
        asl::Vector2i  _myPoTSize;
    private:
        void setState( DriverState theState );
        void synchronize();
        void allocateGridBuffers();
        RasterHandle allocateRaster(const std::string & theName);
        void readSensorValues();
        void processSensorValues( double theDeltaT);
        void updateDerivedRasters();
        void updateCursors( double theDeltaT);
        void findTouch(CursorMap::iterator & theCursorIt, double theDeltaT);
        void computeIntensity(CursorMap::iterator & theCursorIt, const y60::RasterOfGRAY & theRaster);
        float matchProximityPattern(const CursorMap::iterator & theCursorIt);
        void computeCursorPositions( std::vector<asl::Vector2f> & theCurrentPositions,
                                     const BlobListPtr & theROIs);
        void correlatePositions( const std::vector<asl::Vector2f> & thePreviousPositions, 
                                 const BlobListPtr theROIs);

        asl::Vector3f applyTransform( const asl::Vector2f & theRawPosition,
                                      const asl::Matrix4f & theTransform );
        asl::Matrix4f getTransormationMatrix();

        DriverState    _myState;

        unsigned       _mySyncLostCounter;

        RasterHandle _myRawRaster;
        RasterHandle _myDenoisedRaster;
        RasterHandle _myGradientRaster; // XXX unused
        RasterHandle _myMomentRaster;
        std::vector<std::string> _myRasterNames;

        y60::ScenePtr _myScene;

        asl::Time   _myLastFrameTime;
        float       _myRunTime;
        int         _myComponentThreshold;
        int         _myNoiseThreshold;
        float       _myTouchThreshold;
        float       _myGainPower;
        double      _myMinTouchInterval;

        CursorMap   _myCursors;
        int         _myIDCounter;

        dom::NodePtr _myTransform;

        float _myTweakVal; // XXX

        // Transport Layer Members:
        // will be refactored into a separate class, when 
        // we go for ethernet
        void readDataFromPort();
        void scanForSerialPort();
        void freeSerialPort();

        std::vector<unsigned char> _myFrameBuffer;
        asl::SerialDevice * _mySerialPort;

        int  _myLineStart;
        int  _myLineEnd;
        int  _myMaxLine;
        bool _myMagicTokenFlag;

        bool _myUseUSBFlag; // used by the linux implementation, because
                            // USB TTYs have a diffrent naming scheme
        int  _myPortNum;
        int  _myBaudRate;
        int  _myBitsPerSerialWord;
        int  _myStopBits;
        bool _myHandshakingFlag;
        asl::SerialDevice::ParityMode _myParity;
        std::vector<unsigned char> _myReceiveBuffer;
};


//=== Configuration Helpers ========================

dom::NodePtr getASSSettings(dom::NodePtr theSettings);

template <class T>
bool
getConfigSetting(dom::NodePtr theSettings, const std::string & theName, T & theValue,
                 const T & theDefault)
{
    dom::NodePtr myNode = theSettings->childNode( theName );
    if ( ! myNode ) {
        AC_WARNING << "No node named '" << theName << "' found in configuration. "
                   << "Adding default value '" << theDefault << "'";
        myNode = theSettings->appendChild( dom::NodePtr( new dom::Element( theName )));
        dom::NodePtr myTextNode = myNode->appendChild( dom::NodePtr( new dom::Text() ));
        myTextNode->nodeValue( asl::as_string( theDefault ));
    }

    if ( myNode->childNodesLength() != 1 ) {
        throw asl::Exception(std::string("Configuration node '") + theName +
            "' must have exactly one child.", PLUS_FILE_LINE);
    }
    if ( myNode->childNode("#text") ) {
        T myNewValue = asl::as<T>( myNode->childNode("#text")->nodeValue() );
        if (myNewValue != theValue) {
            theValue =  myNewValue;
            return true;
        } else {
            return false;
        }
    } else {
        throw asl::Exception(std::string("Node '") + myNode->nodeName() + 
                "' does not have a text child." , PLUS_FILE_LINE);
    }
}

template <class Enum>
bool
getConfigSetting(dom::NodePtr theSettings, const std::string & theName, Enum & theValue,
                 typename Enum::Native theDefault)
{
    return getConfigSetting( theSettings, theName, theValue, Enum( theDefault ));
}

}
