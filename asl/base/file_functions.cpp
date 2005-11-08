/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: file_functions.cpp,v $
//
//   $Revision: 1.23 $
//
// Description: file helper functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "settings.h"
#include "file_functions.h"
#include "os_functions.h"
#include "string_functions.h"

#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#ifdef WIN32
#include <windows.h>
#include <direct.h>
#include <sys/utime.h>
#else
#include <libgen.h>
#include <unistd.h>
#include <utime.h>
#endif

#define DB(x) // x

namespace asl {

#ifdef WIN32
#  define STAT   _stat
#  define STATF  _stat
#else
#  define STAT    stat
#  define STATF   stat
#endif


    std::string getBaseName(const std::string & theFileName) {
#ifdef WIN32
        char drive[_MAX_DRIVE];
        char dir[_MAX_DIR];
        char fname[_MAX_FNAME];
        char ext[_MAX_EXT];

        _splitpath( theFileName.c_str(), drive, dir, fname, ext );
        std::string myBaseName = std::string(fname)+std::string(ext);
#else
        std::string myBaseName;
        if ( ! theFileName.empty()) {
            if (theFileName.at(theFileName.length()-1) == '/') { // Huh? what's that for??? [DS/MS]
                return std::string("");
            }

            char * myBuffer = strdup(theFileName.c_str());
            myBaseName = basename(myBuffer);
            free(myBuffer);
        }
#endif
        return myBaseName;
    }

    std::string getDirName(const std::string & theFileName) {
#ifdef WIN32
        char drive[_MAX_DRIVE];
        char dir[_MAX_DIR];
        char fname[_MAX_FNAME];
        char ext[_MAX_EXT];

        _splitpath( theFileName.c_str(), drive, dir, fname, ext );
        std::string myDirName = std::string(drive)+std::string(dir);
        if (myDirName.empty()) {
            myDirName = "./";
        }
#else
        if (theFileName.at(theFileName.length()-1) == '/') {
            return theFileName;
        }

        char * myBuffer = strdup(theFileName.c_str());
        std::string myDirName(dirname(myBuffer));
        free(myBuffer);

        myDirName += "/";
#endif
        return myDirName;
    }

    std::string getExtension(const std::string & theFileName)
    {
        std::string::size_type myDotPos = theFileName.rfind(".");
        if (myDotPos != std::string::npos) {

            std::string::size_type mySlashPos = theFileName.rfind("/");
            if (mySlashPos == std::string::npos) {
                mySlashPos = theFileName.rfind("\\");
            }
            if (mySlashPos != std::string::npos && mySlashPos > myDotPos) {
                return "";
            }

            return theFileName.substr(myDotPos+1);
        }
        return "";
    }

    std::string removeExtension(const std::string & theFileName) { 
        std::string::size_type myDotPos = theFileName.rfind(".");
        if (myDotPos != std::string::npos) {
            std::string::size_type mySlashPos = theFileName.rfind("/");
#ifdef WIN32
            // on Windows look for both / and backslash
            if (mySlashPos == std::string::npos) {
                mySlashPos = theFileName.rfind("\\");
            }
#endif
            if (mySlashPos != std::string::npos && mySlashPos > myDotPos) {
                return theFileName;
            }

            return theFileName.substr(0, myDotPos);
        }
        return theFileName;
    }

    unsigned
    splitPaths(const std::string & theDelimitedPaths,
               std::vector<std::string> & thePathVector)
    {
        thePathVector.clear();
        std::string mySearchPath = expandEnvironment(theDelimitedPaths);

#ifdef LINUX
        static const char * myDelimiters = ";:";
#else
        static const char * myDelimiters = ";";
#endif

        std::string::size_type myEndPos = mySearchPath.find_first_of(myDelimiters);
        while (myEndPos != std::string::npos) {
            std::string myPath = mySearchPath.substr(0, myEndPos);
            thePathVector.push_back(myPath);
            mySearchPath = mySearchPath.substr(myEndPos + 1);
            myEndPos = mySearchPath.find_first_of(myDelimiters);
        }
        if (!mySearchPath.empty()) {
            thePathVector.push_back(mySearchPath);
        }

        return thePathVector.size();
    }

    std::string searchFile(const std::string & theFileName, const std::string & theSearchPath) {
       if (fileExists(theFileName)) {
             return theFileName;
       }
       std::vector<std::string> mySearchPaths;
       splitPaths(theSearchPath, mySearchPaths);

       for (int i = 0; i < mySearchPaths.size(); ++i) {
            std::string myPath = mySearchPaths[i];

            if (myPath.at(myPath.size() - 1) != '/') {
                myPath += "/";
            }
            std::string myFileWithPath = myPath + theFileName;
            if (fileExists(myFileWithPath)) {
                return myFileWithPath;
            }
        }

        return "";
    }


/// read a complete file into a string

    bool getWholeFile(const std::string & theFileName, std::string & theContent) {
        std::ifstream inFile(theFileName.c_str());
        if (inFile) {
            std::vector<char> myBuffer(65536);
            theContent.resize(0);
            while (inFile) {
                inFile.read(&(*myBuffer.begin()),myBuffer.size());
                theContent.append(&(*myBuffer.begin()),inFile.gcount());
            }
        } else {
            return false;
        }
        return inFile.eof() && !inFile.bad();
    }

    namespace {
        // we don't want to drag in numeric_functions. so we define it here
        unsigned long Minimum(unsigned long a, unsigned long b) {
            if (a > b) return a ; else return b;
        }
    }

    bool getWholeFile(const std::string & theFileName, asl::ResizeableBlock & theContent) {
        const int myChunksize = 65536;
        unsigned long myFileSize = getFileSize(theFileName);
        theContent.resize(myFileSize);
        DB(AC_TRACE << "myFileSize = " << myFileSize);
        std::ifstream inFile(theFileName.c_str(), std::ios::binary);
        unsigned long myPos = 0;
        if (inFile) {
            while (inFile && myPos < theContent.size()) {
                DB(AC_TRACE << "myPos = " << myPos << ", chunk = " << Minimum(myChunksize,theContent.size()-myPos));
                inFile.read(&(*(theContent.strbegin()+myPos)),Minimum(myChunksize,theContent.size()-myPos));
                myPos += inFile.gcount();
            }
        } else {
            return false;
        }
        DB(AC_TRACE << "ready, myPos = " << myPos);
       if (theContent.size()!=myPos) {
            theContent.resize(myPos);
            return false;
        }
        return true;
    }

    std::string getWholeFile(const std::string & theFileName) {
        std::string myResult;
        if (!getWholeFile(theFileName,myResult)) {
            myResult.resize(0);
        };
        return myResult;
    };

    bool putWholeFile(const std::string & theFileName, const std::string & theContent) {
        std::ofstream outFile(theFileName.c_str());
        if (outFile) {
            outFile << theContent;
            return outFile != 0;
        }
        return false;
    }

    bool putWholeFile(const std::string & theFileName, const asl::ReadableBlock & theContent) {
        std::ofstream outFile(theFileName.c_str(), std::ios::binary);
        if (outFile) {
            outFile.write(theContent.strbegin(),theContent.size());
            return outFile != 0;
        }
        return false;
    }

    bool deleteFile(const std::string & theFileName) {
#ifdef WIN32
        return _unlink(theFileName.c_str()) == 0;
#else
        return unlink(theFileName.c_str()) == 0;
#endif
    }

    bool
    moveFile(const std::string& theOldFileName, const std::string & theNewFileName) {
#ifdef WIN32
        return MoveFileEx(theOldFileName.c_str(), theNewFileName.c_str(), MOVEFILE_COPY_ALLOWED) != 0;
#else
        return rename(theOldFileName.c_str(), theNewFileName.c_str())==0;
#endif
    }

    bool
    setLastModified(const std::string & theFilename, time_t theModificationDate) {
#ifdef WIN32
        _utimbuf myTimeBuf;
#else
        utimbuf myTimeBuf;
#endif
        myTimeBuf.actime = theModificationDate;
        myTimeBuf.modtime = theModificationDate;
#ifdef WIN32
        return _utime(theFilename.c_str(), &myTimeBuf)==0;
#else
        return utime(theFilename.c_str(), &myTimeBuf)==0;
#endif
    }

    time_t
    getLastModified(const std::string & theFilename) {
        struct STAT myStat;
        if (STAT(theFilename.c_str(), &myStat) == -1) {
            return -1;
        }
        return myStat.st_mtime;
    }
} //Namespace asl


