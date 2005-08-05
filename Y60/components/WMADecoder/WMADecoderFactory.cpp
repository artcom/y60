//=============================================================================
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: WMADecoderFactory.cpp,v $
//     $Author: christian $
//   $Revision: 1.1 $
//       $Date: 2005/04/08 18:38:36 $
//
//  WindowsMediaAudio (WMA) decoder class.
//
//=============================================================================

#include "WMADecoderFactory.h"
#include "WMADecoder.h"

#include <asl/file_functions.h>

extern "C"
EXPORT asl::PlugInBase * y60WMADecoder_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::WMADecoderFactory(myDLHandle);
}

namespace y60 {

    WMADecoderFactory::WMADecoderFactory(asl::DLHandle theDLHandle) :
        PlugInBase(theDLHandle)
    {}

    std::string
    WMADecoderFactory::canDecode(const std::string & theUrl, asl::ReadableStream * theStream) {
        std::string myExtension = asl::toLowerCase(asl::getExtension(theUrl));
        if (myExtension == "wma" || myExtension == "asf" || myExtension == "wmv" ||
            (theUrl.find("mms://") == 0 && myExtension == "" ) ||
            (theUrl.find("http://") == 0 && myExtension == "" ))
            //XXX: WMADecoder is now fallback for i.e. http://server/dir
        {
            return MIME_TYPE_WMA;
        } else if (myExtension == "mp3" &&
                   (theUrl.find("http://") == 0 || theUrl.find("mms://") == 0))
        {
            return MIME_TYPE_MP3;
        } else {
            return "";
        }
    }

    AudioBase::IFileReader * 
    WMADecoderFactory::instance(const std::string & theName, double theVolume) const {
        return new WMADecoder(theName, theVolume);
    }
}