/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
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

#include <y60/jsgtk/GTKApp.h>
#include <y60/jsbase/jssettings.h>
#if defined(_MSC_VER)
    #pragma warning(push,1)
    #pragma warning(disable:4512 4413 4244 4250)
#endif //defined(_MSC_VER)
#include <gtkmm.h>
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif //defined(_MSC_VER)

#include <asl/base/begin_end.h>
#include <asl/base/Arguments.h>
#include <asl/base/string_functions.h>
#include <asl/base/StdOutputRedirector.h>
#include <asl/base/os_functions.h>

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
        if (!ourArguments.parse(myAppArgv.size(), asl::begin_ptr(myAppArgv) )) {
            return 0;
        }

        asl::StdOutputRedirector::get().init(ourArguments);
        GTKApp myApp;

        if (ourArguments.haveOption("--no-jswarnings")) {
            myApp.setReportWarnings(false);
        }

        myApp.setStrictFlag(true);

        if (ourArguments.haveOption("--jsversion")) {
            myApp.setJSVersion(asl::as<int>(ourArguments.getOptionArgument("--jsversion")));
        }

        string myIncludePath;
        if (ourArguments.haveOption("-I")) {
            myIncludePath = ourArguments.getOptionArgument("-I");
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
            rv = myApp.run(myFilename, myIncludePath, myScriptArgs);
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


