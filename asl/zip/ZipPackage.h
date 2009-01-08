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
//    $RCSfile: ZipPackage.h,v $
//
//   $Revision: 1.4 $
//
// Description: unit test template file - change ZipReader to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef __asl__ZipPackage_h_included__
#define __asl__ZipPackage_h_included__

#include "asl_zip_settings.h"

#include "IPackage.h"
#include "ZipReader.h"

namespace asl {

class ASL_ZIP_EXPORT ZipPackage : public IPackage {
    public:
        ZipPackage(const std::string & theZipReader);
        ~ZipPackage();
        const std::string & getPath() const { return _myZipFilename; };
        FileList getFileList(const std::string & theSubDir = "", bool theRecurseFlag = false);
        std::string findFile(const std::string & theRelativePath) const;
        Ptr<ReadableBlockHandle> getFile(const std::string & theRelativePath);
        Ptr<ReadableStreamHandle> getStream(const std::string & theRelativePath);

    private:
        ZipReader _myZipReader;
        std::string _myZipFilename;
        FileList _myFileList;
};

}
#endif



