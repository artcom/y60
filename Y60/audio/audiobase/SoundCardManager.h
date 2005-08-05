//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SoundCardManager.h,v $
//
//     $Author: christian $
//
//   $Revision: 1.8 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_SOUNDCARDMANAGER
#define INCL_SOUNDCARDMANAGER

#include "SoundCard.h"

#include <asl/Singleton.h>
#include <iostream>
#include <vector>

#ifdef WIN32
#   include <windows.h>
#   include <dsound.h>
#   define USE_DIRECTSOUND
#endif


namespace AudioBase {

    class SoundCardManager : public asl::Singleton<SoundCardManager> {
        friend class asl::SingletonManager;
    public:
        virtual ~SoundCardManager();
        
        int getNumCards();
        SoundCard* getCard(int i);

        friend std::ostream& operator << (std::ostream& strm, const SoundCardManager& myMgr);
    #ifdef USE_DIRECTSOUND
        friend BOOL CALLBACK dsCallback(LPGUID lpGuid, LPCSTR lpcstrDescription,
                                    LPCSTR lpcstrModule, LPVOID lpContext);
    #endif

    private:
        SoundCardManager();
        
        std::vector<SoundCard*> _myCards;
};

std::ostream& operator << (std::ostream& strm, const SoundCardManager& myMgr);

}
#endif
