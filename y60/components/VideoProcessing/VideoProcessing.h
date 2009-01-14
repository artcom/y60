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

#ifndef __VIDEOPROCESSING_H__
#define __VIDEOPROCESSING_H__

#include "y60_videoprocessing_settings.h"

#include <asl/base/PlugInBase.h>
#include <y60/jslib/IRendererExtension.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jsbase/JSScriptablePlugin.h>
#include <y60/jslib/AbstractRenderWindow.h>
#include "Algorithm.h"


#include <y60/jsbase/JSWrapper.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jslib/JSScene.h>
#include <y60/scene/Scene.h>

#include <vector>

namespace y60 {


	class VideoProcessingExtension :
        public asl::PlugInBase,
		public y60::IRendererExtension,
        public jslib::IScriptablePlugin 
            {
           
            public:
                // VideoProcessingExtension();
                // virtual ~VideoProcessingExtension();

                VideoProcessingExtension(asl::DLHandle theDLHandle);
             
                void init(y60::ScenePtr theScene);
                void onStartup(jslib::AbstractRenderWindow * theWindow) {};
                bool onSceneLoaded(jslib::AbstractRenderWindow *) {
                    return true;
                }
                void handle(jslib::AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {};
                void onFrame(jslib::AbstractRenderWindow * theWindow , double t);

                void onGetProperty(const std::string & thePropertyName, 
                                   PropertyValue & theReturnValue) const;
                void onSetProperty(const std::string & thePropertyName, 
                                   const PropertyValue & thePropertyValue);

                void onPreRender(jslib::AbstractRenderWindow * theRenderer) {};
                void onPostRender(jslib::AbstractRenderWindow * theRenderer) {};
        
                typedef VideoProcessingExtension NATIVE;
                typedef asl::Ptr<NATIVE> OWNERPTR;
                typedef jslib::JSWrapper<NATIVE, OWNERPTR,
                    jslib::StaticAccessProtocol> Base;


                const char * ClassName() {
                    static const char * myClassName = "VideoProcessingExtension";
                    return myClassName;
                }
        
             
                virtual JSFunctionSpec* StaticFunctions();
                virtual JSFunctionSpec* Functions();

                void onUpdateSettings(dom::NodePtr theSettings) {
                    AC_INFO << "New Settings: " << *theSettings;
                }

            private:
                AlgorithmPtr _myAlgorithm;
                typedef std::vector<AlgorithmPtr> AlgorithmList;
                AlgorithmList  _myAlgorithmList;
                y60::ScenePtr _myScene;
                RendererPtr _myRenderer;
                dom::Node _myResultsNode;
            };
    
    typedef asl::Ptr<VideoProcessingExtension> VideoProcessingExtensionPtr;
} // end namspace


#endif
