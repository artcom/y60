/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 2003, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testHeartbeat.js,v $
//
//   $Revision: 1.2 $
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

for (var i = 0; i< 10; i++) {
    print ("throb");
    throbHeartbeat("HeartbeatTest.ecg");
    msleep(100);
    gc();
}

exit(0);

