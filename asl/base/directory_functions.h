/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
    Declaration of POSIX directory browsing functions and types for Win32.

    Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
    History: Created March 1997. Updated June 2003.
    Rights:  See end of file.
   __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_directory_functions_
#define _included_asl_directory_functions_

#include "Exception.h"

#include <vector>
#include <string>




namespace asl {
/*! \addtogroup aslbase */
/* @{ */
    DEFINE_EXCEPTION(FileNotFoundException, asl::Exception);
    DEFINE_EXCEPTION(DirectoryException, asl::Exception);

    std::vector<std::string> getDirectoryEntries(const std::string & thePath);
    
    void createDirectory(const std::string & theDirectory);
    std::string getTempDirectory();
    std::string getAppDataDirectory(const std::string & theAppName);
    std::string getAppDirectory();
    bool isDirectory(const std::string & theDirectory);
    std::string stripTrailingSlashes(const std::string & theDirectory);
/* @} */
}

#ifdef __cplusplus
extern "C"
{
        
#endif
#ifdef WIN32

typedef struct DIR DIR;

struct dirent
{
    char *d_name;
};

DIR           *opendir(const char *);
int           closedir(DIR *);
struct dirent *readdir(DIR *);
void          rewinddir(DIR *);

#else // Linux and Mac Os X
#   include <dirent.h>
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

#ifdef __cplusplus
} // extern C
#endif



#endif

