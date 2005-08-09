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
//   $RCSfile: acxpshell.cpp,v $
//   $Author: pavel $
//   $Revision: 1.56 $
//   $Date: 2005/04/24 00:41:20 $
//
//   Description: Application-wrapper for xpshell
//
//=============================================================================

#include <y60/jssettings.h>
#include "SDLApp.h"

#include "SDL/SDL.h"

#include <asl/Arguments.h>
#include <asl/StdOutputRedirector.h>
#include <asl/DirectoryPackage.h>
#include <asl/string_functions.h>
#include <asl/os_functions.h>
#include <asl/Revision.h>

#include <iostream>

using namespace std;
using namespace jslib;

asl::Arguments ourArguments;

#include <iostream>
#include <sstream>

const asl::Arguments::AllowedOption ourAllowedOptions[] = {
    {"--jsversion", "VERSION"},
    {"-I", "include path (semicolon-separated)"},
    {"--no-jswarnings", ""},
    {"--version", ""},
    {"--revision", ""},
    {"--pause-on-error", ""},
    {"--std-logfile", "logfile base filename for stdout/stderr"},
    {"--help", ""},
    {"", ""}
};

int
main(int argc, char **argv) {
    int rv = 1;
    try {
        ourArguments.addAllowedOptions(ourAllowedOptions);
        ourArguments.parse(argc, argv);

        asl::StdOutputRedirector myRedirector(ourArguments);

        AC_PRINT << ourArguments.getProgramName() << " Copyright (C) 2003-2005 ART+COM";
        AC_INFO << "Build date " << __DATE__ << " " << __TIME__;
        AC_INFO << "Revision: " << asl::ourRevision;

        if (ourArguments.haveOption("--version")) {
            AC_PRINT << "build on " << __DATE__ << " at " << __TIME__
                     << " (Rev: " << asl::ourRevision << ")";
            return 0;
        }

        if (ourArguments.haveOption("--revision")) {
            AC_PRINT << "Revision: " << asl::ourRevision;
            return 0;
        }

        // look in AC_TEXTURE_DIR if set
        std::string myTextureDirs = asl::expandEnvironment("${AC_TEXTURE_DIR}");
        if (myTextureDirs.empty() != 0) {
            JSApp::getPackageManager()->add(myTextureDirs);
        }

        // *always* look in the current working directory
        JSApp::getPackageManager()->add(asl::IPackagePtr(new asl::DirectoryPackage("")));

        SDLApp myApp;

        if (ourArguments.haveOption("-I")) {
            myApp.setIncludePath(ourArguments.getOptionArgument("-I"));
        }

        if (ourArguments.haveOption("--no-jswarnings")) {
            myApp.setReportWarnings(false);
        }

        myApp.setStrictFlag(true);

        if (ourArguments.haveOption("--jsversion")) {
            myApp.setJSVersion(asl::as<int>(ourArguments.getOptionArgument("--jsversion")));
        }

        if (ourArguments.haveOption("--help")) {
            ourArguments.printUsage();
            return 0;
        }

        if (ourArguments.getCount() < 1) {
            ourArguments.printUsage();
            return 1;
        }

        std::string myFilename = ourArguments.getArgument(0);
        std::vector<std::string> myScriptArgs;

        unsigned myArgCount = ourArguments.getCount();
        for (unsigned i = 1; i < myArgCount; ++i) {
            myScriptArgs.push_back(ourArguments.getArgument(i));
        }

        if (SDL_Init(SDL_INIT_NOPARACHUTE) == -1) { // prevents SDL from catching fatal signals
            AC_FATAL << SDL_GetError() << endl;
            rv = 1;
        } else {
            myApp.setProgramName(ourArguments.getProgramName());
            rv = myApp.run(myFilename, myScriptArgs);
            SDL_Quit();
        }

    } catch (asl::Exception & ex) {
        rv = 1;
        AC_FATAL << "Exception caught in main: " << ex << endl;
    } catch (std::exception & ex) {
        rv = 1;
        AC_FATAL << "std::exception caught in main: " << ex.what() << endl;
    } catch (...) {
        rv = 1;
        AC_FATAL << "unknown exception caught in main" << endl;
    }

    if (rv != 0 && ourArguments.haveOption("--pause-on-error")) {
        char temp[100];
        AC_FATAL << "Some Errors occured. Hit RETURN to Exit." << endl;
        cin.getline(temp, sizeof(temp));
    }

#ifdef WIN32
    // [CH]: For some strange reason windows will not pass the error code to the shell if it is returned.
    // But this should not hurt, because MSDN says:
    // "Issuing a return statement from the main function is equivalent to calling the exit
    // function with the return value as its argument."
    exit(rv);
#endif

    return rv;
}


