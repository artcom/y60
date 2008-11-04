//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "IScriptablePlugin.h"
#include "IScriptablePlugin.impl"
#include "JSScriptablePlugin.h"

namespace jslib {

    template
    unsigned IScriptablePlugin::getSetting(const dom::NodePtr & theSettings,
            const std::string & theSettingName, unsigned theDefault);
    template
    int IScriptablePlugin::getSetting(const dom::NodePtr & theSettings,
            const std::string & theSettingName, int theDefault);
    template
    bool IScriptablePlugin::getSetting(const dom::NodePtr & theSettings,
            const std::string & theSettingName, bool theDefault);
    template
    asl::Vector2f IScriptablePlugin::getSetting(const dom::NodePtr & theSettings,
            const std::string & theSettingName, asl::Vector2f theDefault);
    template
    asl::Vector3f IScriptablePlugin::getSetting(const dom::NodePtr & theSettings,
            const std::string & theSettingName, asl::Vector3f theDefault);
    template
    asl::Vector4f IScriptablePlugin::getSetting(const dom::NodePtr & theSettings,
            const std::string & theSettingName, asl::Vector4f theDefault);
    template
    float IScriptablePlugin::getSetting(const dom::NodePtr & theSettings,
            const std::string & theSettingName, float theDefault);

    JSFunctionSpec * 
    IScriptablePlugin::Functions() { 
        return 0;
    }
    
    JSFunctionSpec * 
    IScriptablePlugin::StaticFunctions() { 
        return 0;
    }
    
    JSConstIntPropertySpec * 
    IScriptablePlugin::ConstIntProperties() {
        return 0;
    }

    void
    IScriptablePlugin::initClasses(JSContext * theContext, JSObject *theGlobalObject) {
	    JSScriptablePlugin::initClass(theContext, theGlobalObject, ClassName(),
                Functions(), StaticFunctions(), ConstIntProperties());
    }
}

