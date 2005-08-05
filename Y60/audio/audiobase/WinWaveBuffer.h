//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: WinWaveBuffer.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.2 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_WINWAVEBUFFER
#define INCL_WINWAVEBUFFER

#include <asl/Ptr.h>

#include <windows.h>

namespace AudioBase {

class WinWaveBuffer {
public:
    WinWaveBuffer(unsigned int mySize, DWORD myUserData);
    virtual ~WinWaveBuffer();
    
    WAVEHDR * getWavHeader();
    char * getBuffer();
    void clear();
    
private:

    // Don't use this.
    WinWaveBuffer();
    WinWaveBuffer(const WinWaveBuffer&);
    WinWaveBuffer& operator=(const WinWaveBuffer&);
    
    WAVEHDR _myHeader;
    char * _myBuffer;
};

typedef asl::Ptr<WinWaveBuffer> WinWaveBufferPtr;

}

#endif
