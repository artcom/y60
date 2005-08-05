#ifndef AC_AUDIO_BAND_REJECT_FILTER_INCLUDED
#define AC_AUDIO_BAND_REJECT_FILTER_INCLUDED

#include "ButterworthFilter.h"

// Implemenation inspired by spkit 
// URL http://www.music..helsinki.fi/research/spkit

namespace AudioBase {
    
class  BandRejectFilter : public ButterworthFilter {
    public:
        BandRejectFilter(double theFrquency,
                         double theBandwidth,
                         int theSampleRate,
                         double theVolume = 1.0);


        double getFrequency() const;
        double getBandwidth() const;

    private:
        double _myFrequency;
        double _myBandwidth;
};

}
#endif // AC_AUDIO_BAND_REJECT_FILTER_INCLUDED
