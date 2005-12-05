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
//    $RCSfile: testDirectorIP.js,v $
//
//   $Revision: 1.2 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


// Note: Run this script when the director test movie is running to see if
// the two connect.

var mySocket;
print("Start.\n");
mySocket = new Socket(Socket.UDP, 1103, "127.0.0.1");
print("Connecting...\n");
mySocket.connect("127.0.0.1", 1104);
print("Connected.\n");

mySocket.setBlockingMode(false);
print(mySocket);

var myReceivedString;

var i=0;
while(i<10) {
    var s = '<GlobeProperties>\n<Coordinates latitude="49.43" longitude="89.1"/>\n</GlobeProperties>\n';
    mySocket.write(s+s+s+s+s+s+s+s);
    myReceivedString = mySocket.read();
    if (myReceivedString.length > 0) {
        print(myReceivedString);
    } else {
        print("no packet received");
    }

    msleep(100);
    i++;
}
mySocket.close();
