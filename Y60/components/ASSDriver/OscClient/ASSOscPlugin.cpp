//============================================================================
//
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "JSASSOscClient.h"

#include <y60/jsbase/IScriptablePlugin.h>
#include <asl/base/PlugInBase.h>

namespace y60 {

class ASSOscPlugin :
    public asl::PlugInBase,
    public jslib::IScriptablePlugin
{
    public:
        ASSOscPlugin (asl::DLHandle theDLHandle);
        virtual ~ASSOscPlugin();

        void initClasses(JSContext * theContext, JSObject *theGlobalObject);
        const char * ClassName() {
            static const char * myClassName = "ASSOscPlugin";
            return myClassName;
        }

};

} // end of namespace y60

using namespace std;
using namespace asl;

namespace y60 {

ASSOscPlugin::ASSOscPlugin(DLHandle theDLHandle) :
            PlugInBase(theDLHandle)
{
}

ASSOscPlugin :: ~ASSOscPlugin() {
}

void 
ASSOscPlugin::initClasses(JSContext * theContext, JSObject *theGlobalObject) {
    jslib::JSASSOscClient::initClass(theContext, theGlobalObject);
}


} // end of namespace y60

extern "C"
EXPORT PlugInBase * ASSOscPlugin_instantiatePlugIn(DLHandle myDLHandle) {
    return new y60::ASSOscPlugin(myDLHandle);
}
