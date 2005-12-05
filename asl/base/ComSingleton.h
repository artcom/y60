/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description:  ComSingleton is a singleton that is used by the WMVDecoder to check
//              if COM is already initialized.
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : perfect
//    formatting             : ok
//    documentation          : ok
//    test coverage          : ok
//    names                  : ok
//    style guide conformance: ok
//    technical soundness    : ok
//    dead code              : ok
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : ok
//
//    overall review status  : ok
//
//    recommendations:
//       - none
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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
