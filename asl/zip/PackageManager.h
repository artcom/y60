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
//    $RCSfile: PackageManager.h,v $
//
//   $Revision: 1.5 $
//
// Description: unit test template file - change ZipFile to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef __asl_packagemanager_h_included__
#define __asl_packagemanager_h_included__

#include "IPackage.h"

#include <asl/Block.h>
#include <asl/Singleton.h>
#include <map>
#include <list>
#include <string>

namespace asl {

/*! \addtogroup aslbase */
/* @{ */
   
/// Manages packages of files. 
/** allows the efficient search for and opening of files contained in packages (see asl::IPackage). 
 *  Example packages are asl::DirectoryPackage and asl::ZipPackage
*/
class PackageManager {
    public:
        PackageManager() {};
        virtual ~PackageManager() {};
 
        /// add a package to the package manager
        void add(IPackagePtr thePackage);

        /// add multiple directorys and/or zip files to the package manager.
        /** Automatically creates DirectoryPackages or ZipPackages and adds them.
            @param thePaths a semicolon-delimited list of directorys or zip-files
        */
        void add(const std::string & thePackageName);

        /// adds the contents of other PackageManager.
        /** All packages contained in theOther are also added to this.
            @param theOther the PackageManager from which to copy 
        */
        void add(const PackageManager & theOther);

        /// opens a file located at theRelativePath.
        /** Searches theRelativePath in either thePackageName or all packages.
            and open the file located at theRelativePath if found. 
            @param theRelativePath is the path to the file requested, 
            it is relative and must be defined inside a package.
            @param thePackageName is the name of a package, 
            package names are stored just as you enter them by calling add.
            @returns the opened file as a asl::ReadableBlock or null if not found.
        */
        Ptr<ReadableBlock> openFile(const std::string & theRelativePath,
                                    const std::string & thePackageName = "");

        /// listFiles lists the files in theRelativePath.
        /** Searches theRelativePath in either thePackageName or all packages.
            and returns all files in theRelativePath. 
            If theRelativePath is a file it returns theRelativePath.
            @param theRelativePath is the path to the file requested, 
            it is relative and must be defined inside a package.
            @param thePackageName is the name of a package, 
            package names are stored just as you enter them by calling add.
            @returns the list as a std::vector<std::string> or an empty vector if not found.
        */
        IPackage::FileList listFiles(const std::string & theRelativePath = "",
                                     const std::string & thePackageName = "");

        /// searches for a file.
        /**
            @deprecated backward compatibility for asl::searchFile(), use 
                    asl::PackageManager::openFile instead.
            @returns the path to a file or an empty string if not found.
            If the file was found inside a container (e.g. zip-file), then
            the zip file will be part of the path.
            (myDir/myZipfile.zip/myFile.txt)
        */
        std::string searchFile(const std::string & theRelativePath);

        /// get a list of package paths.
        /**
          @returns a semicolon-delimited list of packages which will be searched.
        */
        std::string getSearchPath() const;

    private:
        PackageManager(const PackageManager &);
        PackageManager & operator=(const PackageManager &);
        typedef std::list<IPackagePtr> PackageList;
        PackageList _myPackages;

        IPackage::FileList listPackageFiles(IPackagePtr thePackage,
                                            const std::string & theRelativePath);
        IPackagePtr findPackage(const std::string & theRelativePath, const std::string & thePackage);   
};

typedef Ptr<PackageManager> PackageManagerPtr;

class AppPackageManager : public Singleton<AppPackageManager>  {
    public:
        AppPackageManager() {
            _myPackageManager = PackageManagerPtr(new PackageManager());
        }
        const PackageManagerPtr & getPtr() const {
            return _myPackageManager;
        }
    private:
        PackageManagerPtr _myPackageManager;
};

/* @} */
}


#endif


