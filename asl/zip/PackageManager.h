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

#include "asl_zip_settings.h"

#include "IPackage.h"

#include <asl/base/Block.h>
#include <asl/base/Singleton.h>
#include <map>
#include <list>
#include <string>

namespace asl {

typedef std::list<IPackagePtr> PackageList;

/*! \addtogroup aslbase */
/* @{ */

/// Manages packages of files.
/** allows the efficient search for and opening of files contained in packages (see asl::IPackage).
 *  Example packages are asl::DirectoryPackage and asl::ZipPackage
*/
class ASL_ZIP_DECL PackageManager {
    public:



        PackageManager() {};
        virtual ~PackageManager() {};

        /// add a package to the package manager
        void add(IPackagePtr thePackage);

        /// removes a package from the package manager
        /** @returns true if the package has been removed
         */
        bool remove(IPackagePtr thePackage);

        /// removes a package from the package manager
        /**    @param thePackageName the name of the directory or zip-file to remove
               @returns true if the package has been removed
         */
        bool remove(const std::string & thePackageName);

        /// add multiple directorys and/or zip files to the package manager.
        /** Automatically creates DirectoryPackages or ZipPackages and adds them.
            @param thePackageNames a semicolon-delimited list of directorys or zip-files
        */
        void add(const std::string & thePackageNames);

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
            @returns the opened file as a asl::ReadableStream or null if not found.
        */
        Ptr<ReadableStreamHandle> readStream(const std::string & theRelativePath,
                                    const std::string & thePackageName = "");
        /// opens a file located at theRelativePath.
        /** Searches theRelativePath in either thePackageName or all packages.
            and reads the entire file into a block if found.
            @param theRelativePath is the path to the file requested,
            it is relative and must be defined inside a package.
            @param thePackageName is the name of a package,
            package names are stored just as you enter them by calling add.
            @returns the opened file as a asl::ReadableBlock or null if not found.
        */
        Ptr<ReadableBlockHandle> readFile(const std::string & theRelativePath,
                                    const std::string & thePackageName = "");

        /// findFiles lists the files in theRelativePath.
        /** Searches theRelativePath in either thePackageName or all packages.
            and returns all files in theRelativePath.
            If theRelativePath is a file it returns theRelativePath.
            @param theRelativePath is the path to the file requested,
            it is relative and must be defined inside a package.
            @param thePackageName is the name of a package,
            package names are stored just as you enter them by calling add.
            @returns the list as a std::vector<std::string> or an empty vector if not found.
        */
        IPackage::FileList findFiles(const std::string & theRelativePath = "",
                                     const std::string & thePackageName = "",
                                     bool doRecursiveSearch = false);

        /// searches for a file.
        /**
            @deprecated backward compatibility for asl::searchFile(), use
                    asl::PackageManager::readFile instead.
            @returns the path to a file or an empty string if not found.
            If the file was found inside a container (e.g. zip-file), then
            the zip file will be part of the path.
            (myDir/myZipfile.zip/myFile.txt)
        */
        std::string searchFile(const std::string & theRelativePath) const;

        /// get a list of package paths.
        /**
          @returns a semicolon-delimited list of packages which will be searched.
        */
        std::string getSearchPath() const;

        PackageList getPackageList() const;

    private:
        PackageManager(const PackageManager &);
        PackageManager & operator=(const PackageManager &);
        PackageList _myPackages;

        IPackage::FileList listPackageFiles(IPackagePtr thePackage,
                                            const std::string & theRelativePath,
                                            bool doRecursiveSearch);
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


