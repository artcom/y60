//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: QuitFlagSingleton.h,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2005/02/16 12:22:21 $
//
//
//=============================================================================

#include <asl/Singleton.h>

#ifndef _Y60_ACXPSHELL_EXIT_INCLUDED_
#define _Y60_ACXPSHELL_EXIT_INCLUDED_

namespace jslib {

    class QuitFlagSingleton :
        public asl::Singleton<QuitFlagSingleton>
    {
        friend class asl::SingletonManager;
        public:
            QuitFlagSingleton() : _myQuitFlag(false) {}
            void setQuitFlag(bool theFlag) {
                _myQuitFlag = theFlag;
            }

            bool getQuitFlag() const {
                return _myQuitFlag;
            }
        private:
            bool _myQuitFlag;
    };
}

#endif

