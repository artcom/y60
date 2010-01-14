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

#include "asl_zip_settings.h"

#include <asl/base/Exception.h>
#include <asl/base/Ptr.h>
#include <asl/base/Block.h>

namespace asl {

DEFINE_EXCEPTION(PackageException, asl::Exception);

/*! \addtogroup aslbase */
/* @{ */

/// interface for file packages
class ASL_ZIP_DECL IPackage {
    public:
        typedef std::vector<std::string> FileList;

        virtual ~IPackage() {};

        /// returns the path of the package.
        virtual const std::string & getPath() const = 0;

        /// returns a list of contained files.
        virtual FileList getFileList(const std::string & theSubDir = "", bool theRecursiveFlag = false) = 0;

        /// returns absolute path to contained file or empty.
        virtual std::string findFile(const std::string & theRelativePath) const = 0;

        /// opens a stream to a contained file.
        virtual Ptr<ReadableStreamHandle> getStream(const std::string & theRelativePath) = 0;

        /// get the contents of a contained file.
        virtual Ptr<ReadableBlockHandle> getFile(const std::string & theRelativePath) = 0;
};

typedef asl::Ptr<IPackage> IPackagePtr;

/* @} */
}
#endif
