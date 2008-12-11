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
//    $RCSfile: DirectoryPackage.cpp,v $
//   $Revision: 1.2 $
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "DirectoryPackage.h"
#include <asl/base/file_functions.h>
#include <asl/base/MappedBlock.h>

using namespace std;

namespace asl {

DirectoryPackage::DirectoryPackage(const std::string & theDirectory,
                                   bool theIgnoreDotFiles) :
    _myIgnoreDotFiles(theIgnoreDotFiles)
{
    _myDirectory = normalizeDirectory(theDirectory, true);
}

std::string
DirectoryPackage::getAbsolutePath(const std::string & theRelativePath) const
{
    if (_myDirectory.empty()) {
        return theRelativePath;
    }
    if (theRelativePath.empty()) {
        return _myDirectory;
    }
    return _myDirectory + "/" + theRelativePath;
}

DirectoryPackage::FileList
DirectoryPackage::getFileList(const std::string & theSubDir, bool theRecurseFlag) {
    AC_DEBUG << "getFileList dir='" << _myDirectory << "' path='" << theSubDir << "'";
    std::string myDirToSearch = getAbsolutePath(theSubDir);
    FileList myDirList = getDirectoryEntries(myDirToSearch);
    FileList myFileList;
    for (unsigned i = 0; i < myDirList.size(); ++i) {
        if (_myIgnoreDotFiles && myDirList[i][0] == '.') {
            continue;
        }
        std::string myAbsolutePath = myDirToSearch + "/" + myDirList[i];
        string mySubPath = theSubDir.empty() ? myDirList[i] : theSubDir + "/" + myDirList[i];
        if (isDirectory(myAbsolutePath) == false) {
            myFileList.push_back(mySubPath);
        } else if (theRecurseFlag) {
            // recurse into directory
            std::vector<string> mySubdirList = getFileList(mySubPath, theRecurseFlag);
            for (std::vector<string>::size_type i = 0; i < mySubdirList.size(); ++i) {
                myFileList.push_back(mySubdirList[i]);
            }
        }

    }
    return myFileList;
}

std::string
DirectoryPackage::findFile(const std::string & theRelativePath) const {
    AC_DEBUG << "findFile dir='" << _myDirectory << "' path='" << theRelativePath << "'";
    std::string myAbsolutePath = getAbsolutePath(theRelativePath);
    if (fileExists(myAbsolutePath)) {
        return myAbsolutePath;
    }
    return "";
}

Ptr<ReadableStreamHandle>
DirectoryPackage::getStream(const std::string & theRelativePath) {
    return Ptr<ReadableStreamHandle>(new AlwaysOpenReadableFileHandle(getAbsolutePath(theRelativePath)));
}
Ptr<ReadableBlockHandle>
DirectoryPackage::getFile(const std::string & theRelativePath) {
    std::string myPath = getAbsolutePath(theRelativePath);
    return Ptr<ReadableBlockHandle>(new AnyReadableBlockHandle(asl::Ptr<ReadableBlock>(new ConstMappedBlock(myPath)), myPath));
}

}
