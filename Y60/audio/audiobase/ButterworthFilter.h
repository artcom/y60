#ifndef AC_AUDIO_BUTTERWORTH_FILTER_INCLUDED
#define AC_AUDIO_BUTTERWORTH_FILTER_INCLUDED

#include "AudioSyncModule.h"

// Implemenation inspired by spkit 
// URL http://www.music..helsinki.fi/research/spkit

namespace AudioBase {

class ButterworthFilter : public AudioSyncModule {
    public:
        ButterworthFilter(const char * theName, int mySampleRate, 
                double theVolume = 1.0);
        virtual ~ButterworthFilter() = 0;

        void process();

    protected:
        SAMPLE a[3];
        SAMPLE b[2];

    private:

        void processSample(double & theSample);

        struct ButterworthBuffer {
            SAMPLE x[2];
            SAMPLE y[2];
        };

        ButterworthBuffer _myBuffer;
};
}
#endif // AC_AUDIO_BUTTERWORTH_FILTER_INCLUDED
