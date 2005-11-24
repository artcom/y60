/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testConfigurator.tst.js,v $
//
//   $Revision: 1.5 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");
use("Configurator.js");

function ConfiguratorUnitTest() {
    this.Constructor(this, "ConfiguratorUnitTest");
};

const COMMON_SETTINGS = "<settings>" +
                        "    <CommonOnly><CommonOnly>Common</CommonOnly></CommonOnly>" +
                        "    <CommonANDHost><CommonOnly>Common</CommonOnly><CommonANDHost>Common</CommonANDHost></CommonANDHost>" +
                        "</settings>";
const HOST_SETTINGS   = "<settings>" +
                        "<HostOnly><HostOnly>Host</HostOnly></HostOnly>" +
                        "<CommonANDHost><HostOnly>Host</HostOnly><CommonANDHost>Host</CommonANDHost></CommonANDHost>" +
                        "</settings>";

const RESULT =
    "<settings>\n" +
    "    <CommonOnly>\n" +
    "        <CommonOnly>Common</CommonOnly>\n" +
    "    </CommonOnly>\n" +
    "    <CommonANDHost>\n" +
    "        <CommonOnly>Common</CommonOnly>\n" +
    "        <CommonANDHost>Host</CommonANDHost>\n" +
    "        <HostOnly>Host</HostOnly>\n" +
    "    </CommonANDHost>\n" +
    "    <HostOnly>\n" +
    "        <HostOnly>Host</HostOnly>\n" +
    "    </HostOnly>\n" +
    "</settings>\n";

ConfiguratorUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        writeWholeStringToFile("settings.xml", COMMON_SETTINGS);
        writeWholeStringToFile("settings-" + hostname() + ".xml", HOST_SETTINGS);
        var myConfigurator = new Configurator(null, "settings.xml");
        obj.mySettings = myConfigurator.getSettings();
        obj.myResult = RESULT;
        ENSURE("obj.mySettings == obj.myResult");
        deleteFile("settings.xml");
        deleteFile("settings-" + hostname() + ".xml");
    }
};

var myTestName = "testConfigurator.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new ConfiguratorUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
