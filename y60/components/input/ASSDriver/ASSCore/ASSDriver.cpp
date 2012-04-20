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

#include "ASSDriver.h"
#include "SerialTransport.h"
#include "SocketTransport.h"
#include "DummyTransport.h"
#include "ASSUtils.h"

#include <asl/base/Logger.h>
#include <asl/base/Assure.h>
#include <asl/math/numeric_functions.h>

#ifdef ASS_LATENCY_TEST
#   include <asl/SerialDevice.h>
#   include <asl/SerialDeviceFactory.h>
#endif

#include <y60/base/SettingsParser.h>
#include <y60/image/PixelEncoding.h>
#include <y60/scene/ImageBuilder.h>
#include <y60/jslib/AbstractRenderWindow.h>
#include <y60/jsbase/JSScriptablePlugin.h>
#include <y60/scene/Overlay.h>

#include <iostream>
#include <algorithm>
#include <map>
#include <ctype.h>

#ifdef LINUX
#   include <values.h>
#else
#   include <float.h>
#endif


using namespace std;
using namespace asl;
using namespace y60;
using namespace jslib;

namespace y60 {


    const char * CMD_ENTER_CONFIG_MODE("x");
    const char * CMD_QUERY_CONFIG_MODE("c98");
    const char * CMD_LEAVE_CONFIG_MODE("c99");

    const char * CMD_CALLIBRATE_TRANSMISSION_LEVEL( "c01");
    const char * CMD_PERFORM_TARA( "c02" );

    static const char * RAW_RASTER = "ASSRawRaster";
    static const char * FILTERED_RASTER = "ASSFilteredRaster";
    static const char * MOMENT_RASTER = "ASSMomentRaster";

    const unsigned int BEST_VERSION( 260 );

    bool RESAMPLING = false;
    const unsigned int GRID_SCALE_X = 2;
    const unsigned int GRID_SCALE_Y = 2;


    const float TOUCH_MARKER_LIFE_TIME( 1.0 );

#ifdef OSX
#undef verify
#endif

    Box2f
    asBox2f( const Box2i & theBox ) {
        return Box2f( Vector2f( float(theBox[Box2i::MIN][0]), float(theBox[Box2i::MIN][1])),
                      Vector2f( float(theBox[Box2i::MAX][0]), float(theBox[Box2i::MAX][1])));
    }

    ASSDriver::ASSDriver() :
        IRendererExtension("ASSDriver"),
        _myGridSize(0,0),
        _myPoTSize(1,1),
        _myIDCounter(0),
        _myRawRaster(dom::ValuePtr()),
        _myDenoisedRaster(dom::ValuePtr()),
        _myMomentRaster(dom::ValuePtr()),
        _myResampledRaster(dom::ValuePtr()),
        _myScene(),
        _myWindow( 0 ),
        _myLastFrameTime( asl::Time() ),
        _myRunTime(0.0),
        _myComponentThreshold( 5 ),
        _myNoiseThreshold( 15 ),
        _myFirstDerivativeThreshold( 200 ),
        _myGainPower(2.0f),
        _myMinTouchInterval( 0.25 ),
        _myMinTouchThreshold( 9 ),
        _myInterpolationMethod(0),
        _myCureBrokenElectrodesFlag(0),
        _myCaputureSensorDataFlag(0),
        _myCapturedFrameCounter(0),
        _myClampToScreenFlag(false),
        _myDebugTouchEventsFlag( 0 ),
        _myProbePosition( -1, -1),
        _myGridColor(0.5, 0.5, 0, 1.0),
        _myCursorColor(0.5, 0.5, 0, 1.0),
        _myTouchColor(1.0, 1.0, 1.0, 1.0),
        _myTextColor(1.0, 1.0, 1.0, 1.0),
        _myProbeColor(0.0, 0.75, 0.0, 1.0),
        _myResampleColor(1.0, 0.0, 0.0, 1.0),
        _mySettings(dom::NodePtr()),
        _myUseCCRegionForMomentumFlag(0),
        _myUserDefinedMomentumBox(-1, -1, 1, 1),
        _myCureVLines(-1,-1,-1,-1),
        _myCureHLines(-1,-1,-1,-1),
        _myCurePoints(0),
        _myShearX(0.0),
        _myShearY(0.0),
        _myTransformEventOrientation(0.0f),
        _myTransformEventScale(1.0f,1.0f),
        _myTransformEventPosition(0.0f,0.0f)
    {
        AC_DEBUG << "ASSDriver::ASSDriver()";
#ifdef ASS_LATENCY_TEST
        _myLatencyTestPort = asl::getSerialDevice( 0 );
        _myLatencyTestPort->open( 9600, 8, SerialDevice::NO_PARITY, 1, false);
#endif
    }

    ASSDriver::~ASSDriver() {
        AC_DEBUG << "ASSDriver::~ASSDriver()";
#ifdef ASS_LATENCY_TEST
        if ( _myLatencyTestPort ) {
            delete _myLatencyTestPort;
            _myLatencyTestPort = 0;
        }
#endif
    }

    bool
    ASSDriver::onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
        //AC_PRINT << "ASSDriver::onSceneLoaded()";
        _myWindow = theWindow;
        _myScene = theWindow->getCurrentScene();
        // TODO: trigger raster reallocation
        //setState(NOT_CONNECTED);
        return true;
    }

    void
    ASSDriver::onFrame(jslib::AbstractRenderWindow * theWindow, double theTime) {
    }

    void
    ASSDriver::allocateGridBuffers(const asl::Vector2i & theGridSize ) {
        _myGridSize = theGridSize;

        _myPoTSize[0] = nextPowerOfTwo( _myGridSize[0] );
        _myPoTSize[1] = nextPowerOfTwo( _myGridSize[1] );

        _myRasterNames.clear();
        _myRawRaster = allocateRaster(RAW_RASTER, _myPoTSize[0], _myPoTSize[1]);
        _myDenoisedRaster = allocateRaster(FILTERED_RASTER, _myPoTSize[0], _myPoTSize[1]);
        _myMomentRaster = allocateRaster(MOMENT_RASTER, _myPoTSize[0], _myPoTSize[1]);

        std::fill(_myRawRaster.raster->pixels().begin(), _myRawRaster.raster->pixels().end(), 0);
        std::fill(_myDenoisedRaster.raster->pixels().begin(), _myDenoisedRaster.raster->pixels().end(), 0);
        std::fill(_myMomentRaster.raster->pixels().begin(), _myMomentRaster.raster->pixels().end(), 0);

        // XXX: resampling
        unsigned myWidth = nextPowerOfTwo(_myGridSize[0] * GRID_SCALE_X );
        unsigned myHeight = nextPowerOfTwo( _myGridSize[1] * GRID_SCALE_Y );
        _myResampledRaster = allocateRaster("resampled_raster", myWidth, myHeight);

        createTransportLayerEvent( "configure" );
    }

    RasterHandle
    ASSDriver::allocateRaster(const std::string & theName, int theWidth, int theHeight) {
        if (_myScene) {
            dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(theName);
            if (myImage) {
                myImage->getFacade<y60::Image>()->createRaster(
                    theWidth, theHeight, 1, y60::GRAY);
                _myRasterNames.push_back( theName );
                return RasterHandle( myImage->childNode(0)->childNode(0)->nodeValueWrapperPtr());
            } else {
                ImageBuilder myImageBuilder(theName);
                _myScene->getSceneBuilder()->appendImage(myImageBuilder);
                y60::ImagePtr myImage = myImageBuilder.getNode()->getFacade<y60::Image>();
                myImage->set<y60::IdTag>( theName );
                myImage->set<y60::NameTag>( theName );
                myImage->createRaster( theWidth, theHeight, 1, y60::GRAY);

                _myRasterNames.push_back( theName );

                return RasterHandle( myImage->getNode().childNode(0)->childNode(0)->nodeValueWrapperPtr());
            }
        } else {
            AC_WARNING << "No scene. Allocating loose rasters.";
            RasterHandle myHandle(createRasterValue(y60::GRAY, theWidth, theHeight));
            myHandle.raster->clear();
            return myHandle;

        }
    }

    template <class PixelT>
    struct Threshold {
        Threshold(const PixelT & theThreshold) :
            _myThreshold( theThreshold ) {}
        PixelT operator () (const PixelT & a) {
            if (a.get() < _myThreshold.get()) {
                return PixelT(0);
            } else {
                return PixelT(a.get() - _myThreshold.get());
            }
        }

        PixelT _myThreshold;
    };

    template <class PixelT>
    struct Power {
        Power(float thePower) :
            _myGainPower( thePower) {}
        PixelT operator () (const PixelT & a) {
            return PixelT( (unsigned char)(pow(float(a.get()), _myGainPower) /  pow(255.0f, _myGainPower - 1)));
        }

        float _myGainPower;
    };

    template <class RASTER>
    struct AnalyseMoment {
        void operator() (const RASTER & theRaster) {
            analyseMoments( theRaster, result );
        }
        MomentResults result;
    };

    template <class PixelT>
    struct AnalyseProximity {
        AnalyseProximity() :
            proximity( 0.0 ),
            max( 0 ) {}
        void operator () (const PixelT & a) {
            proximity += sqrt( float( a.get() ) );
            max = maximum( max, a.get() );
        }
        float proximity;
        typename PixelT::value_type max;
    };

    template<class RASTER>
    void findMaximum(const RASTER & mat, MaximumResults & res)
    {
        for (int y = 0; y < mat.vsize(); y++) {
            for (int x = 0; x < mat.hsize(); x++) {
                if(mat(x,y).get() > res.max) {
                    res.max = mat(x,y).get();
                    res.center[0] = static_cast<float>(x);
                    res.center[1] = static_cast<float>(y);
                }
            }
        }

    }

    template <class RASTER>
    struct FindMaximum {
        FindMaximum()
        {   result.max = 0; }
        void operator () (const RASTER & theRaster) {
            findMaximum(theRaster, result);
        }
        MaximumResults result;
    };

    void
    ASSDriver::cureHLine( unsigned theLineNo ) {

        unsigned char * myBrokenLine = _myRawRaster.raster->pixels().begin() +
            theLineNo * _myPoTSize[0];


        unsigned char * myUpperNeighbour = 0;
        unsigned char * myLowerNeighbour = 0;

        if (theLineNo == 0) {
            myUpperNeighbour = myLowerNeighbour = myBrokenLine + _myPoTSize[0];
        } else if ( theLineNo == static_cast<unsigned>(_myGridSize[1]) - 1 ) {
            myUpperNeighbour = myLowerNeighbour = myBrokenLine - _myPoTSize[0];
        } else {
            myUpperNeighbour = myBrokenLine - _myPoTSize[0];
            myLowerNeighbour = myBrokenLine + _myPoTSize[0];
        }
        for (unsigned i = 0; i < static_cast<unsigned>(_myGridSize[0]); ++i) {
            *myBrokenLine = (int(*myUpperNeighbour++) +  int(*myLowerNeighbour++)) >> 1;
            myBrokenLine++;
        }
    }

    void
    ASSDriver::cureVLine( unsigned theRowNo ) {

        unsigned char * myBrokenPoint = 0;
        unsigned char * myLeftNeighbour = 0;
        unsigned char * myRightNeighbour = 0;

        for (unsigned i = 0; i < static_cast<unsigned>(_myGridSize[1]); ++i) {
            myBrokenPoint = _myRawRaster.raster->pixels().begin() + i * _myPoTSize[0] + theRowNo;

            if (theRowNo == 0) {
                myLeftNeighbour = myRightNeighbour = myBrokenPoint + 1;
            } else if ( theRowNo == static_cast<unsigned>(_myGridSize[0]) - 1 ) {
                myLeftNeighbour = myRightNeighbour = myBrokenPoint - 1;
            } else {
                myLeftNeighbour = myBrokenPoint - 1;
                myRightNeighbour = myBrokenPoint + 1;
            }

            *myBrokenPoint = (int(*myLeftNeighbour++) +  int(*myRightNeighbour++)) / 2;
        }
    }


    enum Neighbours {
        LOWER,
        UPPER,
        RIGHT,
        LEFT,
        NUM_NEIGHBOURS
    };


    void
    ASSDriver::curePoint( unsigned theX, unsigned theY ) {
        unsigned char * myBrokenPoint = _myRawRaster.raster->pixels().begin() +
            theY * _myPoTSize[0] + theX;

        unsigned char * myNeighbours[NUM_NEIGHBOURS];

        if (theY == 0) {
            myNeighbours[UPPER] = 0;
        } else if ( theY == static_cast<unsigned>(_myGridSize[1]) - 1 ) {
            myNeighbours[LOWER] = 0;
        } else {
            myNeighbours[UPPER] = myBrokenPoint - _myPoTSize[0];
            myNeighbours[LOWER] = myBrokenPoint + _myPoTSize[0];
        }

        if (theX == 0) {
            myNeighbours[LEFT] = 0;
        } else if ( theX == static_cast<unsigned>(_myGridSize[0]) - 1 ) {
            myNeighbours[RIGHT] = 0;
        } else {
            myNeighbours[LEFT] = myBrokenPoint - 1;
            myNeighbours[RIGHT] = myBrokenPoint + 1;
        }

        unsigned myNumSamples = 0;
        int mySum = 0;
        for (unsigned i = 0; i < NUM_NEIGHBOURS; ++i) {
            if (myNeighbours[i]) {
                mySum += *myNeighbours[i];
                myNumSamples += 1;
            }
        }



        *myBrokenPoint = (unsigned char)(mySum / myNumSamples);
    }

    void
    ASSDriver::cureBrokenElectrodes() {

        for(unsigned int i=0; i<_myCureVLines.size(); i++) {
            if( _myCureVLines[i] != -1 && _myCureVLines[i] < _myGridSize[0]) {
                //AC_PRINT << "cure v line " << _myCureVLines[i] << " i " << i;
                cureVLine(static_cast<unsigned>(_myCureVLines[i]));
            }
        }

        for(unsigned int i=0; i<_myCureHLines.size(); i++) {
            if( _myCureHLines[i] != -1 && _myCureVLines[i] < _myGridSize[1]) {
                //AC_PRINT << "cure h line " << _myCureHLines[i] << " i " << i;
                cureHLine(static_cast<unsigned>(_myCureHLines[i]));
            }
        }

        for(unsigned int i=0; i<_myCurePoints.size(); i++) {
            if( _myCurePoints[i][0] != -1 && _myCurePoints[i][1] != -1 &&
                _myCurePoints[i][0] < _myGridSize[0] && _myCurePoints[i][1] < _myGridSize[1]) {
                //AC_PRINT << "cure point " << _myCurePoints[i][0] << "  " << _myCurePoints[i][1] << " i " << i;
                curePoint( static_cast<unsigned>(_myCurePoints[i][0])
                         , static_cast<unsigned>(_myCurePoints[i][1]) );
            }
        }

    }


    void
    ASSDriver::updateDerivedRasters() {

        if(_myCureBrokenElectrodesFlag) {
            cureBrokenElectrodes();
        }

        const y60::RasterOfGRAY & myRawRaster = *
            dom::dynamic_cast_Value<y60::RasterOfGRAY>( & * (_myRawRaster.value) );
        y60::RasterOfGRAY & myDenoisedRaster = *
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );
        y60::RasterOfGRAY & myMomentRaster = *
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myMomentRaster.value) );

        std::transform( myRawRaster.begin(), myRawRaster.end(),
                        myDenoisedRaster.begin(),
                        Threshold<gray<unsigned char> >( static_cast<unsigned char>(_myNoiseThreshold)));

        std::transform( myDenoisedRaster.begin(), myDenoisedRaster.end(),
                        myMomentRaster.begin(),
                        Power<gray<unsigned char> >( _myGainPower));

        dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );
        dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myMomentRaster.value) );

        // XXX brute force resampling
        if (RESAMPLING) {
            _myResampledRaster.raster->pasteRaster( * _myMomentRaster.value, 0, 0, _myGridSize[0], _myGridSize[1],
                                                    0,0, _myGridSize[0] * GRID_SCALE_X, _myGridSize[1] * GRID_SCALE_Y);
        }
    }

    void
    ASSDriver::processSensorValues(const ASSEvent & theEvent) {

        asl::Time myCurrentTime;
        _myRunTime += myCurrentTime - _myLastFrameTime;
        _myLastFrameTime = myCurrentTime;

        updateDerivedRasters();


        //_myTmpPositions.clear();

        if(_myCaputureSensorDataFlag) {
            captureSensorData();
        }

        BlobListPtr myROIs;
        if (RESAMPLING) {
            //myROIs = connectedComponents( _myResampledRaster.raster, _myComponentThreshold);

            // take the peaks
            y60::RasterOfGRAY & myResampledRaster = *
                dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myResampledRaster.value) );

            // std::transform( myResampledRaster.begin(), myResampledRaster.end(),
            //     myResampledRaster.begin(),
            //     Threshold<gray<unsigned char> >( _myComponentThreshold ));

            asl::GRAY *pixels = myResampledRaster.begin();
            unsigned width = _myResampledRaster.raster->width();
            unsigned height = _myResampledRaster.raster->height();
            for(unsigned y=0; y<height; y++) {
                for(unsigned x=0; x<width; x++) {
                    asl::GRAY val = *(pixels + myResampledRaster.offset(x,y));
                    if( val.get() > _myComponentThreshold ){
                        //AC_PRINT << val;
                        _myTmpPositions.push_back( Vector2f(static_cast<float>(x/GRID_SCALE_X)
                                                           ,static_cast<float>(y/GRID_SCALE_Y)));
                    }
                }
            }
            dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myResampledRaster.value) );


        } else {
            myROIs = connectedComponents( _myMomentRaster.raster, _myComponentThreshold);
            if(_myInterpolationMethod ) {
                std::vector<MomentResults> myCurrentPositions;
                computeCursorPositions( myCurrentPositions, myROIs);
                correlatePositions( myCurrentPositions, myROIs, theEvent );
            } else {
                std::vector<MaximumResults> myCurrentPositions;
                computeCursorPositions( myCurrentPositions, myROIs);
                correlatePositions( myCurrentPositions, myROIs, theEvent );
            }
            updateCursors( 1.0 / _myTransportLayer->getFramerate(), theEvent );
        }


    }

    asl::Vector2f
    ASSDriver::interpolateMaximum(const asl::Vector2f theCenter, float theMaximum) {
        Vector2f myResult(0.0,0.0);

        y60::RasterOfGRAY & myDenoisedRaster = *
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );


        float x0y0 = myDenoisedRaster((unsigned)theCenter[0]-1, (unsigned)theCenter[1]-1).get();
        float x1y0 = myDenoisedRaster((unsigned)theCenter[0], (unsigned)theCenter[1]-1).get();
        float x2y0 = myDenoisedRaster((unsigned)theCenter[0]+1, (unsigned)theCenter[1]-1).get();

        float x0y1 = myDenoisedRaster((unsigned)theCenter[0]-1, (unsigned)theCenter[1]).get();
        //float x1y1 = myDenoisedRaster((unsigned)theCenter[0], (unsigned)theCenter[1]).get();
        float x2y1 = myDenoisedRaster((unsigned)theCenter[0]+1, (unsigned)theCenter[1]).get();

        float x0y2 = myDenoisedRaster((unsigned)theCenter[0]-1, (unsigned)theCenter[1]+1).get();
        float x1y2 = myDenoisedRaster((unsigned)theCenter[0], (unsigned)theCenter[1]+1).get();
        float x2y2 = myDenoisedRaster((unsigned)theCenter[0]+1, (unsigned)theCenter[1]+1).get();

        float x = theCenter[0]
                + 0.125f * ((- (x0y0 / theMaximum)) + (x2y0 / theMaximum))
                + 0.75f  * ((- (x0y1 / theMaximum)) + (x2y1 / theMaximum))
                - 0.125f * ((- (x0y2 / theMaximum)) + (x2y2 / theMaximum));

        float y = theCenter[1]
                + 0.125f * ((- (x0y0 / theMaximum)) + (x0y2 / theMaximum))
                + 0.75f  * ((- (x1y0 / theMaximum)) + (x1y2 / theMaximum))
                - 0.125f * ((- (x2y0 / theMaximum)) + (x2y2 / theMaximum));

        myResult[0] = x;
        myResult[1] = y;

        dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );

        return myResult;
    }


    template<>
    void
    ASSDriver::computeCursorPositions<MomentResults>( std::vector<MomentResults> & theCurrentPositions,
                                                      const BlobListPtr & theROIs)
    {
        typedef subraster<gray<unsigned char> > SubRaster;
        if (RESAMPLING) {
            _myResampledRaster.raster->pasteRaster( * _myDenoisedRaster.value, 0, 0, _myGridSize[0], _myGridSize[1],
                                                    0,0, _myGridSize[0] * GRID_SCALE_X, _myGridSize[1] * GRID_SCALE_Y);
        }
        for (BlobList::const_iterator it = theROIs->begin(); it != theROIs->end(); ++it) {
            Box2i myBox = (*it)->bbox();

            FindMaximum<SubRaster> myMaxAnalysis;
            _myDenoisedRaster.raster->apply( myBox[Box2i::MIN][0], myBox[Box2i::MIN][1],
                                             myBox[Box2i::MAX][0] + 1, myBox[Box2i::MAX][1] + 1, myMaxAnalysis);


            if (!_myUseCCRegionForMomentumFlag) {
                Point2i myCenter = (*it)->bbox().getCenter();
                Point2i myUpperLeft;
                Point2i myUpperRight;
                Point2i myLowerLeft;
                Point2i myLowerRight;
                _myUserDefinedMomentumBox.getCorners(myUpperLeft, myLowerRight, myUpperRight, myLowerLeft);
                myBox = Box2i(max(0, myCenter[0] + myUpperLeft[0]) , max(0,myCenter[1] + myUpperLeft[1]),
                              min(_myGridSize[0], myCenter[0] + myLowerRight[0]) ,min(_myGridSize[1], myCenter[1] + myLowerRight[1]));
            }
            AnalyseMoment<SubRaster> myMomentAnalysis;
            // XXX is this correct ?
            if (RESAMPLING) {
                _myResampledRaster.raster->apply( myBox[Box2i::MIN][0], myBox[Box2i::MIN][1],
                                                  myBox[Box2i::MAX][0] + 1, myBox[Box2i::MAX][1] + 1, myMomentAnalysis);
            } else {
                //            _myMomentRaster.raster->apply( myBox[Box2i::MIN][0], myBox[Box2i::MIN][1],
                //                    myBox[Box2i::MAX][0] + 1, myBox[Box2i::MAX][1] + 1, myMomentAnalysis);
                _myDenoisedRaster.raster->apply( myBox[Box2i::MIN][0], myBox[Box2i::MIN][1],
                                                 myBox[Box2i::MAX][0] + 1, myBox[Box2i::MAX][1] + 1, myMomentAnalysis);
            }

            MomentResults myResult = myMomentAnalysis.result;
            myResult.center += Vector2f( float(myBox[Box2i::MIN][0]),
                                         float(myBox[Box2i::MIN][1]));

            AC_TRACE << "moment position " << myResult.center;
            theCurrentPositions.push_back( myResult );
        }

    }

    template<>
    void
    ASSDriver::computeCursorPositions<MaximumResults>( std::vector<MaximumResults> & theCurrentPositions,
                                                       const BlobListPtr & theROIs)

    {
        typedef subraster<gray<unsigned char> > SubRaster;
        if (RESAMPLING) {
            _myResampledRaster.raster->pasteRaster( * _myDenoisedRaster.value, 0, 0, _myGridSize[0], _myGridSize[1],
                                                    0,0, _myGridSize[0] * GRID_SCALE_X, _myGridSize[1] * GRID_SCALE_Y);
        }
        for (BlobList::const_iterator it = theROIs->begin(); it != theROIs->end(); ++it) {
            Box2i myBox = (*it)->bbox();

            FindMaximum<SubRaster> myMaxAnalysis;
            _myDenoisedRaster.raster->apply( myBox[Box2i::MIN][0], myBox[Box2i::MIN][1],
                                             myBox[Box2i::MAX][0] + 1, myBox[Box2i::MAX][1] + 1, myMaxAnalysis);

            MaximumResults myResult = myMaxAnalysis.result;

            myResult.center += Vector2f( float(myBox[Box2i::MIN][0]),
                                         float(myBox[Box2i::MIN][1]));


            myResult.center = interpolateMaximum(myResult.center, myResult.max);

            AC_TRACE << "maximum position " << myResult.center;

            theCurrentPositions.push_back( myResult );
        }

    }


    template <class N>
    N sqr(const N & n) {
        return n * n;
    }

    void
    ASSDriver::updateCursors(double theDeltaT, const ASSEvent & theEvent) {
        /*y60::RasterOfGRAY & myDenoisedRaster = **/
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );

        /*y60::RasterOfGRAY & myRawRaster = **/
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );

        CursorMap::iterator myCursorIt = _myCursors.begin();
        for(; myCursorIt != _myCursors.end(); ++myCursorIt) {
            findTouch(myCursorIt, theDeltaT, theEvent);
        }
        dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );
        dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );
    }

    void
    ASSDriver::findTouch(CursorMap::iterator & theCursorIt, double theDeltaT,
                         const ASSEvent & theEvent)
    {

        float myFirstDerivative = float((theCursorIt->second.intensity -
                                         theCursorIt->second.previousIntensity) / theDeltaT);
        //float myFirstDerivative = (theCursorIt->second.intensity - theCursorIt->second.getMinIntensity()) / theDeltaT;
        theCursorIt->second.firstDerivative = myFirstDerivative;

        float myTouch = 0;
        if ( myFirstDerivative > _myFirstDerivativeThreshold  &&
             _myRunTime - theCursorIt->second.lastTouchTime > _myMinTouchInterval &&
             theCursorIt->second.intensity > _myMinTouchThreshold)
        {
            myTouch = theCursorIt->second.intensity;
            //AC_PRINT << "touched me! at " << _myRunTime;
            theCursorIt->second.lastTouchTime = _myRunTime;
            createEvent( theCursorIt->first, "touch", theCursorIt->second.position,
                         applyTransform(theCursorIt->second.position, getTransformationMatrix() ),
                         theCursorIt->second.roi, theCursorIt->second.intensity, theEvent );
            _myTouchHistory.push_back( TouchEvent(_myRunTime, theCursorIt->second.position ));
        }

        theCursorIt->second.intensityHistory.push_back( theCursorIt->second.intensity );
        if( theCursorIt->second.intensityHistory.size() > MAX_HISTORY_LENGTH ) {
            theCursorIt->second.intensityHistory.pop_front();
        }

        if (_myDebugTouchEventsFlag) {
            cout << _myRunTime << "\t" << theCursorIt->second.intensity << "\t" << myFirstDerivative*0.1 << "\t" << myTouch
                 << "\t" << theCursorIt->second.position[0] << endl;
        }

    }

    void
    ASSDriver::computeIntensity(CursorMap::iterator & theCursorIt, const y60::RasterOfGRAY & theRaster) {

        const Vector2f & myPosition = theCursorIt->second.position;

        Vector2i myTopLeft;
        myTopLeft[0] = int( floor( myPosition[0] ));
        myTopLeft[1] = int( floor( myPosition[1] ));

        Vector2f myWeight;
        myWeight[0] = myPosition[0] - myTopLeft[0];
        myWeight[1] = myPosition[1] - myTopLeft[1];

        float myHeadRoom = 0.3f;
        myWeight[0] *= 1 - 2 * myHeadRoom;
        myWeight[0] += myHeadRoom;
        myWeight[1] *= 1 - 2 * myHeadRoom;
        myWeight[1] += myHeadRoom;

        float myV00 = sqrt( float( theRaster[ theRaster.hsize() * myTopLeft[1] + myTopLeft[0] ].get()));
        float myV10 = sqrt( float( theRaster[ theRaster.hsize() * myTopLeft[1] + myTopLeft[0] + 1].get()));
        float myV01 = sqrt( float( theRaster[ theRaster.hsize() * (myTopLeft[1] + 1) + myTopLeft[0]].get()));
        float myV11 = sqrt( float( theRaster[ theRaster.hsize() * (myTopLeft[1] + 1) + myTopLeft[0] + 1].get()));

        float myIntensity = myV11 * myWeight[0]  * myWeight[1]+
            myV01 * (1 - myWeight[0]) * myWeight[1] +
            myV10 * myWeight[0] * (1 - myWeight[1]) +
            myV00 * (1 - myWeight[0]) * (1 - myWeight[1]);


        int myROIChanged = 0;
        if ( theCursorIt->second.previousRoi.hasPosition() &&
            (theCursorIt->second.roi[Box2f::MIN] != theCursorIt->second.previousRoi[Box2f::MIN] ||
             theCursorIt->second.roi[Box2f::MAX] != theCursorIt->second.previousRoi[Box2f::MAX]))
        {
            myROIChanged = int(myIntensity);
            myIntensity = (theCursorIt->second.previousIntensity + myIntensity) / 2;
        }


        if (theCursorIt->second.previousIntensity == 0.0) {
            theCursorIt->second.previousIntensity = myIntensity;
        } else {
            theCursorIt->second.previousIntensity = theCursorIt->second.intensity;
        }
        theCursorIt->second.intensity = myIntensity;

    }

    asl::Matrix4f
    ASSDriver::getTransformationMatrix() {
        Matrix4f myTransform;
        //if (_myOverlay ) {
            //const y60::Overlay & myOverlay = *(_myOverlay->getFacade<y60::Overlay>());
            const Vector2f & myPosition = _myTransformEventPosition;//myOverlay.get<Position2DTag>();
            const Vector2f & myScale = _myTransformEventScale;//myOverlay.get<Scale2DTag>();
            const float & myOrientation = _myTransformEventOrientation;//myOverlay.get<Rotation2DTag>();

            myTransform.makeIdentity();

            myTransform.translate( Vector3f( 0.5, 0.5, 0));

            myTransform.scale( Vector3f( myScale[0], myScale[1], 0));
            myTransform.translate( Vector3f( myPosition[0], myPosition[1], 0));
            myTransform.rotateZ( myOrientation );

            // XXX: shearing hack
            Matrix4f myShear;
            myShear.makeIdentity();
            myShear[0][1] = _myShearX;
            myShear[1][0] = _myShearY;
            myShear.setType(asl::UNKNOWN);
            myTransform.postMultiply(myShear);
        //} else {
        //    myTransform.makeIdentity();
        //}
        return myTransform;
    }

#define PAVELS_CORRELATOR
#ifdef PAVELS_CORRELATOR
    template<>
    void
    ASSDriver::correlatePositions<MomentResults>( const std::vector<MomentResults> & theCurrentPositions,
                                                  const BlobListPtr theROIs, const ASSEvent & theEvent)

    {

        /*y60::RasterOfGRAY & myRawRaster = **/
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );

        Matrix4f myTransform = getTransformationMatrix();

        const BlobList & myROIs = * theROIs;

        // populate a map with all distances between existing cursors and new positions
        typedef std::multimap<float, std::pair<int,int> > DistanceMap;
        DistanceMap myDistanceMap;
        float myDistanceThreshold = 4.0;
        if (_myTransportLayer->getGridSpacing() > 0) {
            myDistanceThreshold *= 100.0f / _myTransportLayer->getGridSpacing();
        }
        AC_TRACE << "cursors: " <<_myCursors.size()<< " current positions: " << theCurrentPositions.size() << "; distance threshold: " << myDistanceThreshold;
        CursorMap::iterator myCursorIt  = _myCursors.begin();
        for (; myCursorIt != _myCursors.end(); ++myCursorIt ) {
            //computeIntensity(myCursorIt, myRawRaster);

            myCursorIt->second.correlatedPosition = -1;
            for (unsigned i = 0; i < theCurrentPositions.size(); ++i) {
                float myDistance = magnitude( myCursorIt->second.position + myCursorIt->second.motion
                                              - theCurrentPositions[i].center);
                if (myDistance < myDistanceThreshold) {
                    myDistanceMap.insert(std::make_pair(myDistance, std::make_pair(i, myCursorIt->first)));
                }
            }
        }

        // will contain the correlated cursor id at index n for position n or -1 if uncorrelated
        std::vector<int> myCorrelatedPositions(theCurrentPositions.size(), -1);

        AC_TRACE << "distance map is " << myDistanceMap.size() << " elements long.";

        // iterate through the distance map and correlate cursors in increasing distance order
        for (DistanceMap::iterator dit = myDistanceMap.begin();
             dit!=myDistanceMap.end(); ++dit)
        {
            // check if we already have correlated one of our nodes
            int myPositionIndex = dit->second.first;
            int myCursorId = dit->second.second;
            if (_myCursors[myCursorId].correlatedPosition == -1) {
                if (myCorrelatedPositions[myPositionIndex] == -1)  {
                    // correlate
                    myCorrelatedPositions[myPositionIndex] = myCursorId;
                    Cursor & myCursor = _myCursors[myCursorId];
                    myCursor.correlatedPosition = myPositionIndex;

                    AC_TRACE << "correlated cursor " << myCursorId << " with " << myPositionIndex;

                    // update cursor with new position
                    myCursor.motion = theCurrentPositions[myPositionIndex].center - myCursor.position;
                    myCursor.position = theCurrentPositions[myPositionIndex].center;
                    myCursor.major_direction = theCurrentPositions[myPositionIndex].major_dir;
                    myCursor.minor_direction = theCurrentPositions[myPositionIndex].minor_dir;
                    // TODO: intensity is not up-to-date yet, so we the previous intensity will be posted in the move event
                    //myCursor.previousIntensity = myCursor.intensity;
                    //myCursor.intensity = theCurrentPositions[myPositionIndex].intensity;
                    myCursor.previousRoi = myCursor.roi;
                    myCursor.roi = asBox2f( myROIs[myPositionIndex]->bbox() );

                    // post a move event
                    createEvent( myCursorId, "move", myCursor.position,
                                 applyTransform( myCursor.position, myTransform),
                                 myCursor.roi, myCursor.intensity, theEvent); // TODO
                }
                // place an "else" block here for code if we want to allow to correlate multiple cursors to the same position,
                // but he have to take care that they will be separated at some later point again
            }
        }

        // Now let us iterate through all new positions and create "cursor add" events for every uncorrelated position
        for (unsigned i = 0; i < theCurrentPositions.size(); ++i) {
            if (myCorrelatedPositions[i] == -1)  {
                // new cursor
                int myNewID( _myIDCounter++ );
                AC_TRACE << "new cursor " <<myNewID<< " at " << theCurrentPositions[i].center;
                myCorrelatedPositions[i] = myNewID;
                _myCursors.insert( make_pair( myNewID, MomentCursor( theCurrentPositions[i],
                                                                     asBox2f( myROIs[i]->bbox()) )));
                _myCursors[myNewID].correlatedPosition = i;
                createEvent( myNewID, "add", theCurrentPositions[i].center,
                             applyTransform( theCurrentPositions[i].center, myTransform),
                             asBox2f( myROIs[i]->bbox() ), _myCursors[myNewID].intensity,
                             theEvent); // TODO
            }
        }

        // Now let us iterate through all cursors and create "cursor remove" events for every uncorrelated cursors
        for (CursorMap::iterator myIt = _myCursors.begin(); myIt != _myCursors.end(); ) {
            // advance iterator to allow safe map erase while iterating
            CursorMap::iterator nextIt = myIt;
            ++nextIt;
            if (myIt->second.correlatedPosition == -1) {
                // cursor removed
                createEvent( myIt->first, "remove", myIt->second.position,
                             applyTransform( myIt->second.position, myTransform ),
                             myIt->second.roi, 0 /*myCursor.intensity*/, theEvent); // TODO
                AC_TRACE << "removing cursor "<<myIt->first<<" at " << myIt->second.correlatedPosition;
                _myCursors.erase(myIt);
            }
            myIt = nextIt;
        }

        dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );
    }

    template<>
    void
    ASSDriver::correlatePositions<MaximumResults>( const std::vector<MaximumResults> & theCurrentPositions,
                                                   const BlobListPtr theROIs, const ASSEvent & theEvent)

    {

        /*y60::RasterOfGRAY & myRawRaster = **/
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );

        Matrix4f myTransform = getTransformationMatrix();

        const BlobList & myROIs = * theROIs;

        // populate a map with all distances between existing cursors and new positions
        typedef std::multimap<float, std::pair<int,int> > DistanceMap;
        DistanceMap myDistanceMap;
        float myDistanceThreshold = 4.0;
        if (_myTransportLayer->getGridSpacing() > 0) {
            myDistanceThreshold *= 100.0f / _myTransportLayer->getGridSpacing();
        }
        AC_TRACE << "cursors: " <<_myCursors.size()<< " current positions: " << theCurrentPositions.size() << "; distance threshold: " << myDistanceThreshold;
        CursorMap::iterator myCursorIt  = _myCursors.begin();
        for (; myCursorIt != _myCursors.end(); ++myCursorIt ) {
            //computeIntensity(myCursorIt, myRawRaster);

            myCursorIt->second.correlatedPosition = -1;
            for (unsigned i = 0; i < theCurrentPositions.size(); ++i) {
                float myDistance = magnitude( myCursorIt->second.position + myCursorIt->second.motion
                                              - theCurrentPositions[i].center);
                if (myDistance < myDistanceThreshold) {
                    myDistanceMap.insert(std::make_pair(myDistance, std::make_pair(i, myCursorIt->first)));
                }
            }
        }

        // will contain the correlated cursor id at index n for position n or -1 if uncorrelated
        std::vector<int> myCorrelatedPositions(theCurrentPositions.size(), -1);

        AC_TRACE << "distance map is " << myDistanceMap.size() << " elements long.";

        // iterate through the distance map and correlate cursors in increasing distance order
        for (DistanceMap::iterator dit = myDistanceMap.begin();
             dit!=myDistanceMap.end(); ++dit)
        {
            // check if we already have correlated one of our nodes
            int myPositionIndex = dit->second.first;
            int myCursorId = dit->second.second;
            if (_myCursors[myCursorId].correlatedPosition == -1) {
                if (myCorrelatedPositions[myPositionIndex] == -1)  {
                    // correlate
                    myCorrelatedPositions[myPositionIndex] = myCursorId;
                    Cursor & myCursor = _myCursors[myCursorId];
                    myCursor.correlatedPosition = myPositionIndex;

                    AC_TRACE << "correlated cursor " << myCursorId << " with " << myPositionIndex;

                    // update cursor with new position
                    myCursor.motion = theCurrentPositions[myPositionIndex].center - myCursor.position;
                    myCursor.position = theCurrentPositions[myPositionIndex].center;
                    // TODO: intensity is not up-to-date yet, so we the previous intensity will be posted in the move event
                    //myCursor.previousIntensity = myCursor.intensity;
                    //myCursor.intensity = theCurrentPositions[myPositionIndex].intensity;
                    myCursor.previousRoi = myCursor.roi;
                    myCursor.roi = asBox2f( myROIs[myPositionIndex]->bbox() );

                    // post a move event
                    createEvent( myCursorId, "move", myCursor.position,
                                 applyTransform( myCursor.position, myTransform),
                                 myCursor.roi, myCursor.intensity, theEvent); // TODO
                }
                // place an "else" block here for code if we want to allow to correlate multiple cursors to the same position,
                // but he have to take care that they will be separated at some later point again
            }
        }

        // Now let us iterate through all new positions and create "cursor add" events for every uncorrelated position
        for (unsigned i = 0; i < theCurrentPositions.size(); ++i) {
            if (myCorrelatedPositions[i] == -1)  {
                // new cursor
                int myNewID( _myIDCounter++ );
                AC_TRACE << "new cursor " <<myNewID<< " at " << theCurrentPositions[i].center;
                myCorrelatedPositions[i] = myNewID;
                _myCursors.insert( make_pair( myNewID, MaximumCursor( theCurrentPositions[i],
                                                                      asBox2f( myROIs[i]->bbox()) )));
                _myCursors[myNewID].correlatedPosition = i;
                createEvent( myNewID, "add", theCurrentPositions[i].center,
                             applyTransform( theCurrentPositions[i].center, myTransform),
                             asBox2f( myROIs[i]->bbox() ), _myCursors[myNewID].intensity,
                             theEvent); // TODO
            }
        }

        // Now let us iterate through all cursors and create "cursor remove" events for every uncorrelated cursors
        for (CursorMap::iterator myIt = _myCursors.begin(); myIt != _myCursors.end(); ) {
            // advance iterator to allow safe map erase while iterating
            CursorMap::iterator nextIt = myIt;
            ++nextIt;
            if (myIt->second.correlatedPosition == -1) {
                // cursor removed
                createEvent( myIt->first, "remove", myIt->second.position,
                             applyTransform( myIt->second.position, myTransform ),
                             myIt->second.roi, 0 /*myCursor.intensity*/, theEvent); // TODO
                AC_TRACE << "removing cursor "<<myIt->first<<" at " << myIt->second.correlatedPosition;
                _myCursors.erase(myIt);
            }
            myIt = nextIt;
        }

        dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );
    }
#else
    void
    ASSDriver::correlatePositions( const std::vector<MomentResults> & theCurrentPositions,
                                   const BlobListPtr theROIs)
    {

        Matrix4f myTransform = getTransformationMatrix();

        const BlobList & myROIs = * theROIs;

        std::vector<int> myCorrelatedIds;
        for (unsigned i = 0; i < theCurrentPositions.size(); ++i) {
            float myMinDistance = FLT_MAX;
            CursorMap::iterator myMinDistIt  = _myCursors.end();
            CursorMap::iterator myCursorIt  = _myCursors.begin();
            for (; myCursorIt != _myCursors.end(); ++myCursorIt ) {
                if ( find( myCorrelatedIds.begin(), myCorrelatedIds.end(), myCursorIt->first ) ==
                     myCorrelatedIds.end())
                {
                    float myDistance = magnitude( myCursorIt->second.position - theCurrentPositions[i].center);
                    if (myDistance < myMinDistance) {
                        myMinDistance = myDistance;
                        myMinDistIt = myCursorIt;
                    }
                }
            }
            float myDistanceThreshold = 4.0f;
            if (_myTransportLayer->getGridSpacing() > 0) {
                myDistanceThreshold *= 100.0f / _myTransportLayer->getGridSpacing();
            }
            //AC_PRINT << "distance threshold: " << myDistanceThreshold;
            if (myMinDistIt != _myCursors.end() && myMinDistance < myDistanceThreshold) {
                // cursor moved
                myCorrelatedIds.push_back( myMinDistIt->first );
                myMinDistIt->second.position = theCurrentPositions[i].center;
                //myMinDistIt->second.major_direction = theCurrentPositions[i].major_dir;
                //myMinDistIt->second.minor_direction = theCurrentPositions[i].minor_dir;

                myMinDistIt->second.previousRoi = myMinDistIt->second.roi;
                myMinDistIt->second.roi = asBox2f( myROIs[i]->bbox() );
                createEvent( myMinDistIt->first, "move", theCurrentPositions[i].center,
                             applyTransform( theCurrentPositions[i].center, myTransform),
                             asBox2f( myROIs[i]->bbox()), myMinDistIt->second.intensity );
            } else {
                // new cursor
                int myNewID( _myIDCounter++ );
                _myCursors.insert( make_pair( myNewID, Cursor( theCurrentPositions[i],
                                                               asBox2f( myROIs[i]->bbox()) )));
                myCorrelatedIds.push_back( myNewID );
                createEvent( myNewID, "add", theCurrentPositions[i].center,
                             applyTransform( theCurrentPositions[i].center, myTransform),
                             asBox2f( myROIs[i]->bbox()), myMinDistIt->second.intensity );
            }
        }


        CursorMap::iterator myIt = _myCursors.begin();
        std::vector<int> myOutdatedCursorIds;
        for (; myIt != _myCursors.end(); ++myIt) {
            if ( find( myCorrelatedIds.begin(), myCorrelatedIds.end(), myIt->first ) ==
                 myCorrelatedIds.end())
            {
                // cursor removed
                myOutdatedCursorIds.push_back( myIt->first );
                createEvent( myIt->first, "remove", myIt->second.position,
                             applyTransform( myIt->second.position, myTransform ),
                             myIt->second.roi, myIt->second.intensity);
            }
        }

        for (unsigned i = 0; i < myOutdatedCursorIds.size(); ++i) {
            _myCursors.erase( myOutdatedCursorIds[i] );
        }
    }
#endif

    void
    ASSDriver::drawBox(const Cursor & theCursor) {
        Box2f myBox = theCursor.roi;
        if (!_myUseCCRegionForMomentumFlag) {
            Point2f myCenter = theCursor.roi.getCenter();
            Point2i myUpperLeft;
            Point2i myUpperRight;
            Point2i myLowerLeft;
            Point2i myLowerRight;
            _myUserDefinedMomentumBox.getCorners(myUpperLeft, myLowerRight, myUpperRight, myLowerLeft);
            myBox = Box2f(myCenter[0] + myUpperLeft[0], myCenter[1] + myUpperLeft[1],
                          myCenter[0] + myLowerRight[0], myCenter[1] + myLowerRight[1]);
        }
        Point2f myUpperLeft;
        Point2f myUpperRight;
        Point2f myLowerLeft;
        Point2f myLowerRight;
        myBox.getCorners(myUpperLeft, myLowerRight, myUpperRight, myLowerLeft);
        glColor4f(1,1,1,1);
        glBegin( GL_LINE_LOOP );
        glVertex2fv(myUpperLeft.begin());
        glVertex2fv(myLowerLeft.begin());
        glVertex2fv(myLowerRight.begin());
        glVertex2fv(myUpperRight.begin());
        glEnd();

    }

    void
    ASSDriver::drawGrid() {
        glColor4fv( _myGridColor.begin() );
        glBegin( GL_LINES );
        for (unsigned i = 0; i < static_cast<unsigned>(_myGridSize[0]); ++i) {
            glVertex2f(float(i), 0.0);
            glVertex2f(float(i), float(_myGridSize[1] - 1));
        }
        for (unsigned i = 0; i < static_cast<unsigned>(_myGridSize[1]); ++i) {
            glVertex2f( 0.0, float(i));
            glVertex2f( float(_myGridSize[0] - 1), float(i));
        }
        glEnd();
    }


    void
    ASSDriver::drawCircle( const Vector2f & thePosition, float theRadius,
                           unsigned theSubdivision, const Vector4f & theColor)
    {
        float myStep = 2.f * float(PI) / theSubdivision;

        glColor4fv( theColor.begin() );
        glBegin( GL_LINE_LOOP );
        for (unsigned i = 0; i < theSubdivision; ++i) {
            Vector2f myPosition( theRadius * sin( i * myStep ), theRadius * cos( i * myStep ));
            glVertex2fv( (myPosition + thePosition).begin() );
        }
        glEnd();
    }

    void
    ASSDriver::drawMarkers() {
        // draw origin
        drawCircle( Vector2f(0.0, 0.0), 0.15f, 36, _myGridColor );

        /*
          std::vector<asl::Vector2f>::iterator myIt = _myTmpPositions.begin();
          for (; myIt != _myTmpPositions.end(); ++myIt ) {
          drawCircle(*myIt, 0.15f, 36, _myTouchColor);
          }
        */

        // draw cursors
        CursorMap::iterator myIt = _myCursors.begin();
        for (; myIt != _myCursors.end(); ++myIt ) {
            if (RESAMPLING) {
                Vector2f myPos = myIt->second.position;
                myPos[0] *= 0.5;
                drawCircle( myPos, 0.15f, 36, _myCursorColor );
            } else {
                drawCircle( myIt->second.position, 0.15f, 36, _myCursorColor );
            }
            drawBox(myIt->second);
        }

        // draw touch history
        if ( ! _myTouchHistory.empty() ) {
            //std::vector<TouchEvent>::reverse_iterator myIt = _myTouchHistory.rbegin();
            std::vector<TouchEvent>::iterator myIt = _myTouchHistory.begin();
            while ( myIt != _myTouchHistory.end() ) {
                if (_myRunTime - myIt->birthTime > TOUCH_MARKER_LIFE_TIME) {
                    myIt = _myTouchHistory.erase( myIt );
                } else {
                    drawCircle( myIt->position, 0.20f, 36, _myTouchColor);
                    myIt += 1;
                }
            }
        }

        // draw value probe
        if (_myProbePosition[0] >= 0 && _myProbePosition[1] >= 0) {
            drawCircle( _myProbePosition, 0.15f, 36, _myProbeColor);
        }

    }

    void
    ASSDriver::drawLabel(jslib::AbstractRenderWindow * theWindow, const std::string & theText) {
        TextStyle myTextStyle = theWindow->getTextStyle();
        //TextStyle myLabelTextStyle;
        //myLabelTextStyle._myTextColor = _myTextColor;
        theWindow->setTextStyle( TextStyle(_myTextColor) );
        theWindow->renderText( Vector2f( 50, 50 ), theText, "Screen15");
        theWindow->setTextStyle( myTextStyle );
    }

    void
    ASSDriver::onPostRender(jslib::AbstractRenderWindow * theWindow) {
        if ( _myScene) {
            if (_myOverlay) {
                y60::OverlayPtr myOverlay = _myOverlay->getFacade<y60::Overlay>();

                if ( ! myOverlay->get<VisibleTag>() ) {
                    return;
                }


                glPushAttrib( GL_ALL_ATTRIB_BITS );
                glMatrixMode( GL_PROJECTION );
                glPushMatrix();
                glLoadIdentity();

                dom::Node * myOverlayList = _myOverlay->parentNode();
                if ( ! myOverlayList ) {
                    throw ASSException("Failed to get owning viewport.", PLUS_FILE_LINE );
                }
                dom::Node * myViewportNode = myOverlayList->parentNode();
                if ( ! myViewportNode ) {
                    throw ASSException("Failed to get owning viewport.", PLUS_FILE_LINE );
                }
                y60::ViewportPtr myViewport = myViewportNode->getFacade<y60::Viewport>();

                gluOrtho2D(0.0, myViewport->get<ViewportWidthTag>(), myViewport->get<ViewportHeightTag>(), 0.0);

                glMatrixMode( GL_MODELVIEW );
                if (_myTransportLayer) {
                    if ( _myTransportLayer->getState() == RUNNING ) {
                        glPushMatrix();
                        Matrix4f myMat = getTransformationMatrix();
                        glLoadMatrixf( static_cast<const GLfloat*>( myMat.getData()));

                        glDisable( GL_DEPTH_TEST );

                        drawGrid();
                        drawMarkers();

                        glPopMatrix();

                        if (_myProbePosition[0] >= 0 && _myProbePosition[1] >= 0) {
                            unsigned char myValue = * (_myRawRaster.raster->pixels().begin() +
                                                       int(_myProbePosition[1]) * _myPoTSize[0] + int(_myProbePosition[0]));
                            drawLabel(theWindow, string("Value") + as_string(_myProbePosition) +
                                      " = " + as_string(int(myValue)));
                        }
                    } else {
                        drawLabel( theWindow, _myTransportLayer->getState().asString());
                    }

                }
                glMatrixMode( GL_PROJECTION );
                glPopMatrix();
                glMatrixMode( GL_MODELVIEW );
                glPopAttrib();
            } else {
                drawLabel( theWindow, "No transport layer");
            }
        }
    }

// XXX: These macros might be worth publishing [DS]
#define GET_PROP( theName, theVar )             \
    if (thePropertyName == #theName) {          \
        theReturnValue.set( theVar );           \
        return;                                 \
    }

#define RO_PROP( theName )                                          \
    if (thePropertyName == #theName) {                              \
        AC_ERROR << "Property '" << #theName << "' is read only.";  \
        return;                                                     \
    }

#define SET_PROP( theName, theVar )             \
    if (thePropertyName == #theName) {          \
        thePropertyValue.get( theVar );         \
        return;                                 \
    }

    void
    ASSDriver::onGetProperty(const std::string & thePropertyName,
                             PropertyValue & theReturnValue) const
    {
        GET_PROP( gridSize, _myGridSize );
        GET_PROP( overlay, _myOverlay );
        GET_PROP( rasterNames, _myRasterNames );

        GET_PROP( gridColor, _myGridColor );
        GET_PROP( cursorColor, _myCursorColor );
        GET_PROP( touchColor, _myTouchColor );
        GET_PROP( textColor, _myTextColor );
        GET_PROP( probeColor, _myProbeColor );
        GET_PROP( resampleColor, _myResampleColor );

        GET_PROP( transformEventPosition, _myTransformEventPosition );
        GET_PROP( transformEventScale, _myTransformEventScale );
        GET_PROP( transformEventOrientation, _myTransformEventOrientation );

        // XXX: shearing hack
        GET_PROP( shearX, _myShearX );
        GET_PROP( shearY, _myShearY );

        AC_ERROR << "Unknown property '" << thePropertyName << "'.";
    }

    void
    ASSDriver::onSetProperty(const std::string & thePropertyName,
                             const PropertyValue & thePropertyValue)
    {
        RO_PROP( gridSize );
        SET_PROP( overlay, _myOverlay );
        RO_PROP( rasterNames );
        SET_PROP( gridColor, _myGridColor );
        SET_PROP( cursorColor, _myCursorColor );
        SET_PROP( touchColor, _myTouchColor );
        SET_PROP( textColor, _myTextColor );
        SET_PROP( probeColor, _myProbeColor );
        SET_PROP( resampleColor, _myResampleColor );

        SET_PROP( transformEventPosition, _myTransformEventPosition );
        SET_PROP( transformEventScale, _myTransformEventScale );
        SET_PROP( transformEventOrientation, _myTransformEventOrientation );

        // ensure overlay for legacy reasons
        if (_myOverlay) {
            y60::Overlay & myOverlay = *(_myOverlay->getFacade<y60::Overlay>());
            myOverlay.set<Position2DTag>(_myTransformEventPosition);
            myOverlay.set<Scale2DTag>(_myTransformEventScale);
            myOverlay.set<Rotation2DTag>(_myTransformEventOrientation);
        }

        // XXX: shearing hack
        SET_PROP( shearX, _myShearX );
        SET_PROP( shearY, _myShearY );

        AC_ERROR << "Unknown property '" << thePropertyName << "'.";
    }


    void
    ASSDriver::onUpdateSettings(dom::NodePtr theSettings) {


        AC_DEBUG << "updating ASSDriver settings";
        _mySettings = getASSSettings( theSettings );
        setupDriver(_mySettings);
    }

    void
    ASSDriver::setupDriver(dom::NodePtr theSettings) {
        if (!theSettings) {
            throw ASSException(string("Sorry, setup of assdriver with invalid settings node "),
                               PLUS_FILE_LINE);
        }
        if ( _myTransportLayer && _myTransportLayer->settingsChanged( theSettings ) ) {
            _myTransportLayer = TransportLayerPtr();
        }

        if ( ! _myTransportLayer ) {
            string myTransportName;
            getConfigSetting( theSettings, "TransportLayer", myTransportName, string("serial") );

            if ( myTransportName == "serial" ) {
                TransportLayer* myTmp = new SerialTransport(theSettings);
                _myTransportLayer = TransportLayerPtr(myTmp);
            } else if ( myTransportName == "socket" ) {
                _myTransportLayer = TransportLayerPtr( new SocketTransport(theSettings) );
            } else if ( myTransportName == "dummy" ) {
                _myTransportLayer = TransportLayerPtr( new DummyTransport(theSettings) );
            }
            /*} else if ( myTransportName == "udp") {
              TODO: UDP transport layer
              }*/ else {
                throw ASSException(string("Unknown transport layer '") + myTransportName + "'",
                                   PLUS_FILE_LINE);
            }

            /*} else {
              AC_ERROR << "driver setup: no TransportLayer setting found.";*/
            _myTransportLayer->fork();
            _myTransportLayer->setRealtimePriority();
        }

        getConfigSetting( theSettings, "ComponentThreshold", _myComponentThreshold, 5 );
        getConfigSetting( theSettings, "NoiseThreshold", _myNoiseThreshold, 15 );
        getConfigSetting( theSettings, "GainPower", _myGainPower, 2.0f );
        getConfigSetting( theSettings, "FirstDerivativeThreshold", _myFirstDerivativeThreshold, 25.0f );
        getConfigSetting( theSettings, "DebugTouchEvents", _myDebugTouchEventsFlag, 0 );
        getConfigSetting( theSettings, "ProbePosition", _myProbePosition, Vector2f( -1, -1) );
        getConfigSetting( theSettings, "MinTouchInterval", _myMinTouchInterval, 0.25 );
        getConfigSetting( theSettings, "MinTouchThreshold", _myMinTouchThreshold, 9.0f );
        getConfigSetting( theSettings, "ClampToScreen", _myClampToScreenFlag, 0);
        getConfigSetting( theSettings, "CCRegion4MomentumFlag", _myUseCCRegionForMomentumFlag, 0);
        getConfigSetting( theSettings, "InterpolationMethod", _myInterpolationMethod, 0);
        getConfigSetting( theSettings, "CureBrokenElectrodes", _myCureBrokenElectrodesFlag, 0);
        getConfigSetting( theSettings, "CaptureSensorData", _myCaputureSensorDataFlag, 0);

        asl::Vector4f myMomentumRegion;
        getConfigSetting( theSettings, "MomentumRegion", myMomentumRegion, asl::Vector4f(-1,-1,1,1));
        _myUserDefinedMomentumBox = Box2i(int(myMomentumRegion[0]), int(myMomentumRegion[1]), int(myMomentumRegion[2]), int(myMomentumRegion[3]));


        asl::Vector4f myCureVLines;
        getConfigSetting( theSettings, "CureVLineValues", myCureVLines, asl::Vector4f(-1,-1,-1,-1));
        _myCureVLines = Vector4f(myCureVLines[0], myCureVLines[1], myCureVLines[2], myCureVLines[3]);

        asl::Vector4f myCureHLines;
        getConfigSetting( theSettings, "CureHLineValues", myCureHLines, asl::Vector4f(-1,-1,-1,-1));
        _myCureHLines = Vector4f(myCureHLines[0], myCureHLines[1], myCureHLines[2], myCureHLines[3]);

        asl::Vector2f myCurePoint1;
        asl::Vector2f myCurePoint2;
        asl::Vector2f myCurePoint3;
        asl::Vector2f myCurePoint4;

        _myCurePoints.clear();
        getConfigSetting( theSettings, "CurePoint1", myCurePoint1, asl::Vector2f(-1,-1));
        _myCurePoints.push_back(myCurePoint1);

        getConfigSetting( theSettings, "CurePoint2", myCurePoint2, asl::Vector2f(-1,-1));
        _myCurePoints.push_back(myCurePoint2);

        getConfigSetting( theSettings, "CurePoint3", myCurePoint3, asl::Vector2f(-1,-1));
        _myCurePoints.push_back(myCurePoint3);

        getConfigSetting( theSettings, "CurePoint4", myCurePoint4, asl::Vector2f(-1,-1));
        _myCurePoints.push_back(myCurePoint4);


        getConfigSetting( theSettings, "SensorPosition", _myTransformEventPosition, asl::Vector2f(0.0f,0.0f));
        getConfigSetting( theSettings, "SensorScale", _myTransformEventScale, asl::Vector2f(1.0f,1.0f));
        getConfigSetting( theSettings, "SensorOrientation", _myTransformEventOrientation, 0.0f);
        int myMirrorxFlag = 0;
        getConfigSetting( theSettings, "MirrorX", myMirrorxFlag, 0);
        int myMirroryFlag = 0;
        getConfigSetting( theSettings, "MirrorY", myMirroryFlag, 0);
        if (myMirrorxFlag) {
            _myTransformEventScale[0] *= -1;
        }
        if (myMirroryFlag) {
            _myTransformEventScale[1] *= -1;
        }
        // ensure overlay for legacy reasons
        if (_myOverlay) {
            y60::Overlay & myOverlay = *(_myOverlay->getFacade<y60::Overlay>());
            myOverlay.set<Position2DTag>(_myTransformEventPosition);
            myOverlay.set<Scale2DTag>(_myTransformEventScale);
            myOverlay.set<Rotation2DTag>(_myTransformEventOrientation);
        }


    }

    Vector3f
    ASSDriver::applyTransform( const Vector2f & theRawPosition,
                               const Matrix4f & theTransform )
    {
        Vector4f myHPosition;
        myHPosition[0] = theRawPosition[0];
        myHPosition[1] = theRawPosition[1];
        myHPosition[2] = 0.0;
        myHPosition[3] = 1.0;
        myHPosition = myHPosition * theTransform;
        Vector3f my3DPosition;
        my3DPosition[0] = myHPosition[0];
        my3DPosition[1] = myHPosition[1];
        my3DPosition[2] = myHPosition[2];
        if ( _myWindow && _myClampToScreenFlag ) {
            y60::CanvasPtr myCanvas = _myWindow->getCanvas()->getFacade<y60::Canvas>();
            my3DPosition[0] = clamp( my3DPosition[0], 0.0f, float(myCanvas->getWidth()) );
            my3DPosition[1] = clamp( my3DPosition[1], 0.0f, float(myCanvas->getHeight()) );
        }
        return my3DPosition;
    }

#ifdef ASS_LATENCY_TEST
    void
    ASSDriver::toggleLatencyTestPin() {
        static bool state = true;
        //static double lastTime= 0;
        //double now = asl::Time();
        //AC_PRINT << "DTR: " << state;
        _myLatencyTestPort->setStatusLine( state ? SerialDevice::DTR : 0 );
        state = ! state;
        //lastTime = now;
    }
#endif

    void
    ASSDriver::processInput() {
        if (_myTransportLayer) {
            std::vector<ASSEvent> myFrames;
            _myTransportLayer->getEvents(myFrames);

            AC_DEBUG << "Got " << myFrames.size() << " frames.";

            for(std::vector<ASSEvent>::iterator i = myFrames.begin(); i != myFrames.end(); i++) {
                ASSEvent& myEvent = *i;

                //AC_TRACE << "popped event " << myEvent;

                switch (myEvent.type) {
                    case ASS_FRAME:
                    {
                        beginFrame();
                        if (myEvent.size != _myGridSize ) {
                            allocateGridBuffers( myEvent.size );
                        }
                        copyFrame( myEvent.data );
                        // TODO use smart pointers
                        delete [] myEvent.data;
                        processSensorValues(myEvent);
#ifdef ASS_LATENCY_TEST
                        if (myEvent.frameno % 16 == 0) {
                            toggleLatencyTestPin();
                        }
#endif
                        finishFrame();
                    }
                        break;
                    case ASS_LOST_SYNC:
                        createTransportLayerEvent("lost_sync");
                        break;
                    case ASS_LOST_COM:
                        createTransportLayerEvent("lost_communication");
                        break;
                    case ASS_INVALID:
                        AC_DEBUG << "assEvent: ASS_INVALID";
                        break;
                }
            }
        }
    }

    /**
     * Saves the caputred and processed sensor data to disc.
     *
     * The images are stored as SensorData#.png in the current working
     * directory. Note: you need to provide a scene for the ASSDriver
     * because images are stored in the DOM.
     */
    void
    ASSDriver::captureSensorData( ) {
        if (_myScene) {
            dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(MOMENT_RASTER);
            std::ostringstream streamer;
            streamer << setw(5) << setfill('0') << _myCapturedFrameCounter++;

            myImage->getFacade<y60::Image>()->saveToFile("SensorData" + streamer.str() + ".png");

        } else {
            AC_ERROR << "no scene provided; captureing is only possible when rasters are not loose allocated";
        }
    }

    void
    ASSDriver::copyFrame(unsigned char * theData ) {

        /*y60::RasterOfGRAY & myRaster = **/
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );


        unsigned char * myDstPtr = _myRawRaster.raster->pixels().begin();
        unsigned char * mySrcPtr = theData;
        for (unsigned y = 0; y < static_cast<unsigned>(_myGridSize[1]); ++y) {
            memcpy( myDstPtr, mySrcPtr, _myGridSize[0]);
            myDstPtr += _myPoTSize[0];
            mySrcPtr += _myGridSize[0];
        }

        dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );
    }

    void
    ASSDriver::performTara() {
        //AC_PRINT << "ASSDriver::performTara()";
        queueCommand( CMD_PERFORM_TARA );
    }

    void
    ASSDriver::callibrateTransmissionLevels() {
        //AC_PRINT << "ASSDriver::callibrateTransmissionLevels()";
        queueCommand( CMD_CALLIBRATE_TRANSMISSION_LEVEL );
    }

    void
    ASSDriver::disconnect() {
        AC_PRINT << "ASSDriver::disconnect()";
        _myTransportLayer = TransportLayerPtr();
    }
    void
    ASSDriver::connect() {
        AC_PRINT << "ASSDriver::connect()";
        setupDriver( _mySettings );
    }
    void
    ASSDriver::queryConfigMode() {
        //AC_PRINT << "ASSDriver::queryConfigMode()";
        queueCommand( CMD_QUERY_CONFIG_MODE );
    }

    void
    ASSDriver::queueCommand( const char * theCommand ) {
        if (_myTransportLayer) {
            _myTransportLayer->queueCommand( theCommand );
        } else {
            AC_WARNING << "Can not send controller command '" << theCommand << "'. Not connected.";
        }
    }

    static JSBool
    PerformTara(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;

        asl::Ptr<ASSDriver> myNative = getNativeAs<ASSDriver>(cx, obj);
        if (myNative) {
            myNative->performTara();
        } else {
            assert(myNative);
        }
        return JS_TRUE;
    }

    static JSBool
    Connect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;

        asl::Ptr<ASSDriver> myNative = getNativeAs<ASSDriver>(cx, obj);
        if (myNative) {
            myNative->connect();
        } else {
            assert(myNative);
        }
        return JS_TRUE;
    }

    static JSBool
    Disconnect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;

        asl::Ptr<ASSDriver> myNative = getNativeAs<ASSDriver>(cx, obj);
        if (myNative) {
            myNative->disconnect();
        } else {
            assert(myNative);
        }
        return JS_TRUE;
    }

    static JSBool
    CallibrateTransmissionLevels(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;

        asl::Ptr<ASSDriver> myNative = getNativeAs<ASSDriver>(cx, obj);
        if (myNative) {
            myNative->callibrateTransmissionLevels();
        } else {
            assert(myNative);
        }
        return JS_TRUE;
    }

    static JSBool
    QueryConfigMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("");
        DOC_END;

        asl::Ptr<ASSDriver> myNative = getNativeAs<ASSDriver>(cx, obj);
        if (myNative) {
            myNative->queryConfigMode();
        } else {
            assert(myNative);
        }
        return JS_TRUE;
    }

    JSFunctionSpec *
    ASSDriver::Functions() {
        static JSFunctionSpec myFunctions[] = {
            {"connect", Connect, 0},
            {"disconnect", Disconnect, 0},
            {"performTara", PerformTara, 0},
            {"callibrateTransmissionLevels", CallibrateTransmissionLevels, 0},
            {"queryConfigMode", QueryConfigMode, 0},
            {0}
        };
        return myFunctions;
    }

} // end of namespace y60

