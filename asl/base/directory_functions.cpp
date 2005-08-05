/*

    Implementation of POSIX directory browsing functions and types for Win32.

    Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
    History: Created March 1997. Updated June 2003.
    Rights:  See end of file.

*/

#include "directory_functions.h"
#include "os_functions.h"
#include "file_functions.h"

#ifdef WIN32
#include <windows.h>
#include <shlobj.h>
#include <errno.h>
#include <io.h> /* _findfirst and _findnext set errno if they return -1 */
#else
#include <libgen.h>
#endif
#include <stdlib.h>
#include <string.h>

//using namespace std;

namespace asl {

    std::vector<std::string> getDirList(const std::string & thePath) {
        std::vector<std::string> myDirEntries;

        DIR * myDirHandle = opendir(thePath.c_str());
        if (!myDirHandle) {
            throw DirectoryException(std::string("Can not open directory '") + thePath + "'", PLUS_FILE_LINE);
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
            throw DirectoryException(std::string("Create Directory '")+theDirectory+"' failed",PLUS_FILE_LINE);
        };
#else
        if (mkdir(theDirectory.c_str(),0777) != 0) {
            throw DirectoryException(std::string("Create Directory '")+theDirectory+"' failed",PLUS_FILE_LINE);
        }
#endif
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
    char        szAppPath[MAX_PATH] = "";

    ::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);
    // Extract directory
    strAppDirectory = szAppPath;
    strAppDirectory = strAppDirectory.substr(0, strAppDirectory.rfind("\\"));
#else
    char buffer[256];
    char apppath[4096];
    sprintf(buffer, "/proc/%d/exe", getpid());
    readlink(buffer, apppath, 4096);
    strAppDirectory = std::string(dirname(apppath));
#endif
    return strAppDirectory;
}

bool isDirectory(const std::string & theDirectory) {
    DIR * myDirHandle = opendir(theDirectory.c_str());
    if (myDirHandle) {
        closedir(myDirHandle);
        return true;
    }
    return false;
}


} // namespace
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

