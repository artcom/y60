//============================================================================
//
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: SoundLightUSBDMX.h,v $
//     $Author: valentin $
//   $Revision: 1.3 $
//
//=============================================================================

#ifndef AC_SOUND_LIGHT_USB_DMX_INCLUDED
#define AC_SOUND_LIGHT_USB_DMX_INCLUDED

#include <asl/PlugInBase.h>
#include <y60/IScriptablePlugin.h>
#include <y60/JSScriptablePlugin.h>

class SoundLightUSBDMX : public asl::PlugInBase, public jslib::IScriptablePlugin
{
public:
	SoundLightUSBDMX(asl::DLHandle theHandle);
	virtual ~SoundLightUSBDMX();

    // IScriptablePlugin
    void onUpdateSettings(dom::NodePtr theConfiguration);

    void onGetProperty(const std::string & thePropertyName,
					   y60::PropertyValue & theReturnValue) const;

    void onSetProperty(const std::string & thePropertyName,
		               const y60::PropertyValue & thePropertyValue);

    const char * ClassName() {
        return "SoundLightUSBDMX";
    }

    void initClasses(JSContext * theContext, JSObject * theGlobalObject) {
        jslib::JSScriptablePlugin::initClass(theContext, theGlobalObject, ClassName());
    }

private:
	HINSTANCE _myDllHandle;

	typedef int (*DASHARDCOMMAND)(int, int, unsigned char*);
	DASHARDCOMMAND _myUsbCommand;

	static const unsigned MAX_DMX_CHANNELS;
	unsigned char* _myUniverse;

	int DMXCommand(int theCommand, int theParam, unsigned char* theData);
};
#endif // AC_SOUND_LIGHT_USB_DMX_INCLUDED
