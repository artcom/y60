/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ac_asl_StdOutputRedirector_h_
#define _ac_asl_StdOutputRedirector_h_

#include "asl_base_settings.h"

#include <fstream>
#include <assert.h>

#include <string>

#include "Singleton.h"

namespace asl {
    /*! \addtogroup aslbase */
    /* @{ */

    class Arguments;
    class ASL_BASE_DECL StdOutputRedirector : public Singleton<StdOutputRedirector> {
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
            std::streambuf * _myOriginalCoutBuffer;
            std::streambuf * _myOriginalCerrBuffer;
            long             _myMaximumFileSize;
            std::string      _myOutputFilename;

            long long        _myStartTime;
            std::string      _myOldArchiveFilename;
            bool             _myRemoveOldArchiveFlag; // always keep only one archive log and one current
            bool             _myLogInOneFileFlag;     // create on file with multiple app starts
            long long        _myFileSizeCheckFrequInSec;
    };
    /* @} */
}
#endif
