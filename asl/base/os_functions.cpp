/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: os_functions.cpp,v $
//
//   $Revision: 1.5 $
//
// Description: misc operating system & environment helper functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifdef WIN32
  #define _WIN32_WINNT 0x0500
#endif

#include "os_functions.h"
#include "Exception.h"
#include "string_functions.h"
#include "file_functions.h"
#include "Time.h"
#include "Logger.h"

// cycle counter

#include <string>
#include <algorithm>
#include <sstream>

#ifdef WIN32
  #include <windows.h>
#endif

#ifdef LINUX
  #include <sys/types.h>
#endif

#define DB(x) // x;

using namespace std;

namespace asl {

#ifdef WIN32
    // This is executed once on program start.
    char * reallyGetHostname() {
        HINSTANCE           myDLL_Handle;
        typedef UINT (CALLBACK* MYGETCOMPUTERNAME)   (LPTSTR, LPDWORD);
        typedef UINT (CALLBACK* MYGETCOMPUTERNAMEEX) (COMPUTER_NAME_FORMAT, LPTSTR, LPDWORD);
        MYGETCOMPUTERNAME   myGetComputerName;
        MYGETCOMPUTERNAMEEX myGetComputerNameEx;

        DWORD       length = 4096;
        static char myHostname[4096];
        BOOL        ok = false;

        myDLL_Handle = LoadLibrary("kernel32.dll");
        if (myDLL_Handle != NULL) {
#ifdef UNICODE
            myGetComputerNameEx = (MYGETCOMPUTERNAMEEX)GetProcAddress(myDLL_Handle,
                                                                      "GetComputerNameExW");
#else
            myGetComputerNameEx = (MYGETCOMPUTERNAMEEX)GetProcAddress(myDLL_Handle,
                                                                      "GetComputerNameExA");
#endif // UNICODE
            if (!myGetComputerNameEx) {
                // Win9x version of kernel32.dll
                AC_DEBUG << "Win9x version of 'kernel32.dll' detected.";
#ifdef UNICODE
                myGetComputerName = (MYGETCOMPUTERNAME)GetProcAddress(myDLL_Handle,
                                                                      "GetComputerNameW");
#else
                myGetComputerName = (MYGETCOMPUTERNAME)GetProcAddress(myDLL_Handle,
                                                                      "GetComputerNameA");
#endif // UNICODE
                if (!myGetComputerName) {
                    throw (asl::Exception(
                            "DLL export 'GetComputerName' was not found in Win9x version of 'kernel32.dll'.",
                            PLUS_FILE_LINE));
                } else {
                    ok = myGetComputerName (myHostname, &length);
                }
            } else {
                // call GetComputerNameEx(...)
                ok = myGetComputerNameEx((COMPUTER_NAME_FORMAT)ComputerNameDnsFullyQualified,
                                          myHostname, &length);
            }
            FreeLibrary(myDLL_Handle);
        }
        if (!ok) {
            throw (asl::Exception("Computer name could not be retrieved.", PLUS_FILE_LINE));
        }
        return myHostname;
    }
#endif

#ifdef OSX
    // This is executed once on program start.
    char * reallyGetHostname() {
        const size_t myLength = 4096;
        static char myHostname[myLength];
        int myResult = gethostname(myHostname, myLength);
        if (myResult == -1) {
            throw asl::Exception();
        }
        return myHostname;
    }

    std::string getAppname() {
        static string myPid = asl::as_string(getpid());
        const size_t myLength = 4096;
        static char myAppnameBuffer[myLength];
        static string myCommand (string("ps -c -o command -p ") + myPid + " | tail -1");
        FILE * myProcess = popen(myCommand.c_str(), "r");
        fread(myAppnameBuffer, myLength, 1, myProcess);
        pclose(myProcess);
        std::string myAppname(myAppnameBuffer);
        return myAppname.substr(0, myAppname.size() - 1);
    }
#endif

    const string & hostname() {
#ifdef WIN32
        static string hostname (reallyGetHostname());
#endif
#ifdef LINUX
        static string hostname( getenv("HOSTNAME"));
#endif
#ifdef OSX
        static string hostname( reallyGetHostname());
#endif
        return hostname;
    }

    const string & appname() {
        static string appname;
#ifdef WIN32
        int numArgs;
        LPWSTR * theCmdLine = CommandLineToArgvW((LPCWSTR)GetCommandLine(), &numArgs);
        appname = (const char *)theCmdLine[0];
        GlobalFree (theCmdLine);
#endif
#ifdef LINUX
        static string pid = asl::as_string(getpid());
        static string procstat = asl::readFile(string("/proc/")+pid+"/stat");
        appname = procstat.substr(procstat.find('(')+1,procstat.find(')')-procstat.find('(')-1);
#endif
#ifdef OSX
        appname = getAppname();
#endif
        return appname;
    }

    const string & hostappid() {
        static string hostappid = hostname() + ":" + appname();
        return hostappid;

    }

    string getSessionId() {
#ifdef WIN32
        HANDLE token;
        DWORD len;
        BOOL result = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token);
        if(result)
        { /* NT */
            GetTokenInformation(token, TokenStatistics, NULL, 0, &len);
            LPBYTE data = new BYTE[len];
            GetTokenInformation(token, TokenStatistics, data, len, &len);
            LUID uid = ((PTOKEN_STATISTICS)data)->AuthenticationId;
            delete [ ] data;
            ostringstream myString;
            myString << hex << uid.HighPart;
            myString << hex << uid.LowPart;
            return myString.str();
        } /* NT */
        else
        { /* 16-bit OS */
            return "16bit";
        } /* 16-bit OS */
#else
        return hostappid();
#endif
    }

    inline
    long microns_from_secs(double secs) {
        return long(secs * 1000000);
    }

    inline
    double secs_from_microns(long microns) {
        return double(microns) / 1000000;
    }

    void precision_sleep(double theDuration){
#ifdef WIN32
        Sleep((DWORD)theDuration);
#else
        asl::Time startTime;
        double stopTime = double(startTime) + theDuration;
        long remainTime = microns_from_secs(theDuration);
        while ( remainTime > 2000) {
            usleep(1000);
            asl::Time currentTime;
            remainTime = microns_from_secs(stopTime - double(currentTime));
        }
        usleep(remainTime);
#endif
    }


    std::string
    expandEnvironment(const std::string & theString) {
        std::string myExpandedString(theString);
        std::string::size_type myOpenPos;
        std::string::size_type myClosePos = 0;

        while (( myOpenPos = myExpandedString.find("${", myClosePos)) != std::string::npos) {
            myClosePos = myExpandedString.find("}", myOpenPos);
            if (myClosePos == std::string::npos) {
                throw ParseException(std::string("Missing right brace in string '") + theString + "'", "expandEnvironment()");
            }
            std::string myVarName = myExpandedString.substr(myOpenPos+2, myClosePos - myOpenPos - 2);
            for (int i= 0; i < myVarName.size(); ++i) {
                if (!is_alpha(myVarName[i]) && !is_underscore(myVarName[i]) && !is_digit(myVarName[i])) {
                    throw ParseException(std::string("Unexpected character in variable name '") + myVarName + "'", "expandEnvironment()");
                }
            }

            DB(AC_TRACE << "myOpenPos:" << myOpenPos << " myClosePos:" << myClosePos << " myVarName:" << myVarName << endl);
            std::string myValue;
            if (get_environment_var(myVarName, myValue)) {
                DB(AC_TRACE << "     myValue: " << myValue << endl;)
                std::string myReplacement(myValue);
                myExpandedString.replace(myOpenPos, myClosePos-myOpenPos+1, myReplacement);
                myClosePos = myOpenPos + myReplacement.size();
            } else {
                DB(AC_TRACE << "     no value " << endl )
                myExpandedString.erase(myOpenPos, myClosePos-myOpenPos+1);
                myClosePos = myOpenPos;
            }
        }
        DB(AC_TRACE << "      expanded: '" << myExpandedString << "'" << endl;)
        return myExpandedString;
    }

    bool
    get_environment_var(const std::string & theVariable, std::string & theValue) {
         const char * myValue = getenv(theVariable.c_str());
         if (myValue) {
             theValue = myValue;
             return true;
         } else {
             return false;
         }
    }

    void
    set_environment_var(const std::string & theVariable, const std::string & theValue) {
#ifdef WIN32
        _putenv((theVariable + "=" + theValue).c_str());
#else
        setenv(theVariable.c_str(), theValue.c_str(), true);
#endif
    }

    unsigned long getThreadId() {
#ifdef LINUX
        return getpid();
#else
        return GetCurrentThreadId();
#endif
    }


#ifdef WIN32
 bool
 hResultIsOk(HRESULT hr, std::string & theMessage) 
 {
     if (FAILED(hr)) {
         if (FACILITY_WINDOWS == HRESULT_FACILITY(hr)) {
             hr = HRESULT_CODE(hr);
         }
         char * szErrMsg;
         if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
             NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
             (LPTSTR)&szErrMsg, 0, NULL) != 0)
         {
             theMessage = szErrMsg;
             LocalFree(szErrMsg);
         } else {
             char myErrorNumber[1024];
             sprintf(myErrorNumber, "Error # %#x", hr);
             theMessage = myErrorNumber;
         }
         AC_DEBUG << "Error for hr " << hr << " : " << theMessage;
         return false;
     }
     return true;
 }

#endif


} //Namespace asl


