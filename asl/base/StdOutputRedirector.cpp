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


// own header
#include "StdOutputRedirector.h"

#include <iostream>
#include <sstream>
#include <cstring>

#include "TimeStreamFormater.h"
#include "file_functions.h"

#include "Arguments.h"
#include "os_functions.h"
#include "string_functions.h"
#include "Time.h"

using namespace std;

namespace asl {

    const char * LOG_WRAPAROUND_FILESIZE = "AC_LOG_WRAPAROUND_FILESIZE";
    const char * LOG_WRAPAROUND_CHECK_SEC = "AC_LOG_WRAPAROUND_CHECK_SEC";
    const char * LOG_CREATE_ON_EACH_RUN   = "AC_CREATE_LOG_ON_EACH_RUN";
    const char * LOG_REMOVE_OLD_ARCHIVE   = "AC_LOG_REMOVE_OLD_ARCHIVE";

    const char * ourAppStartMessage = "--------- Application startup, archive filename: ";

    std::string expandString(const std::string & theString, std::string & theShortname) {

        std::string myString = theString;
        theShortname = theString;
        std::string::size_type pos;

        // hostname
        pos = myString.find("%h");
        if (pos != std::string::npos) {
            myString.replace(pos, 2, asl::hostname());
            theShortname.replace(pos, 2, asl::hostname());
        }

        // date
        asl::Time now;
        const char * myFormatString = "%Y-%M-%D-%h-%m";
        std::ostringstream myTimeString;
        myTimeString << asl::formatTime(myFormatString) << now;

        pos = myString.find("%d");
        if (pos != std::string::npos) {
            myString.replace(pos, 2, myTimeString.str());
            theShortname.replace(pos, 2, "");
        } else {
            myString = myString.append(myTimeString.str());
        }
        return myString;
    }

    string
    getCurrentTimeString() {
        asl::Time now;
        const char * myFormatString = "%Y-%M-%D-%h-%m";
        std::ostringstream myTimeString;
        myTimeString << asl::formatTime(myFormatString) << now;
        return myTimeString.str();
    }

    StdOutputRedirector::StdOutputRedirector() : _myOutFile(0), _myOriginalCoutBuffer(0), _myOriginalCerrBuffer(0),
                                                 _myMaximumFileSize(0), _myOutputFilename(""), _myStartTime(-1),
                                                 _myOldArchiveFilename(""), _myRemoveOldArchiveFlag(false),
                                                 _myLogInOneFileFlag(false), _myFileSizeCheckFrequInSec(10)
    {}

    StdOutputRedirector::~StdOutputRedirector()  {
        std::cout.rdbuf(_myOriginalCoutBuffer);
        std::cerr.rdbuf(_myOriginalCerrBuffer);
    }

    void
    StdOutputRedirector::init(const Arguments & theArguments) {
        if (theArguments.haveOption("--std-logfile")) {
            const char * myEnv = ::getenv(LOG_WRAPAROUND_FILESIZE);
            if (myEnv) {
                string mylogFileSize(myEnv);
                _myMaximumFileSize = asl::as<long>(mylogFileSize);
            }
            myEnv = ::getenv(LOG_CREATE_ON_EACH_RUN);
            if (myEnv) {
                string myTmpStr(myEnv);
                _myLogInOneFileFlag = !(strcmp(toLowerCase(myTmpStr).c_str(), "true") == 0);
            }
            myEnv = ::getenv(LOG_REMOVE_OLD_ARCHIVE);
            if (myEnv) {
                string myTmpStr(myEnv);
                _myRemoveOldArchiveFlag = (strcmp(toLowerCase(myTmpStr).c_str(), "true") == 0);
            }

            myEnv = ::getenv(LOG_WRAPAROUND_CHECK_SEC);
            if (myEnv) {
                string myTmpStr(myEnv);
                _myFileSizeCheckFrequInSec = asl::as<long>(myTmpStr);
            }

            std::string myFilenameWithTimestamp = expandString(theArguments.getOptionArgument("--std-logfile"),
                                     _myOutputFilename);
            if (!_myLogInOneFileFlag) {
                _myOutputFilename = myFilenameWithTimestamp;
            }
            // for syncing c like stderr & c++ cerr
            // default is true, not syncing is supposted to be faster, so maybe we should disable it
            //ios_base::sync_with_stdio(false);

            redirect();

            // write a timestamp
            cout <<  ourAppStartMessage << myFilenameWithTimestamp << endl;
            cout << "Timestamp: " << getCurrentTimeString() << endl;
            cout << "---------" << endl;

            // remove all but latest archives
            if (_myRemoveOldArchiveFlag) {
                removeoldArchives();
            }

        }
    }

    void
    StdOutputRedirector::removeoldArchives() {
        vector<std::string> myFiles = getDirectoryEntries(getDirectoryPart(_myOutputFilename));
        // search the newest
        int myNewestIndex = -1;
        time_t myHighestTimeStamp = 0;
        vector<int> myFilesToDelete;
        for (vector<std::string>::size_type myFileIndex = 0; myFileIndex != myFiles.size(); myFileIndex++) {
            string myFilename = myFiles[myFileIndex];//getDirectoryPart(_myOutputFilename) + myFiles[myFileIndex];
            string mySearchString(removeExtension(_myOutputFilename) + "logarchive_" );
            size_t myPos = myFilename.rfind(mySearchString.c_str() ,0, mySearchString.size());
            if (myPos != string::npos) {
                time_t myTimeStamp = getLastModified(myFilename);
                if (myTimeStamp > myHighestTimeStamp) {
                    myHighestTimeStamp = myTimeStamp;
                    myNewestIndex = myFileIndex;
                }
                myFilesToDelete.push_back(myFileIndex);
            }
        }
        for (vector<std::string>::size_type myFileIndex = 0; myFileIndex != myFilesToDelete.size(); myFileIndex++) {
            if (myFilesToDelete[myFileIndex] != myNewestIndex ) {
                deleteFile(getDirectoryPart(_myOutputFilename) + myFiles[myFilesToDelete[myFileIndex]]);
            } else {
                _myOldArchiveFilename = myFiles[myNewestIndex];
            }
        }
    }

    void
    StdOutputRedirector::redirect() {
        if (_myLogInOneFileFlag) {
            _myOutputStreamOut.open(_myOutputFilename.c_str(), ofstream::app | ofstream::out);
        } else {
            _myOutputStreamOut.open(_myOutputFilename.c_str(), ofstream::trunc | ofstream::out);
        }
        _myOutFile = _myOutputStreamOut.rdbuf();
        _myOriginalCoutBuffer = std::cout.rdbuf(_myOutFile);
        _myOriginalCerrBuffer = std::cerr.rdbuf(_myOutFile);
    }

    void
    StdOutputRedirector::checkForFileWrapAround()  {
        if (_myMaximumFileSize > 0) {
            if (_myStartTime == -1) {
                _myStartTime = (long long)asl::Time().millis();
            }
            long long myDelta = asl::Time().millis() - _myStartTime;
            if (myDelta > (_myFileSizeCheckFrequInSec * 1000)) {
                long myCurrentSize = getFileSize(_myOutputFilename);
                if (myCurrentSize >= _myMaximumFileSize) {
                    _myOutputStreamOut.close();
                    // remove old archive
                    if (_myLogInOneFileFlag && _myRemoveOldArchiveFlag &&
                        _myOldArchiveFilename != "" &&  fileExists(_myOldArchiveFilename))
                    {
                        deleteFile(_myOldArchiveFilename);
                    }
                    // rename current log to archive version
                    string myNewFilename = removeExtension(_myOutputFilename) + "logarchive_" +
                                                           getCurrentTimeString() + (".log");
#ifdef _WIN32
                    MoveFileEx(_myOutputFilename.c_str(),
                               myNewFilename.c_str(),
                               MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
#else
                    rename(_myOutputFilename.c_str(), myNewFilename.c_str());
#endif
                    _myOldArchiveFilename = myNewFilename;
                    redirect();
                }
                _myStartTime = (long long)asl::Time().millis();
            }
        }
    }

}

