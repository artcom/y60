//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: VideoProcessingExtension.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.15 $
//
// Description:
//
//=============================================================================

#include <y60/JSNode.h>
#include <asl/PlugInBase.h>
#include <y60/IRendererExtension.h>
#include <y60/IScriptablePlugin.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/Scene.h>
#include <y60/Movie.h>

#include <asl/string_functions.h>
#include <asl/Exception.h>
#include <asl/Logger.h>

#include <iostream>

using namespace std;
//using namespace asl;
using namespace jslib;
using namespace dom;
using namespace y60;

typedef asl::raster<asl::BGR>  video_frame;


/**
 * @ingroup Y60video
 * video processing interface class 
 * implement onFrame for processing a frame
 * input parameters are handled by configure()
 * ouput is handled by result()
 */
class Algorithm {
public:
    virtual void configure(const Node & theNode) { 
        AC_WARNING << "Algorithm::configure not implemented. " << theNode << endl;
    }
    virtual const Node & result() const { 
        AC_WARNING << "Algorithm::result not implemented." << endl;
        static Node dummy;
        return dummy;
    }
    virtual void onFrame(const video_frame & theFrame, double t) = 0;
};

typedef asl::Ptr<Algorithm> AlgorithmPtr;

/**********************************************************/
//demo algorithm

class TestAlgorithm : public Algorithm {
public:
    static string getName() { return "test"; }

    void onFrame(const video_frame & theFrame, double t) {
        int x = 0, y = 0;
        cout << "raster size " << theFrame.xsize() << "x" << theFrame.ysize() 
            << " value " << x << "," << y << " = " 
            << int( getRedValue(theFrame(x,y)) ) << "," 
            << int( getGreenValue(theFrame(x,y)) )<< ","   
            << int( getBlueValue(theFrame(x,y)) ) 
            << endl;
    }
};

/**********************************************************/

/**
 * @ingroup Y60video
 * a rudimentary shot-detection algorithm 
 * based on color histogram differences between consecutive frames
 */
class ShotDetectionAlgorithm : public Algorithm {
    static const int BINS_PER_CHANNEL = 5;
    static const int NBINS = BINS_PER_CHANNEL*BINS_PER_CHANNEL*BINS_PER_CHANNEL;
    unsigned long _myHistogram[2][NBINS];
    double _myThreshold;
    double _myLastShotTime, _myMinimalShotLength;
    Element _myResultNode;

    void clearHistogram(int theIndex) {
        for(int b = 0; b < NBINS; ++b) {
            _myHistogram[theIndex][b] = 0;
        }
    }
    
    unsigned long intersectHistograms() {
        unsigned long myTotal = 0;
        for(int b = 0; b < NBINS; ++b) {
            myTotal += std::min(_myHistogram[0][b], _myHistogram[1][b]);
        }
        return myTotal;
    }
        
    //debug
    void printHistogram(int theIndex) {
        for(int b = 0; b < NBINS; ++b) {
            cout << _myHistogram[theIndex][b] << " ";
        }
        cout << endl;
    }

    void addHistogram(int theIndex, Node & theNode) {
        y60::VectorOfUnsignedInt myHistogram(_myHistogram[theIndex], 
                                             _myHistogram[theIndex] + NBINS);
        theNode("histogram").appendChild( dom::Text(asl::as_string(myHistogram)) );
    }
    
public:
    ShotDetectionAlgorithm() : _myResultNode("shots") {
        _myThreshold = 0.9;
        _myMinimalShotLength = 1.0;
        _myLastShotTime = 0;
        clearHistogram(0);
        clearHistogram(1);
    }

    static string getName() { return "shot_detection"; }
       
    void onFrame(const video_frame & theFrame, double theTime) {
        static int n = 0;

        //clear current histogram
        clearHistogram(n);
        
        //compute histogram for current frame 
        int myScaled;
        int myFactors[] = {1, BINS_PER_CHANNEL, BINS_PER_CHANNEL*BINS_PER_CHANNEL };
        video_frame::const_iterator it;
        for(it = theFrame.begin(); it != theFrame.end(); ++it) {
            int myBin = 0;
            for(int c = 0;c < 3; ++c) {
                myScaled = (BINS_PER_CHANNEL * int((*it)[c])) / 256;
                myBin += myFactors[c] * myScaled;
            }
            
            /*
                AC_ERROR << myBin 
                    << " color " << int((*it)[0]) << "," 
                    << int((*it)[1]) << "," << int((*it)[2]);
            */
            _myHistogram[n][myBin]++;
        }
        
        if (_myResultNode.childNodesLength() == 0 && theTime > 0.5) {
            //wait some time before taking the first shot
            Element myFirstShotNode("shot");
            myFirstShotNode["start"] = asl::as_string(0);
            addHistogram(n, myFirstShotNode);
            _myResultNode.appendChild(myFirstShotNode);
        }

        //printHistogram(n);

        //compute difference to histogram of last frame
        float myDifference = float(intersectHistograms())/theFrame.size();
        float myLength = theTime - _myLastShotTime;
        
        if (_myResultNode.childNodesLength() > 0) {
            //update length of current shot node
            NodePtr myCurrentShot 
                = _myResultNode.childNode(_myResultNode.childNodesLength()-1);
            (*myCurrentShot)["length"] = asl::as_string(myLength);
        }

        AC_DEBUG << "************** diff at " << theTime << " is " << myDifference; 
        if (myDifference < _myThreshold) {
            //we have a potential shot
            
            if (myLength > _myMinimalShotLength) {

                //start a new shot node
                Element myShotNode("shot");
                myShotNode["start"] = asl::as_string(theTime);
                myShotNode["length"] = asl::as_string(0);

                addHistogram(n, myShotNode);
                _myResultNode.appendChild(myShotNode);                

                AC_INFO << "SHOT at " << theTime << " last shot length " << myLength 
                        << " (" << myDifference << ")";
            }
            _myLastShotTime = theTime;
        }

        //toggle current histogram
        n = 1 - n;
    }
    void configure(const Node & theNode) { 
        try {
            asl::fromString(theNode["threshold"].nodeValue(), _myThreshold);
            asl::fromString(theNode["minimal_length"].nodeValue(), _myMinimalShotLength);
        } catch(asl::Exception ex) {
            AC_ERROR << "could not parse configuration " << theNode;
        }
        AC_PRINT << "ShotDetection::configure threshold " << _myThreshold;
        AC_PRINT << "ShotDetection::configure min. shot length " << _myMinimalShotLength;
    }
    const Node & result() const { 
        return _myResultNode;
    }
};

/**********************************************************/
//register your Algorithm class here

#define REGISTER_ALGORITHM(theAlgorithmClass) \
    if (theName == theAlgorithmClass::getName()) { \
        return asl::Ptr<Algorithm>(new theAlgorithmClass); \
    }

AlgorithmPtr createAlgorithm(const string & theName) {
    REGISTER_ALGORITHM(TestAlgorithm);
    REGISTER_ALGORITHM(ShotDetectionAlgorithm);

    AC_WARNING << "algorithm '" << theName << "' not found.";
    return asl::Ptr<Algorithm>(0);
}


/**********************************************************/
//the actual plugin


class VideoProcessingExtension :
    public asl::PlugInBase,
    public y60::IRendererExtension,
    public IScriptablePlugin
{
    public:

        VideoProcessingExtension(asl::DLHandle theDLHandle);

        void onStartup(jslib::AbstractRenderWindow * theWindow);

        bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow);

        void handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent);
        void onFrame(AbstractRenderWindow * theWindow , double t);

        void onGetProperty(const std::string & thePropertyName, 
                PropertyValue & theReturnValue) const;
        void onSetProperty(const std::string & thePropertyName, 
                const PropertyValue & thePropertyValue);

        void onPreRender(AbstractRenderWindow * theRenderer);
        void onPostRender(AbstractRenderWindow * theRenderer);

        const char * ClassName() {
            static const char * myClassName = "VideoProcessingExtension";
            return myClassName;
        }

        void onUpdateSettings(dom::NodePtr theSettings) {
            AC_PRINT << "New Settings: " << *theSettings << endl;
        }

    private:
        NodePtr _myMovieNode;
        MoviePtr _myMovie;
        AlgorithmPtr _myAlgorithm;
};

VideoProcessingExtension :: VideoProcessingExtension(asl::DLHandle theDLHandle) :
    asl::PlugInBase(theDLHandle),
    IRendererExtension("VideoProcessingExtension")
{}

void
VideoProcessingExtension :: onStartup(jslib::AbstractRenderWindow * theWindow) {
}

void
VideoProcessingExtension::onGetProperty(const std::string & thePropertyName,
        PropertyValue & theReturnValue) const
{
    AC_DEBUG << "onGetProperty " << thePropertyName;
    if (thePropertyName == "result") {
        if ( ! _myAlgorithm) {
            AC_WARNING << "no algorithm set but asked for result. ignored. ";
            return;
        }
        
        AC_DEBUG << "result " << _myAlgorithm->result();
        //be cool & lazy and return a copy 
        NodePtr myResult = NodePtr(new Node(_myAlgorithm->result())); 
        theReturnValue.set<NodePtr>(myResult);
    }
}

void
VideoProcessingExtension::onSetProperty(const std::string & thePropertyName,
        const PropertyValue & thePropertyValue)
{
    AC_DEBUG << "onSetProperty " << thePropertyName;
    
    if (thePropertyName == "algorithm") {
        _myAlgorithm = createAlgorithm(thePropertyValue.get<string>()); 

    } else if (thePropertyName == "configuration") {
        if ( ! _myAlgorithm) {
            AC_WARNING << "no algorithm set but got a configuration. ignored. " 
                       << thePropertyValue.get<dom::NodePtr>();
            return;
        }
        _myAlgorithm->configure(*thePropertyValue.get<dom::NodePtr>()); 
    }
}

bool
VideoProcessingExtension::onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
    y60::ScenePtr myScene;
    myScene = theWindow->getRenderer()->getCurrentScene();
    _myMovieNode = myScene->getImagesRoot()->childNode("movie");
    if (!_myMovieNode) {
        AC_ERROR << "onSceneLoaded: no video found";
        return false;
    }
        
    try {
        _myMovie = _myMovieNode->getFacade<Movie>();
    } catch (Exception ex) {
        AC_ERROR << ex;
        return false;
    }
    
    AC_PRINT << "### VideoProcessing " << _myMovie->get<ImageSourceTag>();

    return true;
}

void
VideoProcessingExtension::handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {
}


void
VideoProcessingExtension::onFrame(AbstractRenderWindow * theWindow , double t) {
    if (_myAlgorithm) {
        const video_frame * myRaster = dynamic_cast_Value<video_frame>(&*_myMovie->getRasterValue());
                      
       if (myRaster) { 
            double myMovieTime = _myMovie->get<CurrentFrameTag>() 
                               / _myMovie->get<FrameRateTag>();
            _myAlgorithm->onFrame(*myRaster, myMovieTime);
        } else {
            AC_ERROR << "null ptr for raster BGR " << myRaster;
        }

    }
}

void
VideoProcessingExtension::onPreRender(AbstractRenderWindow * theRenderer) {
}

void
VideoProcessingExtension::onPostRender(AbstractRenderWindow * theRenderer) {
}

extern "C"
EXPORT asl::PlugInBase* VideoProcessingExtension_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new VideoProcessingExtension(myDLHandle);
}
