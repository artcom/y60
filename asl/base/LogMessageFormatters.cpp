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

