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
//    $RCSfile: ZipReader.cpp,v $
//
//   $Revision: 1.9 $
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "ZipReader.h"
#include "unzip.h"
#include "zlib.h"
#include "ioapi.h"
#include <asl/Logger.h>

#include <string>

#define DB(x) // x;

using namespace std;

namespace asl {

#define CHECK_UNZIP(x) int myRetVal(x); if (myRetVal != UNZ_OK) \
    throw ZipFileException(string("UNZIP Error: ")+as_string(myRetVal), PLUS_FILE_LINE)
    
ZipReader::ZipReader(const char *  theInputFileName) : _myInputStream(0)
{
    Path myPath(theInputFileName, UTF8);
    _myInputStream = unzOpen(myPath.toLocale().c_str());
    AC_DEBUG << "opening " << myPath;
    if (_myInputStream == 0) {
        throw ZipFileException(string("can't open zip file '") + myPath.toLocale() + "'",
                PLUS_FILE_LINE);
    }
    readDirectory();
}

ZipReader::~ZipReader() {
    if (_myInputStream) {
        int retVal = unzClose(_myInputStream);
        AC_DEBUG << "closing zip file returns " << retVal;
    }
}

const ZipReader::Directory &
ZipReader::getDirectory() const {
    return _myDirectory;
}

void
ZipReader::readDirectory() {    
    unz_global_info globalInfo;
    CHECK_UNZIP(unzGetGlobalInfo (_myInputStream, &globalInfo));
    
    for (int i=0;i<globalInfo.number_entry;i++)
    {
        char filename_inzip[4096];
        unz_file_info file_info;
        int myError = unzGetCurrentFileInfo(_myInputStream,&file_info,filename_inzip,
                sizeof(filename_inzip),NULL,0,NULL,0);
        CHECK_UNZIP(myError);
        
        Entry curEntry;
        //TODO: fill in other fields
        DB(AC_TRACE << "found '" << filename_inzip << "'" << endl);
        // change all backslashes to forward slashes
        int myNameSize = strlen(filename_inzip);
        for (int j=0; j < myNameSize && j < sizeof(filename_inzip); ++j) {
            if ( filename_inzip[j] == '\\' ) {
                filename_inzip[j] = '/';
            }
        }
        
        curEntry.filename = Path(filename_inzip, Locale);
        curEntry.size = file_info.uncompressed_size;
        unz_file_pos myFilePos;
        unzGetFilePos(_myInputStream, &myFilePos);
        curEntry.directory_pos = myFilePos.pos_in_zip_directory;
        curEntry.file_index = myFilePos.num_of_file;

        // if not an empty filename and without
        // a trailing directory seperator, then
        // add the entry to the list
        if (!curEntry.filename.empty() && 
            curEntry.filename.toLocale()[curEntry.filename.toLocale().size()-1] != '/')
        {
            _myDirectory.push_back(curEntry); 
        }
        
        if ((i+1)<globalInfo.number_entry)
        {
            CHECK_UNZIP(unzGoToNextFile(_myInputStream));
        }
    }
}

Ptr<ReadableBlockHandle> 
ZipReader::getFile(int theFileIndex) {
    return getFile(_myDirectory[theFileIndex]);
}

Ptr<ReadableBlockHandle> 
ZipReader::getFile(const std::string & theFilePath) {
    Path myPath(theFilePath, UTF8);
    for (int i = 0; i < _myDirectory.size(); ++i) {
        DB(AC_TRACE << "cmp:" << _myDirectory[i].filename << "==" << theFilePath << endl);
        if (myPath == _myDirectory[i].filename) {
            DB(AC_TRACE << "OK" << endl);
            return getFile(_myDirectory[i]);
        }
    }
    return Ptr<ReadableBlockHandle>(0);
}

Ptr<ReadableBlockHandle> 
ZipReader::getFile(const Entry & theEntry) {
    DB(AC_TRACE << "unzipping '" << theEntry.filename << "', size=" << theEntry.size << endl);
    unz_file_pos myFilePos;
    myFilePos.pos_in_zip_directory = theEntry.directory_pos;
    myFilePos.num_of_file = theEntry.file_index;

    unzGoToFilePos(_myInputStream, &myFilePos);
    CHECK_UNZIP(unzOpenCurrentFilePassword(_myInputStream, NULL));
    asl::Ptr<Block> myBlock(new Block(theEntry.size));
    asl::Ptr<ReadableBlockHandle> myHandle(new AnyReadableBlockHandle(myBlock, theEntry.filename.toUTF8()));
    size_t myTotalBytesRead =0;
    while (myTotalBytesRead < theEntry.size) {
        int myBytesRead = unzReadCurrentFile(_myInputStream, myBlock->begin()+myTotalBytesRead, 
                theEntry.size-myTotalBytesRead);
        if (myBytesRead < 0) {
            unzCloseCurrentFile(_myInputStream);
            CHECK_UNZIP(myBytesRead);
        }
        myTotalBytesRead += myBytesRead;
    }
    unzCloseCurrentFile(_myInputStream);
    return myHandle;
}

}
