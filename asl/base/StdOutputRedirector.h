//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.


#ifndef _ac_asl_StdOutputRedirector_h_
#define _ac_asl_StdOutputRedirector_h_


#include <fstream>
#include <assert.h>

namespace asl {
    /*! \addtogroup aslbase */
    /* @{ */

    class Arguments;
    class StdOutputRedirector {
        public:
            StdOutputRedirector(const Arguments & theArguments);
            ~StdOutputRedirector();
        private:
            StdOutputRedirector();
                        
            std::ofstream    _myOutputStreamOut;
            std::streambuf * _myOutFile;
    };    
    /* @} */
}
#endif
