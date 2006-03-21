//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "DShowHelper.h"

#include <asl/Logger.h>

using namespace std;

namespace y60 {
void checkForDShowError(HRESULT hr, const std::string & theMessage, 
        const std::string & theFileLine) 
{
    if (SUCCEEDED(hr)) {
        return;
    }
    if (FACILITY_WINDOWS == HRESULT_FACILITY(hr)) {
        hr = HRESULT_CODE(hr);
    }
    std::string myMessage;
    char * szErrMsg;
    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
                NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&szErrMsg, 0, NULL) != 0)
    {
        myMessage = szErrMsg;
        LocalFree(szErrMsg);
    } else {
        char myErrorNumber[1024];
        sprintf(myErrorNumber, "Error # %#x", hr);
        myMessage = myErrorNumber;
    }
    AC_WARNING << theFileLine << ": " << theMessage << " [" << myMessage << "]" << endl;
}

}

