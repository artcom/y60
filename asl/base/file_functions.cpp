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
#include "error_functions.h"
#include "Logger.h"

#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#ifdef WIN32
#include <windows.h>
#include <direct.h>
#include <sys/utime.h>
#include <shlobj.h>
#include <errno.h>
#include <io.h> /* _findfirst and _findnext set errno if they return -1 */
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


    std::string getFilenamePart(const std::string & theFileName) {
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
            if (theFileName.at(theFileName.length()-1) == '/') {  // Huh? what's that for??? [DS/MS]
                // return empty string if theFileName ends with "/"
                return std::string("");
            }

            char * myBuffer = strdup(theFileName.c_str());
            myBaseName = basename(myBuffer);
            free(myBuffer);
        }
#endif
        return myBaseName;
    }

    std::string getDirectoryPart(const std::string & theFileName) {
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
        std::string myDirName;
        if (! theFileName.empty() ) {
            if (theFileName.at(theFileName.length()-1) == '/') {
                return theFileName;
            }

            char * myBuffer = strdup(theFileName.c_str());
            myDirName = dirname(myBuffer);
            free(myBuffer);
            myDirName += "/";
        }

#endif
        return myDirName;
    }

    std::string getParentDirectory(const std::string & theDirectory) {
        std::string myDirName = theDirectory;
         if (myDirName.size()>1) {
            if (myDirName[myDirName.size()-1] == '/') {
                myDirName.erase(myDirName.rfind('/',myDirName.size()-2)+1);
            } else {
                myDirName.erase(myDirName.rfind('/',myDirName.size())+1);
            }
        } else {
            if (myDirName.size()==1) {
                myDirName.resize(0);
            }
        }
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

    bool readFile(const std::string & theFileName, std::string & theContent) {
        std::ifstream inFile(theFileName.c_str(), std::ios::binary);
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

    bool readFile(const std::string & theFileName, asl::ResizeableBlock & theContent) {
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

    std::string readFile(const std::string & theFileName) {
        std::string myResult;
        if (!readFile(theFileName,myResult)) {
            myResult.resize(0);
        };
        return myResult;
    };

    bool writeFile(const std::string & theFileName, const std::string & theContent) {
        std::ofstream outFile(theFileName.c_str(), std::ios::binary);
        if (outFile) {
            outFile << theContent;
            return outFile != 0;
        }
        return false;
    }

    bool writeFile(const std::string & theFileName, const asl::ReadableBlock & theContent) {
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

   std::vector<std::string> getDirectoryEntries(const std::string & thePath) {
        std::vector<std::string> myDirEntries;

        DIR * myDirHandle = opendir(thePath.c_str());
        if (!myDirHandle) {
            throw OpenDirectoryFailed(std::string("thePath='") + thePath + "',reason:"+asl::errorDescription(lastError()), PLUS_FILE_LINE);
        }
        struct dirent *dir_entry;
        while((dir_entry = readdir(myDirHandle)) != 0) {
            if (std::string("..")!= dir_entry->d_name && std::string(".") != dir_entry->d_name) {
                myDirEntries.push_back(dir_entry->d_name);
            }
        }
        closedir(myDirHandle);
        return myDirEntries;
    }

    void
    createDirectory(const std::string & theDirectory) {
#ifdef WIN32
        if (CreateDirectory(theDirectory.c_str(), 0) == 0) {
#else
        if (mkdir(theDirectory.c_str(),0777) != 0) {
#endif
            throw CreateDirectoryFailed(std::string("theDirectory=")+theDirectory+" ,reason:"+asl::errorDescription(lastError()), PLUS_FILE_LINE);
        };
    }

    void
    removeDirectory(const std::string & theDirectory) {
#ifdef WIN32
        if (RemoveDirectory(theDirectory.c_str()) == 0) {
#else
        if (rmdir(theDirectory.c_str()) != 0) {
#endif
            throw RemoveDirectoryFailed(std::string("theDirectory=")+theDirectory+" ,reason:"+asl::errorDescription(lastError()), PLUS_FILE_LINE);
            //AC_WARNING << "removeDirectory failed, theDirectory="<<theDirectory<<" ,reason:"<<asl::errorDescription(lastError()), PLUS_FILE_LINE);
        };
    }

    void
    createPath(const std::string & thePath) {
        std::string myPath = thePath;
        std::vector<std::string> myPathList;
        while (myPath.size()) {
            myPathList.push_back(myPath);
            myPath = getParentDirectory(myPath);
        }
        int i = myPathList.size()-1; 
        for (; i >= 0; --i) {
            std::string myDirName = myPathList[i];
            if (myDirName.empty()) {
                myDirName = "./";
            }
            if (!isDirectory(myDirName)) {
                createDirectory(myDirName);
            }
        }
    }

    std::string getTempDirectory() {
#ifdef WIN32
        char myTempBuffer[MAX_PATH];
        GetTempPath(MAX_PATH, myTempBuffer);
        return std::string(myTempBuffer);
#else
        return std::string("/tmp/");
#endif
    }

    std::string getAppDataDirectory(const std::string & theAppName) {
        std::string myAppDirectory;
#ifdef WIN32
        char myTempBuffer[MAX_PATH];
        //SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, myTempBuffer);
        SHGetSpecialFolderPath(NULL, myTempBuffer, CSIDL_APPDATA, true);
        myAppDirectory = std::string(myTempBuffer)+"/"+theAppName;
#else
        myAppDirectory = expandEnvironment("${HOME}")+"/."+theAppName;
#endif
        if (!fileExists(myAppDirectory)) {
            createDirectory(myAppDirectory);
        }
        return myAppDirectory+"/";
    }

std::string getAppDirectory() {
    std::string strAppDirectory;
#ifdef WIN32
    char szAppPath[MAX_PATH] = "";

    ::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
    // Extract directory
    strAppDirectory = szAppPath;
    strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind("\\"));
#else
    char buffer[256];
    char appPath[4096];
    snprintf(buffer, sizeof(buffer), "/proc/%d/exe", getpid());
    int myBufferSize = readlink(buffer, appPath, sizeof(appPath)-1); // room for nul char
    if (myBufferSize < 0) { 
        throw GetAppDirectoryFailed(std::string("readlink failed for '") + buffer + "'", PLUS_FILE_LINE);
    }
    appPath[myBufferSize] = 0;
    strAppDirectory = std::string(getDirectoryPart(appPath));
#endif
    return strAppDirectory;
}


// TODO: deal with degenerate cases like "C:\" or "/" (root)
std::string 
stripTrailingSlashes(const std::string & theDirectory) { 
    std::string myDirectory(theDirectory);
    while (true) { // strip all trailing slashes
        size_t myLen = myDirectory.size();
        if (myLen == 0) {
            break;
        }
        if (myDirectory[myLen-1] == '/') {
            myDirectory = myDirectory.substr(0, myLen-1);
#ifdef WIN32
        } else if (myDirectory[myDirectory.size()-1] == '\\') {
            myDirectory = myDirectory.substr(0, myLen-1);
#endif
        } else {
            break;
        }
    }
    return myDirectory;    
}

bool isDirectory(const std::string & theDirectory) {
    DIR * myDirHandle = opendir(theDirectory.c_str());
    if (myDirHandle) {
        closedir(myDirHandle);
        return true;
    }
    return false;
}


} // namespace asl

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32

struct DIR
{
    long                handle; /* -1 for failed rewind */
    struct _finddata_t  info;
    struct dirent       result; /* d_name null iff first time */
    char                *name;  /* null-terminated char string */
};

DIR *opendir(const char *name)
{
    DIR *dir = 0;

    if(name && name[0])
    {
        size_t base_length = strlen(name);
        const char *all = /* search pattern must end with suitable wildcard */
            strchr("/\\", name[base_length - 1]) ? "*" : "/*";

        if((dir = (DIR *) malloc(sizeof *dir)) != 0 &&
           (dir->name = (char *) malloc(base_length + strlen(all) + 1)) != 0)
        {
            strcat(strcpy(dir->name, name), all);

            if((dir->handle = (long) _findfirst(dir->name, &dir->info)) != -1)
            {
                dir->result.d_name = 0;
            }
            else /* rollback */
            {
                free(dir->name);
                free(dir);
                dir = 0;
            }
        }
        else /* rollback */
        {
            free(dir);
            dir   = 0;
            errno = ENOMEM;
        }
    }
    else
    {
        errno = EINVAL;
    }

    return dir;
}

int closedir(DIR *dir)
{
    int result = -1;

    if(dir)
    {
        if(dir->handle != -1)
        {
            result = _findclose(dir->handle);
        }

        free(dir->name);
        free(dir);
    }

    if(result == -1) /* map all errors to EBADF */
    {
        errno = EBADF;
    }

    return result;
}

struct dirent *readdir(DIR *dir)
{
    struct dirent *result = 0;

    if(dir && dir->handle != -1)
    {
        if(!dir->result.d_name || _findnext(dir->handle, &dir->info) != -1)
        {
            result         = &dir->result;
            result->d_name = dir->info.name;
        }
    }
    else
    {
        errno = EBADF;
    }

    return result;
}

void rewinddir(DIR *dir)
{
    if(dir && dir->handle != -1)
    {
        _findclose(dir->handle);
        dir->handle = (long) _findfirst(dir->name, &dir->info);
        dir->result.d_name = 0;
    }
    else
    {
        errno = EBADF;
    }
}

#endif
#ifdef __cplusplus
}
#endif

/*

    Copyright Kevlin Henney, 1997, 2003. All rights reserved.

    Permission to use, copy, modify, and distribute this software and its
    documentation for any purpose is hereby granted without fee, provided
    that this copyright and permissions notice appear in all copies and
    derivatives.

    This software is supplied "as is" without express or implied warranty.

    But that said, if there are any problems please get in touch.

*/

