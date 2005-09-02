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
//   $RCSfile: acgtkshell.cpp,v $
//   $Author: christian $
//   $Revision: 1.8 $
//   $Date: 2005/04/20 16:54:30 $
//
//   Description: Application-wrapper for xpshell
//
//=============================================================================

// Commit Tests (acbuild) foo

#include <jsgtk/GTKApp.h>

#include <y60/jssettings.h>
#include <gtkmm.h>

#include <asl/Arguments.h>
#include <asl/string_functions.h>
#include <asl/StdOutputRedirector.h>
#include <asl/os_functions.h>

#include <iostream>

#define DB(x) // x

using namespace std;
using namespace jslib;

asl::Arguments ourArguments;

const asl::Arguments::AllowedOption ourAllowedOptions[] = {
    {"--jsversion", "VERSION"},
    {"-I", "include path (semicolon-separated)"},
    {"--no-jswarnings", ""},
    {"--pause-on-error", ""},
    {"--std-logfile", "logfile base filename for std output"},
    {"", ""}
};

void
printUsage() {
    ourArguments.printUsage();
}

int
acMain(int argc, char **argv) {
    int rv = 0;

    try {
        // gtk removes the "--" argument. So we only pass the args
        // before the "--" to gtk
        int myDoubleHyphenPos = 0;
        for (int i=1; i < argc; ++i) {
            if (strcmp(argv[i],"--") == 0) {
                myDoubleHyphenPos = i;
                break;
            }
        }

        int myGtkArgc = myDoubleHyphenPos ? myDoubleHyphenPos : argc;

        Gtk::Main myGtk(myGtkArgc, argv);

        vector<char *> myAppArgv;
        for (int i=0; i < myGtkArgc; ++i) {
            DB(cerr << "1:" << argv[i] << endl);
            myAppArgv.push_back(argv[i]);
        }
        if (myDoubleHyphenPos) {
            for (int i=myDoubleHyphenPos; i < argc; ++i) {
                DB(cerr << "2:" << argv[i] << endl);
                myAppArgv.push_back(argv[i]);
            }
        }

        ourArguments.addAllowedOptions(ourAllowedOptions);
        if (!ourArguments.parse(myAppArgv.size(), &(*myAppArgv.begin()) )) {
            return 0;
        }

        asl::StdOutputRedirector myRedirector(ourArguments);

        //AC_PRINT << ourArguments.getProgramName() << " Copyright (C) 2003-2005 ART+COM";

        GTKApp myApp;

        if (ourArguments.haveOption("-I")) {
            myApp.setIncludePath(ourArguments.getOptionArgument("-I"));
        }
        std::string myTextureDirs = asl::expandEnvironment("${AC_TEXTURE_DIR}");
        if (myTextureDirs.length() == 0) {
            myTextureDirs = "./";
        }
        JSApp::getPackageManager()->add(myTextureDirs);


        if (ourArguments.haveOption("--no-jswarnings")) {
            myApp.setReportWarnings(false);
        }

        myApp.setStrictFlag(true);

        if (ourArguments.haveOption("--jsversion")) {
            myApp.setJSVersion(asl::as<int>(ourArguments.getOptionArgument("--jsversion")));
        }

        if (ourArguments.getCount() < 1) {
            printUsage();
            rv = 1;
        } else {
            std::string myFilename = ourArguments.getArgument(0);
            std::vector<std::string> myScriptArgs;

            unsigned myArgCount = ourArguments.getCount();
            for (unsigned i=1; i<myArgCount; ++i) {
                myScriptArgs.push_back(ourArguments.getArgument(i));
            }
            myApp.setProgramName(ourArguments.getProgramName());
            rv = myApp.run(myFilename, myScriptArgs);
        }
    } catch (asl::Exception & ex) {
        rv = 1;
        cerr << "Exception caught in main: " << ex << endl;
    } catch (std::exception & ex) {
        rv = 1;
        cerr << "std::exception caught in main: " << ex.what() << endl;
    } catch (...) {
        rv = 1;
        cerr << "unknown exception caught in main" << endl;
    }

    if (rv != 0 && ourArguments.haveOption("--pause-on-error")) {
        char temp[100];
        cerr << "Some Errors occured. Hit RETURN to Exit." << endl;
        cin.getline(temp, sizeof(temp));
    }

    return rv;
}


#ifdef WIN32
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
            LPSTR lpCmdLine, int nCmdShow)
{
    return acMain(__argc, __argv);
}
#endif

int
main(int argc, char ** argv) {
    return acMain(argc, argv);
}


