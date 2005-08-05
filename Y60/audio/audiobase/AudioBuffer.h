// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioBuffer.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.11 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_AUDIOBUFFER
#define INCL_AUDIOBUFFER

#include "AudioDefs.h"
#include <asl/ThreadLock.h>

namespace AudioBase {

class AudioBuffer {
public:
    AudioBuffer ();
    AudioBuffer( const AudioBuffer & otherBuffer);
    AudioBuffer & operator=(const AudioBuffer & otherBuffer);
    virtual ~AudioBuffer();

    void setData (const SAMPLE * myData);
    void set16Bit (const asl::Signed16 * myData, ChannelType inputChannelType, unsigned numChannels /* =2 */);
    void set24Bit (const char * myData, ChannelType inputChannelType, unsigned numChannels /* =2 */);
    void set32Bit (const asl::Signed32 * myData, ChannelType inputChannelType, unsigned numChannels);
    void dump(int importantSample = 0);
    void clear();
    void multiply(double d);
    void normalize(double theMaximum);

    asl::Signed32 * createBuffer32 ();
    asl::Signed16 * createBuffer16 ();
    void copyToBuffer16(asl::Signed16* myData);
    asl::Signed16 getData16(int theSample);
    SAMPLE * getData ();

    bool hasDistortion();
    double getMax();
    bool hasClicks();

    unsigned getSize();             // !!! size in bytes
    static void setSize (unsigned theSize); // !!! NUMBER of samples, NOT size in bytes
    static unsigned getNumSamples();        // !!! returns NUMBER of samples as set with setSize()

    static AudioBuffer * getSilenceBuffer();

private:    
    void dumpSamples (int startSample, int endSample);
    bool britzelTest (double theSample1, double theSample2, int theBufIndex);

    unsigned       _mySize;
    SAMPLE       * _myBuffer;
    double         _myLastSample;
};

}

#endif

