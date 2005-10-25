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
//    $RCSfile: IPackage.h,v $
//
//   $Revision: 1.4 $
//
// Description: unit test template file - change ZipFile to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef __asl__IPackage_h_included__
#define __asl__IPackage_h_included__

#include <asl/Exception.h>
#include <asl/Ptr.h>
#include <asl/Block.h>

namespace asl {

DEFINE_EXCEPTION(PackageException, asl::Exception);

/*! \addtogroup aslbase */
/* @{ */
   
/// interface for file packages
class IPackage {
    public:
        typedef std::vector<std::string> FileList;

        virtual ~IPackage() {};
 
        /// returns the path of the package.
        virtual const std::string & getPath() const = 0;

        /// returns a list of contained files. 
        virtual FileList getFileList(const std::string & theSubDir = "", bool theRecursiveFlag = false) = 0;

        /// returns absolute path to contained file or empty.
        virtual std::string findFile(const std::string & theRelativePath) = 0;

        /// opens a contained file. 
        virtual Ptr<ReadableBlock> getFile(const std::string & theRelativePath) = 0;
};

typedef asl::Ptr<IPackage> IPackagePtr;

/* @} */
}
#endif
