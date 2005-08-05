//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioDefs.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.7 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_AUDIODEFS
#define INCL_AUDIODEFS

#include <asl/settings.h>
#include <assert.h>

namespace AudioBase {

#define SAMPLE double 
const int FADE_DURATION = int(0.01*48000);
/**
 *  Definition of the channeltype in the audiograph
 */
enum ChannelType {
    Mono=0,               ///< Mono channel
    Left=0,               ///< Left channel
    Right=1,              ///< Right channel
    Center=2,             ///< Center channel
    Sub=3,                ///< Subwoofer 5.1 channel
    RearLeft=4,           ///< Rear left 5.1 channel
    RearRight=5,          ///< Rear right 5.1 channel
    Undefined=-1          ///< Undefined Type
};               

//const char* CHANNELNAMES[] = {"Left", "Right", "Mono", "Sub", "RearLeft", "RearRight"};

#define MAXCHANNELS 8

inline ChannelType getChannelType(unsigned index, unsigned numChannels) {
    /**
    * Mapping of ChannelType to index in stream 
    * @link http://www.microsoft.com/whdc/device/audio/multichaud.mspx
    */
    const ChannelType CHANNELTYPE1[] = 
    {Mono, Undefined, Undefined, Undefined, Undefined, Undefined, Undefined, Undefined};
    const ChannelType CHANNELTYPE2[] = 
    {Left, Right, Undefined, Undefined, Undefined, Undefined, Undefined, Undefined};
    const ChannelType CHANNELTYPE3[] = 
    {Left, Right, Center, Undefined, Undefined, Undefined, Undefined, Undefined};
    const ChannelType CHANNELTYPE4[] = 
    {Left, Right, RearLeft, RearRight, Undefined, Undefined, Undefined, Undefined};
    const ChannelType CHANNELTYPE6[] = 
    {Left, Right, Center, Sub, RearLeft, RearRight, Undefined, Undefined};
    const ChannelType CHANNELTYPE8[] = 
    {Left, Right, Center, Sub, RearLeft, RearRight, Undefined, Undefined};
    const ChannelType* CHANNELTYPES[] = 
    {CHANNELTYPE1, CHANNELTYPE2, CHANNELTYPE3, CHANNELTYPE4, 0, CHANNELTYPE6, 0, CHANNELTYPE8};

    unsigned myMaxChannels = sizeof(CHANNELTYPES) / sizeof(CHANNELTYPES[0]);
    assert(numChannels-1 <= myMaxChannels);
    const ChannelType * myTypes = CHANNELTYPES[numChannels-1];
    return myTypes[index];
} 


inline int getChannelIndex(ChannelType type, unsigned numChannels) {
    for (unsigned i = 0; i < MAXCHANNELS; ++i) {
        if (getChannelType(i, numChannels) == type) {
            return i;
        }
    }
    return -1;
}

inline bool testChannelCompatibility (ChannelType c1, ChannelType c2) {
    return (c1 == Mono || c2 == Mono || c1 == c2);
}

}

#endif
