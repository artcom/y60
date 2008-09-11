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

#include <asl/base/Stream.h>
#include <asl/base/Block.h>
#include <asl/xpath/Path.h>
#include <string>

namespace asl {

DEFINE_EXCEPTION(ZipFileException, asl::Exception);
    
class ZipReader {
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
