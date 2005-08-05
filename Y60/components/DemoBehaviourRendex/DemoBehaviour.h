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
//    $RCSfile: DemoBehaviour.h,v $
//
//     $Author: martin $
//
//   $Revision: 1.7 $
//
// Description:
//
//=============================================================================

#ifndef _ac_y60_DemoBehaviour_h_
#define _ac_y60_DemoBehaviour_h_

#include <y60/JSNode.h>
#include <y60/JSVector.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/IRendererExtension.h>
#include <y60/IFactoryPlugin.h>
#include <y60/Body.h>
#include <y60/JSVector.h>
#include <asl/PlugInBase.h>

namespace y60 {

	class DemoBehaviour : public jslib::IScriptablePlugin, public IRendererExtension  {
        public:
			DemoBehaviour();
			void onUpdateSettings(dom::NodePtr theConfiguration);

			void onStartup(jslib::AbstractRenderWindow * theWindow) {}
			bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow) { return false; }

			void handle(jslib::AbstractRenderWindow * theWindow, y60::EventPtr theEvent);
			void onFrame(jslib::AbstractRenderWindow * theWindow, double theTime);

			void onPreRender(jslib::AbstractRenderWindow * theRenderer) {}
			void onPostRender(jslib::AbstractRenderWindow * theRenderer) {}

            void onGetProperty(const std::string & thePropertyName,
                             PropertyValue & theReturnValue) const;

            void onSetProperty(const std::string & thePropertyName,
                             const PropertyValue & thePropertyValue);

			const char * ClassName() {
			    static const char * myClassName = "DemoBehaviour";
				return myClassName;
			}

        private:
            void onKey(y60::Event & theEvent);
            void onMouseButton(y60::Event & theEvent);

            BodyPtr       _myBody;
            asl::Vector2i _myPosition;
            std::string   _myState;
    };

	class DemoBehaviourFactory : public asl::PlugInBase, public jslib::IFactoryPlugin {
		public:
			DemoBehaviourFactory(asl::DLHandle theDLHandle) :
				asl::PlugInBase(theDLHandle)
			{}

			void initClasses(JSContext * theContext, JSObject * theGlobalObject) {
				jslib::JSScriptablePlugin::initClass(theContext, theGlobalObject, ClassName());
			}

			const char * ClassName() {
			    static const char * myClassName = "DemoBehaviour";
				return myClassName;
			}

			jslib::IScriptablePluginPtr createInstance() {
				return jslib::IScriptablePluginPtr(new DemoBehaviour());
			}
	  };
}

#endif
