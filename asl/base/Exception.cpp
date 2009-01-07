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
//    $RCSfile: Exception.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: Exception class hierarchy
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#include "Exception.h"

#include <cstdlib>

#include "string_functions.h"
#include "os_functions.h"

#include "ExitHandler.h"

using namespace std;

namespace asl {

    bool Exception::_ourDumpStackTraceFlag( false );

    ExitHandler _ourExitHandler;

    // TODO: make this a static function
    bool abortOnThrow() {
        bool myDummy;
        static bool myAbortOnThrowFlag = asl::get_environment_var_as("AC_ABORT_ON_THROW", myDummy);
        return myAbortOnThrowFlag;
    }
    Exception::Exception(const std::string & what, const std::string & where)
            : _what(what), _where(where), _name("Exception")
    {
        if (abortOnThrow()) {
            cerr << *this;
            abort();
        }
    };
    
    Exception::Exception(const std::string & what, const std::string & where, const char * name)
            : _what(what), _where(where), _name(name) 
    {
        if (abortOnThrow()) {
            cerr << *this;
            abort();
        }
    };

    void
    Exception::initExceptionBehaviour() {
        bool isSet = asl::get_environment_var_as("AC_TRACE_EXCEPTIONS", _ourDumpStackTraceFlag);
        if ( ! isSet ) {
            _ourDumpStackTraceFlag = false;
        }
    }

    void
    Exception::dumpStackTrace( bool theFlag ) {
        _ourDumpStackTraceFlag = theFlag;
    }
    bool
    Exception::getDumpStackTraceFlag() {
        return _ourDumpStackTraceFlag;
    }

}

string
asl::file_string(const char* file_name) {
    std::string myFilename(file_name); //asl::toLowerCase(file_name));
    std::string::size_type mySlash = myFilename.find_last_of("/\\");
    if (mySlash != std::string::npos) {
        myFilename = myFilename.substr(mySlash+1);
    }
    return myFilename;
}

string
asl::line_string(unsigned line_number) {
    std::string message;
    message = as_string(line_number);
    return message;
}

string
asl::location_string(const char* file_name, unsigned line_number) {
    return string("[") + file_string(file_name) + ":" + as_string(line_number) + "]";
}

string
asl::compose_message(const Exception& ex) {
    std::string myMsg;
    myMsg = 
        string(ex.name() ? ex.name() : "Unspecified Exception") +
        string(": ") +
        string(ex.what().size() ? ex.what() : string("Unspecified reason")) +
        string(" ") +
        string(ex.where().size() ? ex.where() : string("Unspecified origin"));

    if ( asl::Exception::getDumpStackTraceFlag() && ex.stack().size() != 0 ) {
        myMsg += string("\n") + as_string( ex.stack() );
    }

    return myMsg;
}
