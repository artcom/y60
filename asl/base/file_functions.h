/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: file_functions.h,v $
//
//   $Revision: 1.17 $
//
// Description: file helper functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _included_asl_file_functions_
#define _included_asl_file_functions_

#include "Exception.h"
#include "Block.h"
#include "PlugInManager.h"

#include <string>
#include <sys/types.h>
#include <sys/stat.h>

namespace asl {
    
/*! \addtogroup aslbase */
/* @{ */

DEFINE_EXCEPTION(FileNotFoundException, asl::Exception);
DEFINE_EXCEPTION(CreateDirectoryFailed, asl::Exception);
DEFINE_EXCEPTION(RemoveDirectoryFailed, asl::Exception);
DEFINE_EXCEPTION(OpenDirectoryFailed, asl::Exception);

std::vector<std::string> getDirectoryEntries(const std::string & thePath);

/** creates a new directory in an existing directory */
void createDirectory(const std::string & theDirectory);

/** removes an empty directory directory */
void removeDirectory(const std::string & theDirectory);

/** creates a new directory and all necessary parent directories */
void createPath(const std::string & theDirectory);

std::string getTempDirectory();
std::string getAppDataDirectory(const std::string & theAppName);
std::string getAppDirectory();

/// returns true if theDirectory is a readable directory, false otherwise
bool isDirectory(const std::string & theDirectory);
std::string stripTrailingSlashes(const std::string & theDirectory);

/// returns filename without directory     
std::string getFilenamePart(const std::string & theFileName);
/// returns directory without filename     
std::string getDirectoryPart(const std::string & theFileName);

/// returns path shortened by last directory component     
std::string getParentDirectory(const std::string & theDirectoryName);

/// return filename extension, or "" if none was found
std::string getExtension(const std::string & theFileName);

/// returns theFileName without extension
std::string removeExtension(const std::string & theFileName);

/// splits a delimited path list (semicolon or colon delimited) into its components
unsigned splitPaths(const std::string & theDelimitedPaths, std::vector<std::string> & thePathVector);

/// searches a file in a semicolon-seperated list of pathes, 
/// returns the path + file. Embedded environment variables 
/// in the form ${VARNAME} are expanded.
/// @deprecated see asl::PackageManager, which can also search in zip files.
std::string searchFile(const std::string & theFileName, const std::string & theSearchPath);

/// read a complete file into a string
std::string readWholeFile(const std::string& theFileName);
bool readWholeFile(const std::string& theFileName, std::string & theContent);
bool readWholeFile(const std::string& theFileName, asl::ResizeableBlock & theContent);
bool writeWholeFile(const std::string& theFileName, const std::string & theContent);
bool writeWholeFile(const std::string& theFileName, const asl::ReadableBlock & theContent);

bool deleteFile(const std::string& theFileName);
bool moveFile(const std::string& theOldFileName, const std::string & theNewFileName);
bool setLastModified(const std::string & theFilename, time_t theModificationDate);
time_t getLastModified(const std::string & theFilename);

#ifdef OSX
#  define STAT64   stat
#  define STAT64F  stat
#endif
#ifdef LINUX
#  define STAT64   stat64
#  define STAT64F  stat64
#endif
#ifdef WIN32
#  define STAT64   __stat64
#  define STAT64F  _stat64
#endif

/// returns true if file or directory exists
inline
bool fileExists(const std::string& theFileName) {
    struct STAT64 myStat;
    return STAT64F(theFileName.c_str(), &myStat) != -1;
}

DEFINE_EXCEPTION(IO_Failure,asl::Exception)


// Warning: off_t is 32 bit (at least) under windows. This function will
// return incorrect values for files > 2 gb.
inline
off_t getFileSize(const std::string& theFileName) {
    struct STAT64 myStat;
    if (STAT64F(theFileName.c_str(), &myStat) != -1) {
        return static_cast<off_t>(myStat.st_size);
    };
    throw asl::IO_Failure("getFileSize","can't stat file");
}

/* @} */

} //Namespace asl

#undef STAT64
#undef STAT64F
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

