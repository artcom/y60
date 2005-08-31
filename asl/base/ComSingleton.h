//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $RCSfile: ComSingleton.h,v $
//
//   $Author: thomas $
//
//   $Revision: 1.1 $
//
//=============================================================================

#ifndef _ac_ComSingleton_h_
#define _ac_ComSingleton_h_

#include "Singleton.h"

namespace asl {

    /**
    * ComSingleton is a singleton that is used by the WMVDecoder to check
    * if COM is already initialized. 
    */
    class ComSingleton : public Singleton<ComSingleton>
    { 
        friend class SingletonManager;

    public:
        ComSingleton() : _myDecoderCount(0) {
        }

        unsigned getComInitialized() const {
            return _myDecoderCount;
        }

        /**
        * Increments COM ref count and initialize for first user.
        */
        void ref();

        /**
        * Decrements COM reference count and uninitialize for last user.
        */
        void unref();

    private:
        unsigned _myDecoderCount;  ///< Internal counter
    };

}

#endif // _ac_ComSingleton_h_
