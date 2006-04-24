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

#include "ShotDetection.h"
#include "ColorDetection.h"
#include "Algorithm.h"

#include <y60/JSNode.h>
#include <asl/PlugInBase.h>
#include <y60/IRendererExtension.h>
#include <y60/IScriptablePlugin.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/Scene.h>

#include <asl/string_functions.h>
#include <asl/Exception.h>
#include <asl/Logger.h>

#include <iostream>

using namespace std;
using namespace jslib;
using namespace dom;

namespace y60 {

	/**********************************************************/
	// Register your Algorithm class here
	/**********************************************************/
	#define REGISTER_ALGORITHM(theAlgorithmClass) \
		if (theName == theAlgorithmClass::getName()) { \
			return asl::Ptr<Algorithm>(new theAlgorithmClass); \
		}

	AlgorithmPtr createAlgorithm(const string & theName) {
		REGISTER_ALGORITHM(ColorDetection);
		REGISTER_ALGORITHM(TestAlgorithm);
		REGISTER_ALGORITHM(ShotDetectionAlgorithm);

		AC_WARNING << "algorithm '" << theName << "' not found.";
		return asl::Ptr<Algorithm>(0);
	}

	/**********************************************************/

	class VideoProcessingExtension :
		public asl::PlugInBase,
		public y60::IRendererExtension,
		public IScriptablePlugin
	{
		public:
			VideoProcessingExtension(asl::DLHandle theDLHandle);

			void onStartup(jslib::AbstractRenderWindow * theWindow) {};
			bool onSceneLoaded(jslib::AbstractRenderWindow *) {
				return true;
			}
			void handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {};
			void onFrame(AbstractRenderWindow * theWindow , double t);

			void onGetProperty(const std::string & thePropertyName, 
					PropertyValue & theReturnValue) const;
			void onSetProperty(const std::string & thePropertyName, 
					const PropertyValue & thePropertyValue);

			void onPreRender(AbstractRenderWindow * theRenderer) {};
			void onPostRender(AbstractRenderWindow * theRenderer) {};

			const char * ClassName() {
				static const char * myClassName = "VideoProcessingExtension";
				return myClassName;
			}

			void onUpdateSettings(dom::NodePtr theSettings) {
				AC_PRINT << "New Settings: " << *theSettings << endl;
			}

		private:
			ImagePtr _myImage;
			AlgorithmPtr _myAlgorithm;
	};

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
		} else if (thePropertyName == "sourceimage") {
			dom::NodePtr myImageNode = thePropertyValue.get<dom::NodePtr>();
			try {
				_myImage = myImageNode->getFacade<Image>();
				AC_PRINT << "### VideoProcessing " << _myImage->get<ImageSourceTag>();
			} catch (Exception ex) {
				AC_ERROR << ex;
			}
		}
	}

	void
	VideoProcessingExtension::onFrame(AbstractRenderWindow * theWindow , double t) {
		if (_myAlgorithm && _myImage) {
			const BGRRaster * myRaster = dynamic_cast_Value<BGRRaster>(&*_myImage->getRasterValue());
	                      
			if (myRaster) { 
				double myMovieTime = 0; //_myImage->get<CurrentFrameTag>() / _myImage->get<FrameRateTag>();
				_myAlgorithm->onFrame(*myRaster, myMovieTime);
			} else {
				AC_ERROR << "null ptr for raster BGR " << myRaster;
			}

		}
	}

	extern "C"
	EXPORT asl::PlugInBase* VideoProcessingExtension_instantiatePlugIn(asl::DLHandle myDLHandle) {
		return new VideoProcessingExtension(myDLHandle);
	}
}