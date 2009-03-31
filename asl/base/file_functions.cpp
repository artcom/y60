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
//    $RCSfile: file_functions.cpp,v $
//
//   $Revision: 1.23 $
//
// Description: file helper functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "file_functions.h"

#include <asl/base/begin_end.h>
#include "os_functions.h"
#include "string_functions.h"
#include "error_functions.h"
#include "Logger.h"
#include "Path.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <sys/utime.h>
#include <shlobj.h>
#include <errno.h>
#include <io.h> /* _findfirst and _findnext set errno if they return -1 */
#include <direct.h>
#else
#include <libgen.h>
#include <unistd.h>
#include <utime.h>
#endif

#define DB(x) // x

using namespace std;

namespace asl {

#ifdef _WIN32
#  define STAT   _stat
#  define STATF  _stat
#else
#  define STAT    stat
#  define STATF   stat
#endif


    std::string getFilenamePart(const std::string & theFileName) {
#ifdef _WIN32
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
        string myProtocol;
        string myPath;
        parseURI(theFileName, &myProtocol, 0, &myPath);
        
#ifdef _WIN32
        char drive[_MAX_DRIVE];
        char dir[_MAX_DIR];
        char fname[_MAX_FNAME];
        char ext[_MAX_EXT];

        _splitpath( myPath.c_str(), drive, dir, fname, ext );
        std::string myDirName = std::string(drive)+std::string(dir);
        if (myDirName.empty()) {
            myDirName = "./";
        }
        
        // Remove trailing double slashes
        if (myDirName.length() > 1 &&
            myDirName.at(myDirName.length()-1) == '/' && 
            myDirName.at(myDirName.length()-2) == '/') 
        {
            myDirName.resize(myDirName.length() - 1);
        }
#else
        std::string myDirName;
        if (! myPath.empty() ) {
            if (myPath.at(myPath.length()-1) == '/') {
                return myPath;
            }

            char * myBuffer = strdup(myPath.c_str());
            myDirName = dirname(myBuffer);
            free(myBuffer);
            if (!myDirName.empty() &&
                myDirName.at(myDirName.length()-1) != '/')
            {
                myDirName += "/";
            }
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

    std::string getExtension(const std::string & myPath)
    {
        std::string::size_type myDotPos = myPath.rfind(".");
        if (myDotPos != std::string::npos) {

            std::string::size_type mySlashPos = myPath.rfind("/");
            if (mySlashPos == std::string::npos) {
                mySlashPos = myPath.rfind("\\");
            }
            if (mySlashPos != std::string::npos && mySlashPos > myDotPos) {
                return "";
            }

            return myPath.substr(myDotPos+1);
        }
        return "";
    }

    std::string removeExtension(const std::string & theFileName) { 
        std::string::size_type myDotPos = theFileName.rfind(".");
        if (myDotPos != std::string::npos) {
            std::string::size_type mySlashPos = theFileName.rfind("/");
#ifdef _WIN32
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

    /// returns the Host:Port part of an URI
    // URI's are formated as protocol://login/path
    // where login = [username:password@]hostport
    // and hostport = host[:port]
    void parseURI(const std::string & theURI, std::string * theProtocol, std::string * theLogin, std::string * thePath) {
        std::string::size_type myProtocolLoginDelimit = theURI.find("://");
        if (myProtocolLoginDelimit == string::npos && theURI[1] == ':') {
            // we have a DOS-type path 'C:\foo\bar'
            if (thePath) {
                *thePath = theURI;
            }
            return;
        }

        string::size_type myLoginStartDelimiter = string::npos; // position of "//"
        string::size_type myLoginPathDelimit = string::npos;

        if (myProtocolLoginDelimit != string::npos) {
            if (theProtocol) {
                *theProtocol = theURI.substr(0, myProtocolLoginDelimit);
            }
            myLoginStartDelimiter = myProtocolLoginDelimit+1;
        } else if (theURI.substr(0,2) == "\\\\")  {
            // we have MS-URI  \\server\path
            myLoginStartDelimiter = 0;
        }

        if (myLoginStartDelimiter != string::npos) {
            std::string::size_type myLoginStartPos = myLoginStartDelimiter+2;

            myLoginPathDelimit = theURI.find("/", myLoginStartPos );

            if (theLogin) {
                if (myLoginPathDelimit != string::npos) {
                    *theLogin = theURI.substr(myLoginStartPos, myLoginPathDelimit-myLoginStartPos);
                } else {
                    *theLogin = theURI.substr(myLoginStartPos);
                }
            }
        }

        std::string::size_type myPathStartPos = (myLoginPathDelimit == std::string::npos ? 0 : myLoginPathDelimit); 
        if (thePath) {
            *thePath = theURI.substr(myPathStartPos);
        }

        return;
    }

    /// returns the Host part of an URI
    std::string getHostPortPart(const std::string & theURI) {
        std::string myLogin;
        parseURI(theURI, 0, &myLogin, 0);
        string::size_type myCredentialServerDelimiter = myLogin.find("@");
        if (myCredentialServerDelimiter == string::npos) {
            return myLogin;
        }
        return myLogin.substr(myCredentialServerDelimiter+1);
    }

    /// returns the Host part of an URI
    std::string getHostPart(const std::string & theURI) {
        string myHostPort = getHostPortPart(theURI);
        string::size_type myHostPortDelimiter = myHostPort.find(":");
        if (myHostPortDelimiter == string::npos) {
            return myHostPort;
        }
        return myHostPort.substr(0, myHostPortDelimiter);
    }

    unsigned splitPaths(const std::string & theDelimitedPaths,
            std::vector<std::string> & thePathVector)
    {
        thePathVector.clear();
        std::string mySearchPath = expandEnvironment(theDelimitedPaths);

#ifdef _WIN32
        static const char * myDelimiters = ";";
#else
        static const char * myDelimiters = ";:";
#endif

        // XXX maybe use splitString from string_functions.cpp
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
#ifdef LINUX
        if (theFileName.find("/") == std::string::npos) {
            // fix for shared library loading on linux which has a special
            // treatment for filenames without slashes 
            std::string myLocalFileName("./" + theFileName);
            if (fileExists(myLocalFileName)) {
                return myLocalFileName;
            }
        }
#endif
       if (fileExists(theFileName)) {
             return theFileName;
       }
       std::vector<std::string> mySearchPaths;
       splitPaths(theSearchPath, mySearchPaths);

       for (std::vector<std::string>::size_type i = 0; i < mySearchPaths.size(); ++i) {
            std::string myPath = mySearchPaths[i];
            if (myPath.size()) {
                if (myPath.at(myPath.size() - 1) != '/') {
                    myPath += "/";
                }
                std::string myFileWithPath = myPath + theFileName;
                if (fileExists(myFileWithPath)) {
                    return myFileWithPath;
                }
            } 
       }
       return "";
    }

    /// read a complete file into a string
    bool readFile(const std::string & theUTF8Filename, std::string & theContent) {
        std::ifstream inFile(Path(theUTF8Filename, UTF8).toLocale().c_str(), std::ios::binary);
        if (inFile) {
            std::vector<char> myBuffer(65536);
            theContent.resize(0);
            while (inFile) {
                inFile.read(begin_ptr(myBuffer),myBuffer.size());
                theContent.append(begin_ptr(myBuffer),inFile.gcount());
            }
        } else {
            return false;
        }
        return inFile.eof() && !inFile.bad();
    }
    
    /// read lines from a text file and puts it into a vector of strings
    bool readFile(const std::string & theUTF8Filename, std::vector<std::string> & theLines) {
        std::ifstream inFile(Path(theUTF8Filename, UTF8).toLocale().c_str());
        if (inFile) {
            while (inFile) {
                theLines.push_back(std::string());
                getline(inFile,theLines.back());
             }    
        } else {
            return false;
        }
        if (theLines.size() && theLines.back().size() == 0) {
            theLines.pop_back();
        }
        return inFile.eof() && !inFile.bad();
    }

    bool readFile(const std::string & theUTF8Filename, asl::ResizeableBlock & theContent) {
        unsigned long myFileSize = getFileSize(theUTF8Filename);
        theContent.resize(myFileSize);
        DB(AC_TRACE << "myFileSize = " << myFileSize);
        std::ifstream inFile(Path(theUTF8Filename, UTF8).toLocale().c_str(), std::ios::binary);
        if (!inFile) {
            return false;
        }

        //const unsigned CHUNK_SIZE = 65536;
        unsigned long myPos = 0;
        while (inFile && myPos < theContent.size()) {
            unsigned myChunkSize = theContent.size() - myPos;
            //myChunkSize = (myChunkSize > CHUNK_SIZE ? CHUNK_SIZE : myChunkSize); // UH: temporarily disabled
            inFile.read(&(*(theContent.strbegin()+myPos)), myChunkSize);
            myPos += inFile.gcount();
        }
        DB(AC_TRACE << "ready, myPos = " << myPos);
        if (theContent.size()!=myPos) {
            theContent.resize(myPos);
            return false;
        }
        return true;
    }

    std::string readFile(const std::string & theUTF8Filename) {
        std::string myResult;
        if (!readFile(theUTF8Filename,myResult)) {
            myResult.resize(0);
        };
        return myResult;
    };

    bool writeFile(const std::string & theUTF8Filename, const std::string & theContent) {
        std::ofstream outFile(Path(theUTF8Filename, UTF8).toLocale().c_str(), std::ios::binary);
        if (outFile) {
            outFile << theContent;
            return outFile != 0;
        }
        return false;
    }

    bool writeFile(const std::string & theUTF8Filename, const asl::ReadableBlock & theContent) {
        std::ofstream outFile(Path(theUTF8Filename, UTF8).toLocale().c_str(), std::ios::binary);
        if (outFile) {
            outFile.write(theContent.strbegin(),theContent.size());
            return outFile != 0;
        }
        return false;
    }

    bool deleteFile(const std::string & theUTF8Filename) {
#ifdef _WIN32
        return _unlink(Path(theUTF8Filename, UTF8).toLocale().c_str()) == 0;
#else
        return unlink(Path(theUTF8Filename, UTF8).toLocale().c_str()) == 0;
#endif
    }

    bool
    copyFile(const std::string & theOldFileName, const std::string & theNewFileName) {
        asl::Block myContent;
        if (!readFile(theOldFileName, myContent)) {
            return false;
        }
        return writeFile(theNewFileName, myContent);
    }

    bool
    moveFile(const std::string & theOldFileName, const std::string & theNewFileName) {
        Path myOldFileName(theOldFileName, UTF8);
        Path myNewFileName(theNewFileName, UTF8);
        LAST_ERROR_TYPE myError = 0;
#ifdef _WIN32
        if (MoveFileEx(myOldFileName.toLocale().c_str(), 
                       myNewFileName.toLocale().c_str(), MOVEFILE_COPY_ALLOWED
                                                        +MOVEFILE_REPLACE_EXISTING) == 0)
        {
            myError = lastError();
        }
#else
        if (rename(myOldFileName.toLocale().c_str(), myNewFileName.toLocale().c_str()) != 0) {
            myError = lastError();
        }
#endif
        if (myError) {
            AC_ERROR << "Error while moveFile('" << myOldFileName << "','" << myNewFileName << "')";
            AC_ERROR << "    " << errorDescription(myError);
            return false;
        }
        return true;
    }

    bool
    setLastModified(const std::string & theUTF8Filename, time_t theModificationDate) {
#ifdef _WIN32
        _utimbuf myTimeBuf;
#else
        utimbuf myTimeBuf;
#endif
        myTimeBuf.actime = theModificationDate;
        myTimeBuf.modtime = theModificationDate;
#ifdef _WIN32
        return _utime(Path(theUTF8Filename, UTF8).toLocale().c_str(), &myTimeBuf)==0;
#else
        return utime(Path(theUTF8Filename, UTF8).toLocale().c_str(), &myTimeBuf)==0;
#endif
    }

    time_t
    getLastModified(const std::string & theUTF8Filename) {
        struct STAT myStat;
        if (STAT(Path(theUTF8Filename, UTF8).toLocale().c_str(), &myStat) == -1) {
            return -1;
        }
        return myStat.st_mtime;
    }

   std::vector<std::string> getDirectoryEntries(const std::string & theUTF8Path) {
        std::vector<std::string> myDirEntries;

        DIR * myDirHandle = opendir(Path(theUTF8Path, UTF8).toLocale().c_str());
        if (!myDirHandle) {
            throw OpenDirectoryFailed(std::string("thePath='") + theUTF8Path + "',reason:"+asl::errorDescription(lastError()), PLUS_FILE_LINE);
        }
        struct dirent *dir_entry;
        while((dir_entry = readdir(myDirHandle)) != 0) {
            if (std::string("..")!= dir_entry->d_name && std::string(".") != dir_entry->d_name) {
                myDirEntries.push_back(Path(dir_entry->d_name, Locale).toUTF8());
            }
        }
        closedir(myDirHandle);
        return myDirEntries;
    }

    void
    createDirectory(const std::string & theUTF8Path) {
        Path myPath(theUTF8Path, UTF8);
#ifdef _WIN32
        if (CreateDirectory(myPath.toLocale().c_str(), 0) == 0) {
#else
        if (mkdir(myPath.toLocale().c_str(),0777) != 0) {
#endif
            throw CreateDirectoryFailed(std::string("theDirectory=")+theUTF8Path+" ,reason:"+asl::errorDescription(lastError()), PLUS_FILE_LINE);
        };
    }

    void
    removeDirectory(const std::string & theUTF8Path) {
        Path myPath(theUTF8Path, UTF8);
#ifdef _WIN32
        if (RemoveDirectory(myPath.toLocale().c_str()) == 0) {
#else
        if (rmdir(myPath.toLocale().c_str()) != 0) {
#endif
            throw RemoveDirectoryFailed(std::string("theDirectory=")+theUTF8Path+" ,reason:"+asl::errorDescription(lastError()), PLUS_FILE_LINE);
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
#ifdef _WIN32
        char myTempBuffer[MAX_PATH];
        GetTempPath(MAX_PATH, myTempBuffer);
        Path myTempPath(myTempBuffer, Locale);
#else
        Path myTempPath("/tmp/", Locale);
#endif
        return myTempPath.toUTF8();
    }

    std::string getAppDataDirectory(const std::string & theAppName) {
        std::string myAppDirectory;
#ifdef _WIN32
        char myTempBuffer[MAX_PATH];
        //SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, myTempBuffer);
        SHGetSpecialFolderPath(NULL, myTempBuffer, CSIDL_APPDATA, true);
        myAppDirectory = Path(myTempBuffer, Locale).toUTF8()+"/"+theAppName;
#else
        myAppDirectory = Path(expandEnvironment("${HOME}"), Locale).toUTF8()+"/."+theAppName;
#endif
        if (!fileExists(myAppDirectory)) {
            createDirectory(myAppDirectory);
        }
        return myAppDirectory+"/";
    }

    std::string getAppDirectory() {
        std::string strAppDirectory;
#ifdef _WIN32
        char szAppPath[MAX_PATH] = "";

        ::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
        // Extract directory
        strAppDirectory = Path(szAppPath, Locale).toUTF8();
        strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind("\\"));
#else
#ifdef LINUX
        char buffer[256];
        char appPath[4096];
        snprintf(buffer, sizeof(buffer), "/proc/%d/exe", getpid());
        int myBufferSize = readlink(buffer, appPath, sizeof(appPath)-1); // room for nul char
        if (myBufferSize < 0) { 
            throw GetAppDirectoryFailed(std::string("readlink failed for '") + buffer + "'", PLUS_FILE_LINE);
        }
        appPath[myBufferSize] = 0;
        strAppDirectory = std::string(getDirectoryPart(Path(appPath, Locale).toUTF8()));
#else
#ifdef OSX
        CFBundleRef myAppsBundle = CFBundleGetMainBundle();
        if (myAppsBundle == NULL) throw GetAppDirectoryFailed(std::string("CFBundleGetMainBundle failed") 
                , PLUS_FILE_LINE);

        //        CFURLRef myBundleURL = CFBundleCopyExecutableURL(myAppsBundle);
        CFURLRef myBundleURL = CFBundleCopyBundleURL(myAppsBundle);
        if (myBundleURL == NULL) throw GetAppDirectoryFailed(std::string("CFBundleCopyBundleURL failed, error=") 
                , PLUS_FILE_LINE);

        CFStringRef myPath = CFURLCopyFileSystemPath(myBundleURL,  kCFURLPOSIXPathStyle);
        CFRelease(myBundleURL);
        char * myPathCP = NewPtr(CFStringGetLength(myPath)+1);
        CFStringGetCString(myPath, myPathCP, CFStringGetLength(myPath)+1, kCFStringEncodingUTF8);
        strAppDirectory = myPathCP;
        CFRelease(myPath);
        DisposePtr(myPathCP);
        return strAppDirectory; 
#endif
#endif
#endif
        return strAppDirectory;
    }

    std::string 
    getCWD() {
#ifdef _WIN32    
        return std::string(_getcwd( NULL, 0 ));
#else
        char myBuffer[1024];
        getcwd( myBuffer, 1024);
        return std::string(myBuffer);
#endif    
    }

// TODO: deal with degenerate cases like "C:\" or "/" (root)
std::string 
normalizeDirectory(const std::string & theDirectory, bool stripTrailingSlash) { 
    std::string myDirectory(theDirectory);
    // replace backslashes with forward slashes
    std::string::size_type myBackslashPos;
    while (std::string::npos != (myBackslashPos = myDirectory.find("\\"))) {
        myDirectory.replace(myBackslashPos, 1, "/");
    }
    // replace double-slashes with single slashes        
    std::string::size_type myDoubleSlash;
    while (std::string::npos != (myDoubleSlash = myDirectory.find("//"))) {
        myDirectory.replace(myDoubleSlash, 2, "/");
    }
    // strip all trailing slashes
    while (stripTrailingSlash) {
        size_t myLen = myDirectory.size();
        if (myLen == 0) {
            break;
        }
        if (myDirectory[myLen-1] == '/') {
            myDirectory = myDirectory.substr(0, myLen-1);
        } else {
            break;
        }
    }
    return myDirectory;    
}

bool isDirectory(const std::string & theUTF8Path) {
    DIR * myDirHandle = opendir(Path(theUTF8Path, UTF8).toLocale().c_str());
    if (myDirHandle) {
        closedir(myDirHandle);
        return true;
    }
    return false;
}

bool listDirectory(const std::string & theUTF8Path, std::vector<string> & theContent) {
    DIR * myDirHandle = opendir(Path(theUTF8Path, UTF8).toLocale().c_str());
    if (myDirHandle) {
        theContent.resize(0);
        struct dirent * myItem = 0;
        while ( (myItem = readdir(myDirHandle)) != 0) {
            theContent.push_back(myItem->d_name);
        }
        closedir(myDirHandle);
        return true;
    }
    return false;
}

std::string
evaluateRelativePath(const std::string & theBaseDirectory,
                     const std::string & theAbsolutePath,
                     bool theForceRelativePathFlag)
{
    // split into components
    std::vector<std::string> myBaseParts = splitString(normalizeDirectory(theBaseDirectory, true), "/");
    std::vector<std::string> myAbsoluteParts = splitString(normalizeDirectory(theAbsolutePath, true), "/");

    // count matching parts
    unsigned numParts = (myBaseParts.size() < myAbsoluteParts.size() ? myBaseParts.size() : myAbsoluteParts.size());
    unsigned sameCount = 0;
    for (unsigned i = 0; i < numParts; ++i) {
#ifdef _WIN32
        // why should we use something standard like 'strcasecmp' when we can make up our own names instead?
        if (stricmp(myBaseParts[i].c_str(), myAbsoluteParts[i].c_str()) != 0)
#else
        if (myBaseParts[i] != myAbsoluteParts[i])
#endif
        {
            break;
        }
        ++sameCount;
    }
    if (theForceRelativePathFlag == false && sameCount == 0) {
        return theAbsolutePath;
    }

    // assemble relative path
    std::string myRelativePath = "";
    for (unsigned i = sameCount; i < myBaseParts.size(); ++i) {
        if (i > sameCount) {
            myRelativePath += "/";
        }
        myRelativePath += "..";
    }
    if (myRelativePath.size() == 0) {
        myRelativePath = ".";
    }
    for (unsigned i = sameCount; i < myAbsoluteParts.size(); ++i) {
        myRelativePath += "/";
        myRelativePath += myAbsoluteParts[i];
    }

    return myRelativePath;
}

#ifdef OSX
#  define STAT64   stat
#  define STAT64F  stat
#endif
#ifdef LINUX
#  define STAT64   stat64
#  define STAT64F  stat64
#endif
#ifdef _WIN32
#  define STAT64   __stat64
#  define STAT64F  _stat64
#endif

/// returns true if file or directory exists
bool fileExists(const std::string& theUTF8Filename) {
    struct STAT64 myStat;
    return STAT64F(Path(theUTF8Filename, UTF8).toLocale().c_str(), &myStat) != -1;
}
// Warning: off_t is 32 bit (at least) under windows. This function will
// return incorrect values for files > 2 gb.
off_t getFileSize(const std::string& theUTF8Filename) {
    struct STAT64 myStat;
    if (STAT64F(Path(theUTF8Filename, UTF8).toLocale().c_str(), &myStat) != -1) {
        return static_cast<off_t>(myStat.st_size);
    };
    throw asl::IO_Failure("getFileSize","can't stat file");
}


} // namespace asl

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32

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

