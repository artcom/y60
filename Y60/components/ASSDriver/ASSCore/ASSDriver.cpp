//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "ASSDriver.h"
#include "SerialTransport.h"
#include "ASSUtils.h"

#include <asl/Logger.h>
#include <asl/Assure.h>
#include <asl/numeric_functions.h>

#include <y60/ImageBuilder.h>
#include <y60/PixelEncoding.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/Overlay.h>

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


const char * CMD_ENTER_CONFIG_MODE( "x");
const char * CMD_QUERY_CONFIG_MODE( "c98");
const char * CMD_LEAVE_CONFIG_MODE( "c99");

const char * CMD_CALLIBRATE_TRANSMISSION_LEVEL( "c01");
const char * CMD_PERFORM_TARA( "c02" );


static const char * RAW_RASTER = "ASSRawRaster";
static const char * FILTERED_RASTER = "ASSFilteredRaster";
static const char * MOMENT_RASTER = "ASSMomentRaster";

const unsigned int BEST_VERSION( 260 );


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
    _myRawRaster(dom::ValuePtr(0)),
    _myDenoisedRaster(dom::ValuePtr(0)),
    _myMomentRaster(dom::ValuePtr(0)),
    _myScene(0),
    _myNoiseThreshold( 15 ),
    _myComponentThreshold( 5 ),
    _myFirstDerivativeThreshold( 200 ),
    _myGainPower(2.0f),
    _myMinTouchInterval( 0.25 ),
    _myIDCounter( 0 ),
    _myDebugTouchEventsFlag( 0 ),
    _myLastFrameTime( asl::Time() ),
    _myRunTime(0.0),
    _myProbePosition( -1, -1),

    _myGridColor(0.5, 0.5, 0, 1.0),
    _myCursorColor(0.5, 0.5, 0, 1.0),
    _myTouchColor(1.0, 1.0, 1.0, 1.0),
    _myTextColor(1.0, 1.0, 1.0, 1.0),
    _myProbeColor(0.0, 0.75, 0.0, 1.0),

    _myClampToScreenFlag(false),
    _myWindow( 0 ),
    _mySettings(dom::NodePtr(0))

{
}

ASSDriver::~ASSDriver() {
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
    _myRawRaster = allocateRaster(RAW_RASTER);
    _myDenoisedRaster = allocateRaster(FILTERED_RASTER);
    _myMomentRaster = allocateRaster(MOMENT_RASTER);

    createTransportLayerEvent( "configure" );
}

RasterHandle
ASSDriver::allocateRaster(const std::string & theName) {
    if (_myScene) {
        dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(theName);
        if (myImage) {
            myImage->getFacade<y60::Image>()->createRaster( 
                    _myPoTSize[0], _myPoTSize[1], 1, y60::GRAY);
            _myRasterNames.push_back( theName );
            return RasterHandle( myImage->childNode(0)->childNode(0)->nodeValueWrapperPtr());
        } else {
            ImageBuilder myImageBuilder(theName, false);
            _myScene->getSceneBuilder()->appendImage(myImageBuilder);
            y60::ImagePtr myImage = myImageBuilder.getNode()->getFacade<y60::Image>();
            myImage->set<y60::IdTag>( theName );
            myImage->set<y60::NameTag>( theName );
            myImage->createRaster( _myPoTSize[0], _myPoTSize[1], 1, y60::GRAY);

            _myRasterNames.push_back( theName );

            return RasterHandle( myImage->getNode().childNode(0)->childNode(0)->nodeValueWrapperPtr());
        }
    } else {
        AC_WARNING << "No scene. Allocating loose rasters.";
        RasterHandle myHandle(createRasterValue(y60::GRAY, _myPoTSize[0], _myPoTSize[1]));
        return myHandle;
                
    }
}

template <class PixelT>
struct Threshold{
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

template <class PixelT>
struct FindMaximum {
    FindMaximum() :
        max( 0 ) {}
    void operator () (const PixelT & a) {
        max = maximum( max, sqrt(float(a.get())));
    }
    float max;
};

void
ASSDriver::triggerUpload( const char * theRasterId ) {
    if (_myScene) {
        _myScene->getSceneDom()->getElementById(theRasterId)->getFacade<y60::Image>()->triggerUpload();
    }
}

void
ASSDriver::updateDerivedRasters()
{
    const y60::RasterOfGRAY & myRawRaster = *
            dom::dynamic_cast_Value<y60::RasterOfGRAY>( & * (_myRawRaster.value) );
    y60::RasterOfGRAY & myDenoisedRaster = *
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );
    y60::RasterOfGRAY & myMomentRaster = *
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myMomentRaster.value) );

    std::transform( myRawRaster.begin(), myRawRaster.end(),
            myDenoisedRaster.begin(),
            Threshold<gray<unsigned char> >( _myNoiseThreshold));

    std::transform( myDenoisedRaster.begin(), myDenoisedRaster.end(),
            myMomentRaster.begin(),
            Power<gray<unsigned char> >( _myGainPower));

    dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );
    dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myMomentRaster.value) );

    triggerUpload( RAW_RASTER );
    triggerUpload( FILTERED_RASTER );
    triggerUpload( MOMENT_RASTER );
}

void
ASSDriver::processSensorValues() {

    asl::Time myCurrentTime;
    _myRunTime += myCurrentTime - _myLastFrameTime;
    _myLastFrameTime = myCurrentTime;

    updateDerivedRasters();

    BlobListPtr myROIs = connectedComponents( _myMomentRaster.raster, _myComponentThreshold);

    std::vector<MomentResults> myCurrentPositions;
    computeCursorPositions( myCurrentPositions, myROIs);
    correlatePositions( myCurrentPositions, myROIs );
    updateCursors( 1.0 / _myTransportLayer->getFramerate() );
}

void
ASSDriver::computeCursorPositions( std::vector<MomentResults> & theCurrentPositions, 
                                   const BlobListPtr & theROIs)
{
    typedef subraster<gray<unsigned char> > SubRaster;
    for (BlobList::const_iterator it = theROIs->begin(); it != theROIs->end(); ++it) {
        Box2i myBox = (*it)->bbox();
        AnalyseMoment<SubRaster> myMomentAnalysis;
        // XXX is this correct ?
        _myMomentRaster.raster->apply( myBox[Box2i::MIN][0], myBox[Box2i::MIN][1],
                                    myBox[Box2i::MAX][0] + 1, myBox[Box2i::MAX][1] + 1, myMomentAnalysis);

        MomentResults myResult = myMomentAnalysis.result;
        myResult.center += Vector2f( float(myBox[Box2i::MIN][0]),
                                     float(myBox[Box2i::MIN][1]));

        //AC_PRINT << "major angle: " << myMomentAnalysis.result.major_angle << " major dir: " << myMomentAnalysis.result.major_dir;
        /*
        AC_PRINT << "moment: w: " << myMomentAnalysis.result.w 
                 << " l: " << myMomentAnalysis.result.l
                 << " major dir: " << myMomentAnalysis.result.major_dir
                 << " minor dir: " << myMomentAnalysis.result.minor_dir;
                 */

        theCurrentPositions.push_back( myResult );
    }

}

template <class N>
N sqr(const N & n) {
    return n * n;
}

void 
ASSDriver::updateCursors(double theDeltaT) {
 y60::RasterOfGRAY & myDenoisedRaster = *
        dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );

    y60::RasterOfGRAY & myRawRaster = *
        dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );

    CursorMap::iterator myCursorIt = _myCursors.begin();
    for(; myCursorIt != _myCursors.end(); ++myCursorIt) {
        computeIntensity(myCursorIt, myRawRaster);
        findTouch(myCursorIt, theDeltaT);

    }
    dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myDenoisedRaster.value) );
    dom::dynamic_cast_and_closeWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );
}

void
ASSDriver::findTouch(CursorMap::iterator & theCursorIt, double theDeltaT) {
    
    float myFirstDerivative = float((theCursorIt->second.intensity -
            theCursorIt->second.previousIntensity) / theDeltaT);
    //float myFirstDerivative = (theCursorIt->second.intensity - theCursorIt->second.getMinIntensity()) / theDeltaT;
    theCursorIt->second.firstDerivative = myFirstDerivative; 
    
    float myTouch = 0;
    if ( myFirstDerivative > _myFirstDerivativeThreshold  &&
        _myRunTime - theCursorIt->second.lastTouchTime > _myMinTouchInterval &&
        theCursorIt->second.intensity > 9) // XXX This should be tweakable! -pm
    {
        myTouch = theCursorIt->second.intensity;   
        //AC_PRINT << "touched me! at " << _myRunTime;
        theCursorIt->second.lastTouchTime = _myRunTime;
        createEvent( theCursorIt->first, "touch", theCursorIt->second.position,
                applyTransform(theCursorIt->second.position, getTransformationMatrix() ),
                theCursorIt->second.roi, theCursorIt->second.intensity );
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
            theCursorIt->second.roi[Box2f::MIN] != theCursorIt->second.previousRoi[Box2f::MIN] ||
            theCursorIt->second.roi[Box2f::MAX] !=  theCursorIt->second.previousRoi[Box2f::MAX])
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
    if (_myOverlay ) {
        const y60::Overlay & myOverlay = *(_myOverlay->getFacade<y60::Overlay>());
        const Vector2f & myPosition = myOverlay.get<Position2DTag>();
        const Vector2f & myScale = myOverlay.get<Scale2DTag>();
        const float & myOrientation = myOverlay.get<Rotation2DTag>();

        myTransform.makeIdentity();

        myTransform.translate( Vector3f( 0.5, 0.5, 0));

        myTransform.scale( Vector3f( myScale[0], myScale[1], 0));
        myTransform.translate( Vector3f( myPosition[0], myPosition[1], 0));
        myTransform.rotateZ( myOrientation );
    } else {
        myTransform.makeIdentity();
    }
    return myTransform;
}

#define PAVELS_CORRELATOR
#ifdef PAVELS_CORRELATOR 
void 
ASSDriver::correlatePositions( const std::vector<MomentResults> & theCurrentPositions,
        const BlobListPtr theROIs)
{
    Matrix4f myTransform = getTransformationMatrix();

    const BlobList & myROIs = * theROIs;

    // populate a map with all distances between existing cursors and new positions 
    typedef std::multimap<float, std::pair<int,int> > DistanceMap;
    DistanceMap myDistanceMap;
    float myDistanceThreshold = 4.0;
    if (_myTransportLayer->getGridSpacing() > 0) {
        myDistanceThreshold *= 100.0 / _myTransportLayer->getGridSpacing();
    }
    AC_TRACE << "cursors: " <<_myCursors.size()<< " current positions: " << theCurrentPositions.size() << "; distance threshold: " << myDistanceThreshold;
    CursorMap::iterator myCursorIt  = _myCursors.begin();
    for (; myCursorIt != _myCursors.end(); ++myCursorIt ) {
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
                        myCursor.roi, 0 /*myCursor.intensity*/); // TODO
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
            _myCursors.insert( make_pair( myNewID, Cursor( theCurrentPositions[i],
                            asBox2f( myROIs[i]->bbox()) )));
            _myCursors[myNewID].correlatedPosition = i;
            createEvent( myNewID, "add", theCurrentPositions[i].center,
                    applyTransform( theCurrentPositions[i].center, myTransform),
                    asBox2f( myROIs[i]->bbox() ), 0 /*myCursor.intensity*/); // TODO
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
                    myIt->second.roi, 0 /*myCursor.intensity*/); // TODO
            AC_TRACE << "removing cursor "<<myIt->first<<" at " << myIt->second.correlatedPosition;
            _myCursors.erase(myIt);
        }
        myIt = nextIt;
    }
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
            myMinDistIt->second.major_direction = theCurrentPositions[i].major_dir;
            myMinDistIt->second.minor_direction = theCurrentPositions[i].minor_dir;

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
ASSDriver::drawGrid() {
    glColor4fv( _myGridColor.begin() );
    glBegin( GL_LINES );
    for (unsigned i = 0; i < _myGridSize[0]; ++i) {
        glVertex2f(float(i), 0.0);
        glVertex2f(float(i), float(_myGridSize[1] - 1));
    }
    for (unsigned i = 0; i < _myGridSize[1]; ++i) {
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

    // draw cursors
    CursorMap::iterator myIt = _myCursors.begin();
    for (; myIt != _myCursors.end(); ++myIt ) {
        drawCircle( myIt->second.position, 0.15f, 36, _myCursorColor );
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
    const Vector4f & myOldColor = theWindow->getTextColor();
    theWindow->setTextColor( _myTextColor );
    theWindow->renderText( Vector2f( 50, 50 ), theText, "Screen15");
    theWindow->setTextColor( myOldColor );
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
                    glLoadMatrixf( static_cast<const GLfloat*>( getTransformationMatrix().getData()));

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
#define GET_PROP( theName, theVar )    \
    if (thePropertyName == #theName) { \
        theReturnValue.set( theVar );  \
        return;                        \
    }
    
#define RO_PROP( theName ) \
    if (thePropertyName == #theName) { \
        AC_ERROR << "Property '" << #theName << "' is read only."; \
        return; \
    }

#define SET_PROP( theName, theVar ) \
    if (thePropertyName == #theName) { \
        thePropertyValue.get( theVar ); \
        return; \
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

    AC_ERROR << "Unknown property '" << thePropertyName << "'.";
}


void
ASSDriver::onUpdateSettings(dom::NodePtr theSettings) {

    _mySettings = getASSSettings( theSettings );
    setupDriver(theSettings);
}

void
ASSDriver::setupDriver(dom::NodePtr theSettings) {
    if (!theSettings) {
        throw ASSException(string("Sorry, setup of assdriver with invalid settings node "),
                PLUS_FILE_LINE);
    }
    if ( _myTransportLayer && _myTransportLayer->settingsChanged( theSettings ) ) {
        _myTransportLayer = TransportLayerPtr( 0 );
    }

    if ( ! _myTransportLayer ) {
        string myTransportName;
        getConfigSetting( theSettings, "TransportLayer", myTransportName, string("serial") );
        if ( myTransportName == "serial" ) {
            _myTransportLayer = TransportLayerPtr( new SerialTransport(theSettings) );
        } /*else if ( myTransportName == "udp") {
            TODO: UDP transport layer
        }*/ else {
            throw ASSException(string("Unknown transport layer '") + myTransportName + "'",
                    PLUS_FILE_LINE);
        }
    }

    getConfigSetting( theSettings, "ComponentThreshold", _myComponentThreshold, 5 );
    getConfigSetting( theSettings, "NoiseThreshold", _myNoiseThreshold, 15 );
    getConfigSetting( theSettings, "GainPower", _myGainPower, 2.0f );
    getConfigSetting( theSettings, "IntensityThreshold", _myIntensityThreshold, 9.0f );
    getConfigSetting( theSettings, "FirstDerivativeThreshold", _myFirstDerivativeThreshold, 25.0f );
    getConfigSetting( theSettings, "DebugTouchEvents", _myDebugTouchEventsFlag, 0 );
    getConfigSetting( theSettings, "ProbePosition", _myProbePosition, Vector2f( -1, -1) );
    getConfigSetting( theSettings, "MinTouchInterval", _myMinTouchInterval, 0.25 );
    getConfigSetting( theSettings, "ClampToScreen", _myClampToScreenFlag, 0);
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

void 
ASSDriver::processInput() {

    MAKE_SCOPE_TIMER(ASSDriver_processInput);

    if (_myTransportLayer) {
        _myTransportLayer->lockFrameQueue();

        std::queue<ASSEvent> & myFrameQueue = _myTransportLayer->getFrameQueue();

        AC_TRACE << "frame queue size = " << myFrameQueue.size();

        while ( ! myFrameQueue.empty() ) {
            ASSEvent myEvent = myFrameQueue.front();
            myFrameQueue.pop();

            AC_TRACE << "popped event " << myEvent;

            switch (myEvent.type) {
                case ASS_FRAME:
                    {
                        if (myEvent.size != _myGridSize ) {
                            allocateGridBuffers( myEvent.size );
                        }
                        copyFrame( myEvent.data );
                        // TODO use smart pointers 
                        delete [] myEvent.data;
                        processSensorValues();
                    }
                    break;
                case ASS_LOST_SYNC:
                    createTransportLayerEvent("lost_sync");
                    break;
                case ASS_LOST_COM:
                    createTransportLayerEvent("lost_communication");
                    break;
            }
        }
        _myTransportLayer->unlockFrameQueue();
        AC_TRACE << "frame queue size after unlocking = " << myFrameQueue.size();
    } else {
        AC_PRINT << "No TransportLayer";
    }
}

void
ASSDriver::copyFrame(unsigned char * theData ) {
    
    y60::RasterOfGRAY & myRaster = *
            dom::dynamic_cast_and_openWriteableValue<y60::RasterOfGRAY>(&* (_myRawRaster.value) );


    unsigned char * myDstPtr = _myRawRaster.raster->pixels().begin();
    unsigned char * mySrcPtr = theData;
    for (unsigned y = 0; y < _myGridSize[1]; ++y) {
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
    _myTransportLayer = TransportLayerPtr( 0 );
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
    MAKE_SCOPE_TIMER(ASSDriver_queueCommand);
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

