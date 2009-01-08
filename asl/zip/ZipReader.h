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
//    $RCSfile: ZipFile.h,v $
//
//   $Revision: 1.3 $
//
// Description: unit test template file - change ZipFile to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef __asl_zipfile_included_
#define __asl_zipfile_included_

#include "asl_zip_settings.h"

#include <asl/base/Stream.h>
#include <asl/base/Block.h>
#include <asl/base/Path.h>
#include <string>

namespace asl {

DEFINE_EXCEPTION(ZipFileException, asl::Exception);
    
class ASL_ZIP_EXPORT ZipReader {
    public:
        struct Entry {
            Path filename;
            size_t size;
            size_t directory_pos;
            size_t file_index;
        };
        typedef std::vector<Entry> Directory; 
        ZipReader(const char * theInputFileName);
        virtual ~ZipReader();
        const Directory & getDirectory() const;
        Ptr<ReadableBlockHandle> getFile(const Entry & theEntry);
        Ptr<ReadableBlockHandle> getFile(int theFileIndex);
        Ptr<ReadableBlockHandle> getFile(const std::string & theFilePath);
    private:
        ZipReader();
        ZipReader(const ZipReader &);
        ZipReader & operator=(const ZipReader &);
        void readDirectory();
        Directory _myDirectory;
        void * _myInputStream;
};

typedef Ptr<ZipReader> ZipReaderPtr;

}

#endif
