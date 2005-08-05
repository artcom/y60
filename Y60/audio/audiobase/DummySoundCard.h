//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: DummySoundCard.h,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.2 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_DUMMYSOUNDCARD
#define INCL_DUMMYSOUNDCARD

#include "SoundCard.h"

#include <asl/Time.h>
#include <asl/ThreadLock.h>
#include <vector>

namespace AudioBase {

class AudioBuffer;    

class DummySoundCard: public SoundCard {
public:
    DummySoundCard ();
    virtual ~DummySoundCard ();

    virtual std::string asString() const;

    virtual void write(std::vector<AudioBuffer*>& theBuffers);
    virtual void read(std::vector<AudioBuffer*>& theBuffers);
    virtual int getNumUnderruns() const;

    virtual double getCurrentTime();

private:
    asl::Time _myStartTime;
};

}
#endif
