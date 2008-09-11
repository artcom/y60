#ifndef _IJEXTENSION_INCLUDED
#define _IJEXTENSION_INCLUDED

#include <asl/base/Ptr.h>
#include <y60/jsbase/PropertyValue.h>

struct JSContext;
struct JSObject;

namespace jslib {

struct IScriptablePlugin {
    virtual ~IScriptablePlugin() {}
	virtual void initClasses(JSContext * theContext, JSObject *theGlobalObject) = 0;

    // Function will be called if a property is read in js
	virtual void onGetProperty(const std::string & thePropertyName,
		y60::PropertyValue & theReturnValue) const
    {}

	// Function will be called if a property is set in js
    virtual void onSetProperty(const std::string & thePropertyName,
		const y60::PropertyValue & thePropertyValue)
    {}

	// Function will be called if the settings.xml file changed (s. Configurator.js)
	virtual void onUpdateSettings(dom::NodePtr theConfiguration) {};
};

typedef asl::Ptr<IScriptablePlugin> IScriptablePluginPtr;

}
#endif
