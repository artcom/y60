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
//    $RCSfile: ZipWriter.h,v $
//
//   $Revision: 1.3 $
//
// Description: unit test template file - change ZipWriter to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef __asl_zipwriter_included_
#define __asl_zipwriter_included_

#include <asl/base/Stream.h>
#include <asl/base/Block.h>
#include <string>

namespace asl {

DEFINE_EXCEPTION(ZipWriterException, asl::Exception);
    
class ZipWriter {
    public:
        ZipWriter(const char * theOutputFileName);
        ZipWriter(const std::string & theOutputFileName);
        virtual ~ZipWriter();
        void append(ReadableBlock & theData, const char * theFilename);
        void append(ReadableBlock & theData, const std::string & theFilename);
        void close();
    private:
        void create(const char * theOutputFileName);
        ZipWriter();
        ZipWriter(const ZipWriter &);
        ZipWriter & operator=(const ZipWriter &);
        void* _myZipFile;
};

typedef Ptr<ZipWriter> ZipWriterPtr;

}

#endif
