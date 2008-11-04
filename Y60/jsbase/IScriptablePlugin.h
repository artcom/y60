//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_jslib_IScriptablePlugin_h_
#define _ac_jslib_IScriptablePlugin_h_

#include "PropertyValue.h"
#include <js/spidermonkey/jsapi.h>
#include <asl/base/Ptr.h>
#include <asl/dom/Nodes.h>

struct JSContext;
struct JSObject;

namespace jslib {

    struct IScriptablePlugin {
        virtual ~IScriptablePlugin() {}
        virtual void initClasses(JSContext * theContext, JSObject *theGlobalObject);


        virtual JSFunctionSpec * StaticFunctions();

        virtual JSFunctionSpec * Functions();
        virtual JSConstIntPropertySpec * ConstIntProperties();

        /**
         * Return the name under which the class is visible in js.
         * @note return a static pointer please
         */
        virtual const char * ClassName() = 0; // UH make this const?

        /**
         * Called when a property is read in js.
         * @param thePropertyName Name of the property.
         * @param thePropertyValue Return value of the property.
         */
        virtual void onGetProperty(const std::string & thePropertyName,
                                   y60::PropertyValue & theReturnValue) const {
        }

        /**
         * Called when a property is set in js.
         * @param thePropertyName Name of the property that is set.
         * @param thePropertyValue New value of the property.
         */
        virtual void onSetProperty(const std::string & thePropertyName,
                                   const y60::PropertyValue & thePropertyValue) {
        }

        /**
         * Function will be called when some settings are changed.
         * @param theSettings Settings node.
         * @see js/Configurator.js
         */
        virtual void onUpdateSettings(dom::NodePtr theSettings) {
        }

        /**
         * Get named attribute from settings node. Returns default on error.
         * @param theSettings Settings node.
         * @param theSettingName Name of the setting to fetch.
         * @param theDefault Default value in case an error occurs.
         * @return value of attribute or default.
         */
        template <class T>
        T getSetting(const dom::NodePtr & theSettings,
                     const std::string & theSettingName, T theDefault);
    };


    typedef asl::Ptr<IScriptablePlugin> IScriptablePluginPtr;
}
#endif
