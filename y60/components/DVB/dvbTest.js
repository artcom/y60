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

plug("y60DVB");
use("Exception.js");
use("Y60JSSL.js");

function compactString(theString){
    var myReturnString = "";
    
    if (theString) {             
        var myString_split = theString.split("\n");
        for (var j=0; j<myString_split.length; ++j){
            var myStringLine = trim(myString_split[j]);               
            if (myStringLine != ""){
                 myReturnString += myStringLine+"\n";
            }
        }
    }
    return myReturnString;
}

try {
    var myConfig = new Node();
    myConfig.parseFile("channels.de.berlin.xml");
    
    var myDeviceName = "/dev/dvb/adapter0";

    if (!DvbTuner.deviceAvailable(myDeviceName)) {
        print("Device "+myDeviceName+" not available.");
        exit(1);
    }

    var myDvb = new DvbTuner(myConfig, "/dev/dvb/adapter0");
    myDvb.startTeleTextDecoder();
    
    print("tuning 'ZDF'");
    myDvb.tuneChannel("ZDF");

    // Restart teletext decoder, because tuning stops the decoder.
    myDvb.startTeleTextDecoder();

    var myPageNumber = 776;
    
    var myPage = null;
    do {
        myPage = myDvb.getPage(myPageNumber);
    } while(!myPage);
        
    print("Teletext Page Number: " + myPageNumber + "\n" + myPage);
        
    myDvb.stopTeleTextDecoder();
} catch (ex) {
   reportException(ex);
}

