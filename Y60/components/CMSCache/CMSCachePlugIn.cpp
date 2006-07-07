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


#include "JSCMSPackage.h"

#include <y60/IScriptablePlugin.h>
#include <asl/PlugInBase.h>

namespace y60 {
	class CMSPackagePlugIn :
		public asl::PlugInBase,
        public jslib::IScriptablePlugin
	{
	public:
		CMSPackagePlugIn (asl::DLHandle theDLHandle);
		virtual ~CMSPackagePlugIn();

   		void initClasses(JSContext * theContext, JSObject *theGlobalObject);
        const char * ClassName() {
            static const char * myClassName = "CMSPackagePlugIn";
            return myClassName;
        }

	};

}

#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;

CMSPackagePlugIn :: CMSPackagePlugIn(DLHandle theDLHandle) :
			PlugInBase(theDLHandle)
{
    //DJDecoderRegistration::registerCodecs();
}

CMSPackagePlugIn :: ~CMSPackagePlugIn() {
    //DJDecoderRegistration::cleanup();
}

void 
CMSPackagePlugIn::initClasses(JSContext * theContext, JSObject *theGlobalObject) {
    jslib::JSCMSPackage::initClass(theContext, theGlobalObject);
}



extern "C"
EXPORT PlugInBase* y60CMSPackage_instantiatePlugIn(DLHandle myDLHandle) {
	return new y60::CMSPackagePlugIn(myDLHandle);
}
