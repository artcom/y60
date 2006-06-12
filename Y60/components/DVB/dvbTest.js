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
    myDvb.tuneChannel("Das Erste"); 

    // exec("cat /dev/dvb/adapter0/dvr0 | mplayer - &");

//    myDvb.startTeleTextDecoder();
    
        print("tuning 'Das Erste'");
        myDvb.tuneChannel("ZDF");
	msleep(20000);

    var myOutString = "";
    for (var i=0; i<100; ++i){
        msleep(500); 
        print("tuning 'Das Erste'");
	myDvb.tuneChannel("Das Erste");

        msleep(500);
        print("tuning 'Das Erste'");
        myDvb.tuneChannel("Das Erste");

        msleep(8000);
        print("tuning 'Das Erste'");
        myDvb.tuneChannel("Das Erste");

	
        msleep(500);
        print("tuning 'ZDF'");
        myDvb.tuneChannel("ZDF");

        msleep(500);
        print("tuning 'ZDF'");
        myDvb.tuneChannel("ZDF");

        msleep(8000);
        print("tuning 'ZDF'");
        myDvb.tuneChannel("ZDF");


//        var mySubtitle = myDvb.getPage(777);
//        var myCompactSubtitle = compactString(mySubtitle);
//        if (myCompactSubtitle) {
//            var myFormattedString = compactString(mySubtitle);
//            print(myFormattedString);
//        }
    }   
        
//    exec("killall mplayer");

    myDvb.stopTeleTextDecoder();
} catch (ex) {
   reportException(ex);
}

