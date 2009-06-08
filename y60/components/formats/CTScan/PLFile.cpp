/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
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
#include <asl/base/Block.h>
#include <y60/image/PLFilterResizePadded.h>
#include <y60/image/PLFilterFactory.h>
#include <y60/image/Image.h>

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
