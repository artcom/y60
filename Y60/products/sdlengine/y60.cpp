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
#include <asl/string_functions.h>
#include <asl/os_functions.h>

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
    {"--pause-on-error", ""},
    {"--std-logfile", "logfile base filename for stdout/stderr"},
    {"--help", ""},
    {"--tutorial-screenshots", ""},
     //y60 will quit automatically after some frames taking a screen shot.
    {"", ""}
};

#ifdef WIN32
    LONG WINAPI AcUnhandledExceptionFilter(_EXCEPTION_POINTERS * theExceptionInfo) {
        _EXCEPTION_RECORD * myRecord = theExceptionInfo->ExceptionRecord;
        string myMessage;
        string myName;
        switch (myRecord->ExceptionCode) {
            case EXCEPTION_ACCESS_VIOLATION:
                myName = "EXCEPTION_ACCESS_VIOLATION";
                if (myRecord->ExceptionInformation[0] == 0) {
                    myMessage = "The thread tried to read from the virtual address ";
                } else {
                    myMessage = "The thread tried to write to the virtual address ";
                }
                myMessage += asl::as_string((void*)(myRecord->ExceptionInformation[1])) + " for which it does not have the appropriate access.";
                break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                myName = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
                myMessage = "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.";
                break;
            case EXCEPTION_BREAKPOINT:
                myName = "EXCEPTION_BREAKPOINT";
                myMessage = "A breakpoint was encountered.";
                break;
            case EXCEPTION_DATATYPE_MISALIGNMENT:
                myName = "EXCEPTION_DATATYPE_MISALIGNMENT";
                myMessage = "The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.";
                break;
            case EXCEPTION_FLT_DENORMAL_OPERAND:
                myName = "EXCEPTION_FLT_DENORMAL_OPERAND";
                myMessage = "One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.";
                break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                myName = "EXCEPTION_FLT_DIVIDE_BY_ZERO";
                myMessage = "The thread tried to divide a floating-point value by a floating-point divisor of zero.";
                break;
            case EXCEPTION_FLT_INEXACT_RESULT:
                myName = "EXCEPTION_FLT_INEXACT_RESULT";
                myMessage = "The result of a floating-point operation cannot be represented exactly as a decimal fraction.";
                break;
            case EXCEPTION_FLT_INVALID_OPERATION:
                myName = "EXCEPTION_FLT_INVALID_OPERATION";
                myMessage = "This exception represents any floating-point exception not included in this list.";
                break;
            case EXCEPTION_FLT_OVERFLOW:
                myName = "EXCEPTION_FLT_OVERFLOW";
                myMessage = "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.";
                break;
            case EXCEPTION_FLT_STACK_CHECK:
                myName = "EXCEPTION_FLT_STACK_CHECK";
                myMessage = "The stack overflowed or underflowed as the result of a floating-point operation.";
                break;
            case EXCEPTION_FLT_UNDERFLOW:
                myName = "EXCEPTION_FLT_UNDERFLOW";
                myMessage = "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.";
                break;
            case EXCEPTION_ILLEGAL_INSTRUCTION:
                myName = "EXCEPTION_ILLEGAL_INSTRUCTION";
                myMessage = "The thread tried to execute an invalid instruction.";
                break;
            case EXCEPTION_IN_PAGE_ERROR:
                myName = "EXCEPTION_IN_PAGE_ERROR";
                myMessage = "The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network.";
                break;
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                myName = "EXCEPTION_INT_DIVIDE_BY_ZERO";
                myMessage = "The thread tried to divide an integer value by an integer divisor of zero.";
                break;
            case EXCEPTION_INT_OVERFLOW:
                myName = "EXCEPTION_INT_OVERFLOW";
                myMessage = "The result of an integer operation caused a carry out of the most significant bit of the result.";
                break;
            case EXCEPTION_INVALID_DISPOSITION:
                myName = "EXCEPTION_INVALID_DISPOSITION";
                myMessage = "An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception.";
                break;
            case EXCEPTION_NONCONTINUABLE_EXCEPTION:
                myName = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
                myMessage = "The thread tried to continue execution after a noncontinuable exception occurred.";
                break;
            case EXCEPTION_PRIV_INSTRUCTION:
                myName = "EXCEPTION_PRIV_INSTRUCTION";
                myMessage = "The thread tried to execute an instruction whose operation is not allowed in the current machine mode.";
                break;
            case EXCEPTION_SINGLE_STEP:
                myName = "EXCEPTION_SINGLE_STEP";
                myMessage = "A trace trap or other single-instruction mechanism signaled that one instruction has been executed.";
                break;
            case EXCEPTION_STACK_OVERFLOW:
                myName = "EXCEPTION_STACK_OVERFLOW";
                myMessage = "The thread used up its stack.";
                break;
        }

        AC_FATAL << "Unhandled exception caught: \n" <<
                    "Exception code: " << (void*)(myRecord->ExceptionCode) << "\n" <<
                    "Exception adress: " << myRecord->ExceptionAddress << "\n" <<
                    "" << myName << ": " << myMessage << "\n";
        return EXCEPTION_EXECUTE_HANDLER;
    }
#endif

//in SDLWindow.cpp
extern bool _ourGenerateTutorialMode;

int
main(int argc, char **argv) {

#ifdef WIN32
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
    SetUnhandledExceptionFilter(&AcUnhandledExceptionFilter);
#endif
    int rv = 1;
    try {
        ourArguments.addAllowedOptions(ourAllowedOptions);
        if (!ourArguments.parse(argc, argv)) {
            return 0;
        }

        asl::StdOutputRedirector::get().init(ourArguments);

        SDLApp myApp;
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

        string myIncludePath;
        if (ourArguments.haveOption("-I")) {
            myIncludePath = ourArguments.getOptionArgument("-I");
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
            rv = myApp.run(myFilename, myIncludePath, myScriptArgs);
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

#if 0
#ifdef WIN32
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
            LPSTR lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv);
}
#endif
#endif
