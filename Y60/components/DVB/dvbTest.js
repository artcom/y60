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
//    $RCSfile: dvbTest.js,v $
//
//   $Revision: 1.0 $
//
// Description: simple test application for the linux y60 dvb component
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

plug("DVB");
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

