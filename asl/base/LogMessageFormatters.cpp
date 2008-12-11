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
//
//   $RCSfile: LogMessageFormatters.cpp,v $
//   $Author: pavel $
//   $Revision: 1.1 $
//   $Date: 2005/04/29 08:09:22 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#include "LogMessageFormatters.h"
#include "TimeStreamFormater.h"
#include "string_functions.h"
#include "os_functions.h"
using namespace asl;

std::string
FullLogMessageFormatter::format(
                                  asl::Time theTime,
                                  Severity            theSeverity,
                                  const std::string & theModule,
                                  int                 theId,
                                  const std::string & theText)
{
    std::ostringstream myMessage;
    std::string myPID = asl::as_string(getThreadId());

	static const char * myFormatString("%Y-%M-%D %h:%m:%s.%l");
    bool hasNewLineEnd = (theText.size() && theText[theText.size()-1] == '\n');

    myMessage << "[" << myPID << ", " << asl::formatTime(myFormatString) << theTime << "] [" << SeverityName[theSeverity] << "] [" << file_string(theModule.c_str()) << ":" << theId << "] ";
    if (hasNewLineEnd) {
        myMessage << theText.substr(0,theText.size()-1);
    } else {
        myMessage << theText;
    }
    return myMessage.str();
}

std::string
TerseLogMessageFormatter::format(
                                  asl::Time theTime,
                                  Severity            theSeverity,
                                  const std::string & theModule,
                                  int                 theId,
                                  const std::string & theText)
{
    std::ostringstream myMessage;
    bool hasNewLineEnd = (theText.size() && theText[theText.size()-1] == '\n');

    myMessage << "### " << SeverityName[theSeverity] << ": ";
    if (hasNewLineEnd) {
        myMessage << theText.substr(0,theText.size()-1);
    } else {
        myMessage << theText;
    }
    const int myLineWidth = 120;
    std::string myModuleName = file_string(theModule.c_str());
    std::string myModuleId = asl::as_string(theId);
    std::string myPID = asl::as_string(getThreadId());
    int mySpaces =  myLineWidth - myMessage.str().size() - myModuleName.size() - 16 - myModuleId.size() - myPID.size();
    for (int i = 0; i < mySpaces; ++i) {
        myMessage << " ";
    }
    myMessage << " [" << myPID << "," << myModuleName << ":" << theId << ",T= "<< asl::formatTime("%s.%l") << theTime << "]";
    return myMessage.str();
}

std::string
VisualStudioLogMessageFormatter::format(asl::Time theTime,
                                  Severity            theSeverity,
                                  const std::string & theModule,
                                  int                 theId,
                                  const std::string & theText)
{
    std::ostringstream myMessage;
    static const char * myFormatString("%h:%m:%s.%l");
    myMessage << theModule << "(" << theId << ") : " << SeverityName[theSeverity] << " [" << asl::formatTime(myFormatString) << theTime << "] : " << theText;
    return myMessage.str();
}

