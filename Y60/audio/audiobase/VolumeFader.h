//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: VolumeFader.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.3 $
//
//
// Description:
//
//
//=============================================================================

#ifndef INCL_VOLUMEFADER
#define INCL_VOLUMEFADER

namespace AudioBase {

class AudioBuffer;

class VolumeFader {
public:
    VolumeFader(double myVolume, bool myFadeIn);
    void setVolume(unsigned curSample, double myVolume);
    void fadeTo(unsigned curSample, double myVolume, unsigned numSamples);
    void applyVolume(unsigned curSample, AudioBuffer & theBuffer) const;
    double getVolume (unsigned curSample, bool doDebug=false) const;

private:

    double   _myVolume;
    double   _myLastVolume;
    unsigned _myFadeStartSample;
    int      _myFadeDuration;
};


}
#endif
