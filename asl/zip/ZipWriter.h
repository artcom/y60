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

#include <asl/Stream.h>
#include <asl/Block.h>
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
