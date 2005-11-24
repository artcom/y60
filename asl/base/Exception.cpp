/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
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

#include "settings.h"
#include "Exception.h"
#include "string_functions.h"

using namespace std;

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

/*
string
asl::location_string(const char* file_name, unsigned line_number) {
    std::string message;
    message= std::string(file_string(file_name)) + ",line " + as_string(line_number);
    return message;
}

string
asl::compose_message(const Exception& ex) {
    return
        string(ex.name() ? ex.name() : "unspecified Exception") +
        string(" in '") +
        string(ex.where().size() ? ex.where() : string("unspecified origin")) +
        string("' (") +
        string(ex.what().size() ? ex.what() : string("unspecified reason")) +
        string(")");
}
*/

string
asl::location_string(const char* file_name, unsigned line_number) {
    return string("[") + file_string(file_name) + ":" + as_string(line_number) + "]";
}

string
asl::compose_message(const Exception& ex) {
    return
        string(ex.name() ? ex.name() : "Unspecified Exception") +
        string(": ") +
        string(ex.what().size() ? ex.what() : string("Unspecified reason")) +
        string(" ") +
        string(ex.where().size() ? ex.where() : string("Unspecified origin"));
}
