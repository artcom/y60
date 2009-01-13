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
//    $RCSfile: DirectoryPackage.h,v $
//
//   $Revision: 1.3 $
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef __asl__DirectoryPackage_h_included__
#define __asl__DirectoryPackage_h_included__

#include "asl_zip_settings.h"

#include "IPackage.h"

namespace asl {

class ASL_ZIP_EXPORT DirectoryPackage : public IPackage {
    public:
        DirectoryPackage(const std::string & theDirectory, bool theIgnoreDotFiles = true);
        const std::string & getPath() const { return _myDirectory; };
        FileList getFileList(const std::string & theSubDir = "", bool theRecurseFlag = false);
        std::string findFile(const std::string & theRelativePath) const;
        Ptr<ReadableStreamHandle> getStream(const std::string & theRelativePath);
        Ptr<ReadableBlockHandle> getFile(const std::string & theRelativePath);

    private:
        std::string _myDirectory;
        bool _myIgnoreDotFiles;

        std::string getAbsolutePath(const std::string & theRelativePath) const;
};

}
#endif
