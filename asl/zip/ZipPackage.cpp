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

#include "ZipPackage.h"

#include <asl/Logger.h>


using namespace std;

namespace asl {

ZipPackage::ZipPackage(const std::string & theZipFilename) :
    _myZipReader(theZipFilename.c_str()), _myZipFilename(theZipFilename)
{
    const ZipReader::Directory & myDirectory = _myZipReader.getDirectory();
    ZipReader::Directory::const_iterator it;
    for (it = myDirectory.begin(); it != myDirectory.end(); ++it) {
        _myFileList.push_back(it->filename);
    }
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

Ptr<ReadableBlock>
ZipPackage::getFile(const std::string & theRelativePath) {
    return _myZipReader.getFile(theRelativePath);
}

}
