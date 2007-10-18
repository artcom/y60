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

#ifndef __VIDEOPROCESSING_H__
#define __VIDEOPROCESSING_H__

#include <asl/PlugInBase.h>
#include <y60/IRendererExtension.h>
#include <y60/IScriptablePlugin.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/AbstractRenderWindow.h>
#include "Algorithm.h"


#include <y60/JSWrapper.h>
#include <y60/JSNode.h>
#include <y60/JSScene.h>
#include <y60/Scene.h>

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
                    AC_PRINT << "New Settings: " << *theSettings;
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
