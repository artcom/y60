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
//    $RCSfile: PackageManager.cpp,v $
//
//   $Revision: 1.11 $
//
// Description: unit test template file - change ZipFile to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "PackageManager.h"
#include "DirectoryPackage.h"
#include "ZipPackage.h"
#include <asl/file_functions.h>
#include <asl/MappedBlock.h>
#include <asl/Logger.h>
#include <vector>

namespace asl {

void
PackageManager::add(IPackagePtr thePackage) {
    for (PackageList::iterator it=_myPackages.begin(); it!=_myPackages.end(); ++it) {
        if ((*it)->getPath() == thePackage->getPath()) {
            AC_DEBUG << "Package '" << thePackage->getPath() << "' has already been added.";
            return;
        }
    }

    AC_DEBUG << "adding Package '" << thePackage->getPath() << "'";
    _myPackages.push_front(thePackage);
}

bool
PackageManager::remove(IPackagePtr thePackage) {
    for (PackageList::iterator it=_myPackages.begin(); it!=_myPackages.end(); ++it) {
        if ((*it)->getPath() == thePackage->getPath()) {
            _myPackages.erase(it);
            AC_DEBUG << "Package '" << thePackage->getPath() << "' has been removed.";
            return true;
        }
    }
    return false;
}

bool
PackageManager::remove(const std::string & thePackageName) {
    for (PackageList::iterator it=_myPackages.begin(); it!=_myPackages.end(); ++it) {
        if ((*it)->getPath() == thePackageName) {
            _myPackages.erase(it);
            AC_DEBUG << "Package '" << thePackageName << "' has been removed.";
            return true;
        }
    }
    return false;
}

void
PackageManager::add(const std::string & thePaths) {
    AC_TRACE << "adding '" << thePaths << "'";
    std::vector<std::string> myPathVector;
    splitPaths(thePaths, myPathVector);
    AC_DEBUG << "adding " << myPathVector.size() << " packages";
    for (unsigned i = 0; i < myPathVector.size(); ++i) {
        if (isDirectory(myPathVector[i])) {
            AC_DEBUG << "adding Directory '" << myPathVector[i] << "'";
            add(IPackagePtr(new DirectoryPackage(myPathVector[i])));
            continue;
        }
        if (!fileExists(myPathVector[i])) {
            AC_WARNING << "PackageManager: File '" << myPathVector[i] << "' does not exist";
            continue;
        }
        ConstMappedBlock myBlock(myPathVector[i]);
        // check zip file signature
        if (myBlock[0] == 0x50 && myBlock[1] == 0x4b && myBlock[2] == 0x03 && myBlock[3] == 0x04) {
            AC_DEBUG << "adding Zip '" << myPathVector[i] << "'";
            add(IPackagePtr(new ZipPackage(myPathVector[i])));
        } else {
            AC_WARNING << "PackageManager: can not open package '" << myPathVector[i] << "'"
                        << ", not a directory and file type  unknown.";
            AC_WARNING << "Sig is " << myBlock[0] << myBlock[1] << myBlock[2] << myBlock[3];
        }
    }
}

void
PackageManager::add(const PackageManager & theOther) {
    AC_TRACE << "add from package '" << theOther.getSearchPath() << "'";
    PackageList::const_iterator it = theOther._myPackages.begin();
    for (; it != theOther._myPackages.end(); ++it) {
        add(*it);
    }
}

IPackagePtr
PackageManager::findPackage(const std::string & theRelativePath,
                            const std::string & thePackage)
{
    AC_TRACE << "findPackage pkg='" << thePackage << "' path='" << theRelativePath << "'";
    std::string myPackage = normalizeDirectory(thePackage, true);
    AC_TRACE << "findPackage myPkg='" << myPackage << "'";
    for (PackageList::iterator iter = _myPackages.begin();
         iter != _myPackages.end(); ++iter)
    {
        if ((myPackage != "" && (*iter)->getPath() != myPackage)) {
            continue;
        }
        if ((*iter)->findFile(theRelativePath) != "") {
            return *iter;
        }
    }
    return IPackagePtr(0);
}

Ptr<ReadableStreamHandle>
PackageManager::readStream(const std::string & theRelativePath,
                         const std::string & thePackage)
{
    IPackagePtr myPackage = findPackage(theRelativePath, thePackage);
    if (myPackage) {
        return myPackage->getStream(theRelativePath);
    }
    return Ptr<ReadableStreamHandle>(0);
}
Ptr<ReadableBlockHandle>
PackageManager::readFile(const std::string & theRelativePath,
                         const std::string & thePackage)
{
    IPackagePtr myPackage = findPackage(theRelativePath, thePackage);
    if (myPackage) {
        return myPackage->getFile(theRelativePath);
    }
    return Ptr<ReadableBlockHandle>(0);
}

std::string
PackageManager::searchFile(const std::string & theRelativePath) const {
    if (_myPackages.size() == 0) {
        AC_WARNING << "No packages to search!";
    }
    for (PackageList::const_iterator iter = _myPackages.begin();
         iter != _myPackages.end(); ++iter) {
        std::string myAbsolutePath = (*iter)->findFile(theRelativePath);
        if (myAbsolutePath.empty() == false) {
            return myAbsolutePath;
        }
    }
    if (fileExists(theRelativePath)) {
        return theRelativePath;
    }
    return "";
}

IPackage::FileList
PackageManager::listPackageFiles(IPackagePtr thePackage,
                                 const std::string & theRelativePath,
                                 bool doRecursiveSearch)
{
    IPackage::FileList myFileList;
    if (thePackage && thePackage->findFile(theRelativePath).empty() == false) {
        myFileList = thePackage->getFileList(theRelativePath, doRecursiveSearch);
    }

    return myFileList;
}

IPackage::FileList
PackageManager::findFiles(const std::string & theRelativePath,
                          const std::string & thePackage,
                          bool doRecursiveSearch /*= false */)
{
    AC_TRACE << "findFiles pkg='" << thePackage << "' path='" << theRelativePath << "'";
    IPackage::FileList myFileList;
    if (thePackage.empty() == false) {
        IPackagePtr myPackage = findPackage("", thePackage);
        myFileList = listPackageFiles(myPackage, theRelativePath, doRecursiveSearch);
    } else {
        for (PackageList::const_iterator iter = _myPackages.begin();
             iter != _myPackages.end(); ++iter) {
            IPackage::FileList myTmpFileList = listPackageFiles((*iter), theRelativePath, doRecursiveSearch);
            for (unsigned i = 0; i < myTmpFileList.size(); ++i) {
                myFileList.push_back(myTmpFileList[i]);
            }
        }
    }
    return myFileList;
}

std::string
PackageManager::getSearchPath() const {
    std::string mySearchPath;
    PackageList::const_iterator it = _myPackages.begin();
    for (; it != _myPackages.end(); ++it) {
        if ( ! mySearchPath.empty()) {
            mySearchPath += ";";
        }
        mySearchPath += (*it)->getPath();
    }
    return mySearchPath;
}

}
