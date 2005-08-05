//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SoundCardManager.cpp,v $
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

#include "SoundCardManager.h"
#ifdef WIN32
#include "DSoundSoundCard.h"
#include "WinMMSoundCard.h"
#else
#include "ALSASoundCard.h"
#include <alsa/control.h>
#endif

#include "DummySoundCard.h"

#include <asl/Logger.h>
#include <string>

using namespace std;

namespace AudioBase {
#ifdef WIN32
static BOOL CALLBACK dsCallback(LPGUID lpGuid, LPCSTR lpcstrDescription,
                                LPCSTR lpcstrModule, LPVOID lpContext);
#endif

    SoundCardManager::SoundCardManager () {        
#ifdef WIN32
#ifdef USE_DIRECTSOUND
        HRESULT hr = DirectSoundEnumerate(dsCallback, this);
#else
        int myNumCards = waveOutGetNumDevs();
        if (waveInGetNumDevs() != myNumCards) {
          AC_WARNING << "SoundCardManager: Interesting. Number of input and output devices differ." << "I'm not really prepared for this.";
        }
        for (int i = 0; i<myNumCards; i++) {
            WinMMSoundCard * curCard = new WinMMSoundCard(i);
            _myCards.push_back(curCard);
        }
#endif
#else
        int curCardID = -1;
        int myRetVal = snd_card_next(&curCardID);
        while (curCardID != -1) {
            ALSASoundCard * curCard = new ALSASoundCard(curCardID);
            _myCards.push_back(curCard);
            snd_card_next(&curCardID);
        }
#endif
        if (_myCards.empty()) {
            AC_WARNING << "No sound card found.";
            _myCards.push_back(new DummySoundCard());
        }
    }

    SoundCardManager::~SoundCardManager () {
        AC_DEBUG << "~SoundCardManager()";
        for (int i = 0; i< _myCards.size(); i++) {
            delete _myCards[i];
        }
    }

    int SoundCardManager::getNumCards() {
        return _myCards.size();
    }

    SoundCard* SoundCardManager::getCard(int i) {
        return _myCards[i];
    }

    std::ostream& operator << (std::ostream& strm, const SoundCardManager& myMgr) {
        strm << "Number of sound cards: " << myMgr._myCards.size() << endl;
        for (int i=0; i<myMgr._myCards.size(); i++) {
            strm << *(myMgr._myCards[i]);
        }

        strm << endl;
        return strm;
    }

#ifdef USE_DIRECTSOUND
    static BOOL CALLBACK dsCallback(LPGUID lpGuid, LPCSTR lpcstrDescription,
        LPCSTR lpcstrModule, LPVOID lpContext)
    {
        LPGUID lpTemp = NULL;
        SoundCardManager* pManager = static_cast<SoundCardManager*>(lpContext);

        if (lpGuid != NULL)  //  NULL only for "Primary Sound Driver".
        {
            DSoundSoundCard * curCard = new DSoundSoundCard(lpGuid, std::string(lpcstrDescription));
            pManager->_myCards.push_back(curCard);
        }
        return TRUE;
    }
#endif
}

