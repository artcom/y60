//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: PLFile.cpp,v $
//   $Author: christian $
//   $Revision: 1.7 $
//   $Date: 2005/03/24 23:35:59 $
//
//
//=============================================================================

#include "CTScan.h"
#include "PLFile.h"
#include <asl/Block.h>
#include <y60/PLFilterResizePadded.h>
#include <y60/PLFilterFactory.h>
#include <y60/Image.h>

using namespace asl;
using namespace std;

namespace y60 {

PLFile::PLFile(asl::Ptr<asl::ReadableBlock> theInputData, const std::string & theFilename)
    : _myFilename(theFilename), _myBitmap(theInputData, theFilename), CTFile()
{
}

PLFile::~PLFile() {
}

float
PLFile::getZPos(int theFrame) const {
    if (theFrame != 0) {
        throw CTScanException(string("Frame '")+as_string(theFrame)+"'not found", PLUS_FILE_LINE);
    }
    // find last number in the filename - this is probably the slice index
    string::size_type mySearchPos = _myFilename.rfind('.'); // skip extension
    if (mySearchPos == string::npos) {
        mySearchPos = _myFilename.length()-1;
    }

    string::size_type myLastNum = _myFilename.find_last_of("0123456789", mySearchPos);
    if (myLastNum == string::npos) {
        cerr << "###WARNING: could not find slice number in file name '" << _myFilename << "'" << endl;
        return 0;
    }
    mySearchPos = myLastNum-1;
    string::size_type myFirstNum = _myFilename.find_last_not_of("0123456789", mySearchPos)+1;
    string myIndexString = _myFilename.substr(myFirstNum, myLastNum-myFirstNum+1);
    return as<float>(myIndexString);
};

dom::ResizeableRasterPtr 
PLFile::getRaster(int theFrame) {
    return _myBitmap.getRaster();
}

PixelEncoding
PLFile::getEncoding() const {
    return _myBitmap.getEncoding();
}

}
