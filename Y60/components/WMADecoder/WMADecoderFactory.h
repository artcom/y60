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
//   $RCSfile: WMADecoderFactory.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/04/08 18:38:36 $
//
//  WindowsMediaVideo (WMV) decoder class.
//
//=============================================================================

#ifndef _ac_y60_WMADecoderFactory_h_
#define _ac_y60_WMADecoderFactory_h_

#include <y60/IDecoder.h>
#include <asl/PlugInBase.h>
#include <audio/IAudioDecoderFactory.h>

namespace y60 {

    const std::string MIME_TYPE_WMA = "audio/wma";
    const std::string MIME_TYPE_MP3 = "audio/mp3";

    class WMADecoderFactory :
        public asl::PlugInBase,
        public AudioBase::IAudioDecoderFactory
    {
        public:
            WMADecoderFactory(asl::DLHandle theDLHandle);
            virtual ~WMADecoderFactory() {}
            std::string canDecode(const std::string & theUrl, asl::ReadableStream * theStream = 0);
            AudioBase::IFileReader * instance(const std::string & theName, double theVolume = 1.0) const;
    };
}
#endif
