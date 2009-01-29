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
#   if defined(_MSC_VER)
#       pragma warning (push,1)
#   endif //defined(_MSC_VER)
#   include <paintlib/pldecoderfactory.h>
#   if defined(_MSC_VER)
#       pragma warning (pop)
#   endif //defined(_MSC_VER)
#   include <y60/image/IPaintLibExtension.h>
#   include "pldicomdec.h"
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
EXPORT PlugInBase* y60DICOM_instantiatePlugIn(DLHandle myDLHandle) {
	return new y60::DICOMPlugIn(myDLHandle);
}
