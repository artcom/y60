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
//    $RCSfile: ZipPackage.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: unit test template file - change ZipReader to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "ZipPackage.h"

#include <asl/base/Logger.h>
#include <asl/base/file_functions.h>


using namespace std;

namespace asl {

ZipPackage::ZipPackage(const std::string & theZipFilename) :
    _myZipReader(theZipFilename.c_str())
{
    _myZipFilename = normalizeDirectory(theZipFilename, true);
    AC_DEBUG << "opening Zip " << _myZipFilename;
    const ZipReader::Directory & myDirectory = _myZipReader.getDirectory();
    ZipReader::Directory::const_iterator it;
    for (it = myDirectory.begin(); it != myDirectory.end(); ++it) {
        _myFileList.push_back(it->filename.toUTF8());
    }
}

ZipPackage::~ZipPackage() {
    AC_DEBUG << "closing Zip " << _myZipFilename;
}

ZipPackage::FileList
ZipPackage::getFileList(const std::string & theSubDir, bool theRecurseFlag) {
    AC_DEBUG << "getFileList path='" << theSubDir << "'";
    if (theSubDir.empty()) {
        return _myFileList;
    }
    FileList myMatchingFiles;
    for (unsigned i = 0; i < _myFileList.size(); ++i) {
        AC_TRACE << _myFileList[i];
        if ( _myFileList[i].find(theSubDir) == 0) { // must start with theSubDir
            myMatchingFiles.push_back(_myFileList[i]);
        }
    }
    return myMatchingFiles;
}

std::string
ZipPackage::findFile(const std::string & theRelativePath) const {
    AC_DEBUG << "findFile path='" << theRelativePath << "'";
    for (unsigned i = 0; i < _myFileList.size(); ++i) {
        if (_myFileList[i].find(theRelativePath) == 0) { // must start with theRelativePath
            return _myZipFilename + "/" + _myFileList[i];
        }
    }
    return "";
}

Ptr<ReadableBlockHandle>
ZipPackage::getFile(const std::string & theRelativePath) {
    return _myZipReader.getFile(theRelativePath);
}

Ptr<ReadableStreamHandle>
ZipPackage::getStream(const std::string & theRelativePath) {
    return _myZipReader.getFile(theRelativePath);
}

}
