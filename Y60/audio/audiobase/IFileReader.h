//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: IFileReader.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.5 $
//
//
// Description:
//
//
//=============================================================================

#ifndef _ac_AudioBase_IFileReader_h_
#define _ac_AudioBase_IFileReader_h_

#include "AudioAsyncModule.h"

namespace AudioBase {

    class IFileReader :
        public AudioAsyncModule
    {
        public:
            IFileReader(const std::string & theName, int theSampleRate,
                double theVolume = 1.0, bool theFadeIn = false)
                : AudioAsyncModule(theName, theSampleRate, theVolume, theFadeIn)
            {}

            virtual unsigned getNumChannels() const = 0;
            virtual double getDuration() const = 0;
    };
}
#endif

