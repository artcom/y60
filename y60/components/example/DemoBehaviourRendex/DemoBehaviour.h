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

#include "y60_demobehaviourrendex_settings.h"

#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSVector.h>
#include <y60/jsbase/JSScriptablePlugin.h>
#include <y60/jslib/IRendererExtension.h>
#include <y60/jsbase/IFactoryPlugin.h>
#include <y60/scene/Body.h>
#include <y60/jsbase/JSVector.h>
#include <asl/base/PlugInBase.h>

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
