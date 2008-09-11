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
//    $RCSfile: DICOMPlugIn.cpp,v $
//
//     $Author: martin $
//
//   $Revision: 1.3 $
//
// Description:
//
//=============================================================================

// #define PL_DECODER

#include "JSDicomImage.h"

#include <y60/jsbase/IScriptablePlugin.h>
#include <asl/base/PlugInBase.h>

#ifdef PL_DECODER
#include "pldicomdec.h"
#include <y60/image/IPaintLibExtension.h>
#include <paintlib/pldecoderfactory.h>
#endif

namespace y60 {
	class DICOMPlugIn :
		public asl::PlugInBase,
#ifdef PL_DECOCDER    
        public IPaintLibExtension,
        public ::PLDecoderFactory
#endif    
        public jslib::IScriptablePlugin
	{
	public:
		DICOMPlugIn (asl::DLHandle theDLHandle);
#ifdef PL_DECOCDER        
		PLDecoderFactory * getDecoderFactory() const;
        ::PLPicDecoder * CreateDecoder() const;
        bool CanDecode(PLBYTE * pData, int DataLen) const;
#endif        
   		void initClasses(JSContext * theContext, JSObject *theGlobalObject);
        const char * ClassName() {
            static const char * myClassName = "Dicom";
            return myClassName;
        }

	};

}

#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;

DICOMPlugIn :: DICOMPlugIn(DLHandle theDLHandle) :
			PlugInBase(theDLHandle)
{}

#ifdef PL_DECOCDER
PLDecoderFactory * DICOMPlugIn :: getDecoderFactory() const {
    return const_cast<DICOMPlugIn*>(this);
}

PLPicDecoder * DICOMPlugIn :: CreateDecoder() const {
    return new ::PLDICOMDecoder;
}

bool DICOMPlugIn :: CanDecode(PLBYTE * pData, int DataLen) const {
	bool ret;
    if (DataLen>128+4) {
		ret = !strncmp((char*)pData + 128, "DICM", 4);
        if (!ret) {
            cerr << "DICOMPlugIn :: CanDecode: DICOM signature missing. Not a valid DICOm file" << endl;
        } else {
            cerr << "DICOMPlugIn :: CanDecode: found DICM signature." << endl;     
        }
    } else {
        cerr << "DICOMPlugIn :: CanDecode: File is too small to be a valid DICOM file" << endl;
		ret = false;
	}
	return ret;
}
#endif

void 
DICOMPlugIn::initClasses(JSContext * theContext, JSObject *theGlobalObject) {
    JSDicomImage::initClass(theContext, theGlobalObject);
}



extern "C"
EXPORT PlugInBase* pldicom_instantiatePlugIn(DLHandle myDLHandle) {
	return new y60::DICOMPlugIn(myDLHandle);
}
