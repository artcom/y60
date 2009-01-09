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
*/

// own header
#include "ExitHandler.h"

#include "SingletonManager.h"
#include "string_functions.h"
#include "Logger.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <string>

using namespace std;

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
            myMessage += asl::as_string((void*)(myRecord->ExceptionInformation[1])) 
                    + " for which it does not have the appropriate access.";
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
    asl::SingletonManager::get().destroyAllSingletons();
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void exitFunction() {
//    cerr << "exitFunction" << endl;
    asl::SingletonManager::get().destroyAllSingletons();
}

namespace asl {

ExitHandler::ExitHandler() {
#ifdef WIN32    
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
    SetUnhandledExceptionFilter(&AcUnhandledExceptionFilter);
#endif    
//    atexit(exitFunction);
}

void ExitHandler::segFault() {
    int * p = (int*)0;
    *p = 0;
}

}

