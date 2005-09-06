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

#include <string>

#include "Singleton.h"

namespace asl {
    /*! \addtogroup aslbase */
    /* @{ */

    class Arguments;
    class StdOutputRedirector : public Singleton<StdOutputRedirector> {
        friend class SingletonManager;        
        public:
            void init(const Arguments & theArguments);
            ~StdOutputRedirector();
            void checkForFileWrapAround();
        protected:
        private:
            StdOutputRedirector();
            void redirect();
            void removeoldArchives();
                        
            std::ofstream    _myOutputStreamOut;
            std::streambuf * _myOutFile;
            long             _myMaximumFileSize;
            std::string      _myOutputFilename;
            std::string      _myOldArchiveFilename;
            long long        _myStartTime;
            bool             _myRemoveOldArchiveFlag; // always keep only one archive log and one current
            bool             _myLogInOneFileFlag;     // create on file with multiple app starts
            long long        _myFileSizeCheckFrequInSec;
    };    
    /* @} */
}
#endif
