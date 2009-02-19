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
// Description: TODO  
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

#include "VideoProcessing.h" 

#include "ShotDetection.h"
#include "ColorDetection.h"
#include "Histogram.h"
#include "BackgroundSubtraction.h"
#include "FastBlur.h"
#include "Contrast.h"
#include "Blobs.h"
#include "AdaptiveBackground.h"
#include "Mask.h"
#include "DumbScale.h"
#include "Convert.h"
#include "Canny.h"
#include "MaskPerson.h"

#include <asl/base/Exception.h>
#include <asl/base/Logger.h>

#include <iostream>

using namespace std;
using namespace jslib;
using namespace dom;
using namespace y60;
using namespace asl;

namespace y60 {

    /**********************************************************/
    // Register your Algorithm class here
    /**********************************************************/
#define REGISTER_ALGORITHM(theAlgorithmClass)               \
    if (theName == theAlgorithmClass::getName()) {          \
    return asl::Ptr<Algorithm>(new theAlgorithmClass(theName)); \
    }

    AlgorithmPtr createAlgorithm(const string & theName) {
        REGISTER_ALGORITHM(ColorDetection);
        REGISTER_ALGORITHM(Histogram);
        REGISTER_ALGORITHM(TestAlgorithm);
        REGISTER_ALGORITHM(ShotDetectionAlgorithm);
        REGISTER_ALGORITHM(BackgroundSubtraction); 
        REGISTER_ALGORITHM(FastBlur); 
        REGISTER_ALGORITHM(Contrast);
        REGISTER_ALGORITHM(Blobs);
        REGISTER_ALGORITHM(AdaptiveBackground);
        REGISTER_ALGORITHM(Mask);
        REGISTER_ALGORITHM(DumbScale);
        REGISTER_ALGORITHM(Convert);
        REGISTER_ALGORITHM(Canny);
        REGISTER_ALGORITHM(MaskPerson);

        AC_WARNING << "algorithm '" << theName << "' not found.";
        return asl::Ptr<Algorithm>();
    }

    /**********************************************************/


    VideoProcessingExtension :: VideoProcessingExtension(asl::DLHandle theDLHandle) :
    asl::PlugInBase(theDLHandle),
        IRendererExtension("VideoProcessingExtension")
    {}

    void
        VideoProcessingExtension::onGetProperty(const std::string & thePropertyName,
        PropertyValue & theReturnValue) const
    {
        AC_DEBUG << "onGetProperty " << thePropertyName;
        if (thePropertyName == "result") {           
            dom::Element myResultsNode("results");      
            for (AlgorithmList::const_iterator it = _myAlgorithmList.begin(); it != _myAlgorithmList.end(); ++it) {
                dom::NodePtr myResultNode = myResultsNode.appendChild((*it)->result());
                myResultNode->appendAttribute("name", (*it)->getAlgorithmName());    
            }
            //be cool & lazy and return a copy 
            theReturnValue.set<NodePtr>(NodePtr(new Node(myResultsNode)));
        }
    }

    void 
        VideoProcessingExtension::init(y60::ScenePtr theScene) {
            _myScene = theScene;
            _myAlgorithmList.clear();
    }   

    void
        VideoProcessingExtension::onSetProperty(const std::string & thePropertyName,
        const PropertyValue & thePropertyValue)
    {
        AC_INFO << "onSetProperty " << thePropertyName;

        if (thePropertyName == "configuration") {
            dom::Node myConfig = *thePropertyValue.get<dom::NodePtr>();           
            for( unsigned int i=0; i<myConfig.childNodesLength(); i++)  {   
                AC_INFO << "config " << myConfig.childNode("algorithm", i)->getAttribute("name")->nodeValue();
                dom::NodePtr myAlgorithmNode = myConfig.childNode("algorithm", i);
                const std::string myAlgorithmName = myAlgorithmNode->getAttribute("name")->nodeValue();

                AlgorithmPtr myAlgorithm = createAlgorithm(myAlgorithmName); 
                if( myAlgorithm ) {
                    myAlgorithm->setScene(_myScene);
                    myAlgorithm->configure(*myAlgorithmNode); 
                } else {
                    AC_WARNING << "no algorithm set but got a configuration. ignored. " 
                        << thePropertyValue.get<dom::NodePtr>();
                    return;     
                }
                _myAlgorithmList.push_back(myAlgorithm);
            }                      
            AC_INFO << "videoprocessingextension :: configuration se prop " << _myScene;
        }    
    }           

    void    
        VideoProcessingExtension::onFrame(AbstractRenderWindow * theWindow , double t) {
            if ( _myAlgorithmList.size() > 0 ) {
                for (AlgorithmList::iterator it = _myAlgorithmList.begin(); it != _myAlgorithmList.end(); ++it) {
                    (*it)->onFrame(t);
                }
            }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // JavaScript binding

    static JSBool Init(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, 
        jsval* rval) 
    {           
        DOC_BEGIN("");
        DOC_END;

        ensureParamCount(argc, 1);

        y60::ScenePtr myScene;
        if (!convertFrom(cx, argv[0], myScene)) {
            JS_ReportError(cx, "VideoProcessingExtension::init: argument #0 must be a Scene");
            return JS_FALSE;
        }
        VideoProcessingExtensionPtr myNative = getNativeAs<VideoProcessingExtension>(cx, obj);
        myNative->init(myScene);
        return JS_TRUE;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // IScriptablePlugin

    JSFunctionSpec * VideoProcessingExtension::Functions() {
        static JSFunctionSpec functions[] = {   
            {"init", Init, 1},  
            {0} 
        };  
        return functions;
    }   


    JSFunctionSpec * VideoProcessingExtension::StaticFunctions() {
        static JSFunctionSpec functions[] = {
            {0}
        };
        return functions;
    }


    extern "C"
        EXPORT asl::PlugInBase* y60VideoProcessing_instantiatePlugIn(asl::DLHandle myDLHandle) {
            return new VideoProcessingExtension(myDLHandle);
    }
}
