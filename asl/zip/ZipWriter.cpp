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
//    $RCSfile: ZipWriter.cpp,v $
//
//   $Revision: 1.9 $
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "ZipWriter.h"
#include "zip.h"
#include "zlib.h"
#include "ioapi.h"
#include <asl/Logger.h>

#include <string>

#define DB(x) // x;

using namespace std;

namespace asl {

#define CHECK_ZIP(x) { int myRetVal(x); if (myRetVal != ZIP_OK) \
    throw ZipWriterException(string("ZIP Error: ")+as_string(myRetVal), PLUS_FILE_LINE); }
    
ZipWriter::ZipWriter(const char *  theOutputFileName) : _myZipFile(0) 
{
    create(theOutputFileName);
}

ZipWriter::ZipWriter(const std::string & theOutputFileName) 
{
    create(theOutputFileName.c_str());
}

ZipWriter::~ZipWriter() {
    close();
}

void
ZipWriter::create(const char * theOutputFileName) {
    _myZipFile = zipOpen(theOutputFileName,0);
    if (!_myZipFile) {
        throw ZipWriterException(string("ZIP Error: could not create zip file '")+theOutputFileName+"'", PLUS_FILE_LINE);
    }
}

void
ZipWriter::close() {
    if (_myZipFile) {
        CHECK_ZIP(zipClose(_myZipFile, 0));
        _myZipFile = 0;
    }
}

void 
ZipWriter::append(ReadableBlock & theData, const char * theFilename) {
    CHECK_ZIP(zipOpenNewFileInZip(_myZipFile, theFilename, 
            0, // zip_fileinfo
            0, // extrafield_local
            0, // extrafield_local size
            0, // extrafield_global
            0, // extrafield_global size
            0, // comment
            Z_DEFLATED,
            Z_DEFAULT_COMPRESSION));

    CHECK_ZIP(zipWriteInFileInZip(_myZipFile, theData.begin(), theData.size()));

    CHECK_ZIP(zipCloseFileInZip(_myZipFile));
    
    

}

void 
ZipWriter::append(ReadableBlock & theData, const std::string & theFilename) {
    append(theData, theFilename.c_str());
}


}

