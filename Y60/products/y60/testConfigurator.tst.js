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

const COMMON_SETTINGS = "../../testfiles/common_settings.xml";
const MERGED_SETTINGS = "../../testfiles/merged_settings.xml";
const LIST_A_SETTINGS = "../../testfiles/settings_list_a.xml";
const LIST_B_SETTINGS = "../../testfiles/settings_list_b.xml";

ConfiguratorUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        try {

            var myConfigurator = new Configurator( null, COMMON_SETTINGS, 
                                                   [LIST_A_SETTINGS, LIST_B_SETTINGS] );
            obj.mySettingsFileList = myConfigurator.getSettingsFileList();
            ENSURE( "obj.mySettingsFileList[0] == LIST_A_SETTINGS && " + 
                    "obj.mySettingsFileList[1] == LIST_B_SETTINGS" );

            testLoadSettings();
            testSaveSettings();

        } catch (e) {
            print( e );
        }
    }

    function testLoadSettings() {
        var myConfigurator = new Configurator( null, COMMON_SETTINGS, 
                                               [LIST_A_SETTINGS, LIST_B_SETTINGS] );

        obj.mySettings = myConfigurator.getSettings();

        obj.myResult = new Node();
        obj.myResult.parseFile( MERGED_SETTINGS );
        obj.myResult = obj.myResult.firstChild;

        ENSURE("obj.mySettings.toString() == obj.myResult.toString()");
    }

    function testSaveSettings() {
            
        const COMMON_SETTINGS_COPY = "common_settings_copy.xml";
        const LIST_A_SETTINGS_COPY = "settings_list_a_copy.xml";
        const LIST_B_SETTINGS_COPY = "settings_list_b_copy.xml";

        // first copy some files:
        copyFile( COMMON_SETTINGS, COMMON_SETTINGS_COPY );
        copyFile( LIST_A_SETTINGS, LIST_A_SETTINGS_COPY );
        copyFile( LIST_B_SETTINGS, LIST_B_SETTINGS_COPY );

        // initialize the configurator with our copies
        var myConfigurator = new Configurator( null, COMMON_SETTINGS_COPY,
                                               [LIST_A_SETTINGS_COPY, 
                                                LIST_B_SETTINGS_COPY] );

        // create our expected results:
        obj.myCommonSettingsResult = new Node();
        obj.myCommonSettingsResult.parseFile( COMMON_SETTINGS );
        obj.myCommonSettingsResult = obj.myCommonSettingsResult.firstChild;
        obj.myCommonSettingsResult.childNode( "CommonOnlySection" )
                                  .childNode( "CommonOnly" )
                                  .childNode( "#text" ).nodeValue = "Altered";

        obj.myListASettingsResult = new Node();
        obj.myListASettingsResult.parseFile( LIST_A_SETTINGS );
        obj.myListASettingsResult = obj.myListASettingsResult.firstChild;
        obj.myListASettingsResult.childNode( "CommonAndListASection" )
                                 .childNode( "CommonAndListA" )
                                 .childNode( "#text" ).nodeValue = "Altered";
        
        obj.myListBSettingsResult = new Node();
        obj.myListBSettingsResult.parseFile( LIST_B_SETTINGS );
        obj.myListBSettingsResult = obj.myListBSettingsResult.firstChild;
        obj.myListBSettingsResult.childNode( "CommonAndListBSection" )
                                 .childNode( "CommonAndListB" )
                                 .childNode( "#text" ).nodeValue = "Altered";
        obj.myListBSettingsResult.childNode( "CommonSection" )
                                 .childNode( "Common" )
                                 .childNode( "#text" ).nodeValue = "Altered";


        // change and save the current settings
        var myMergedSettings = myConfigurator.getSettings();
        myMergedSettings.childNode( "CommonOnlySection" )
                        .childNode( "CommonOnly" )
                        .childNode( "#text" ).nodeValue = "Altered";
        myMergedSettings.childNode( "CommonAndListASection" )
                        .childNode( "CommonAndListA" )
                        .childNode( "#text" ).nodeValue = "Altered";
        myMergedSettings.childNode( "CommonAndListBSection" )
                        .childNode( "CommonAndListB" )
                        .childNode( "#text" ).nodeValue = "Altered";
        myMergedSettings.childNode( "CommonSection" )
                        .childNode( "Common" )
                        .childNode( "#text" ).nodeValue = "Altered";

        obj.myClonedMergedSettings = myMergedSettings.cloneNode( true );

        myConfigurator.saveSettings();

        // load the saved files
        obj.myCommonSettingsSaved = new Node();
        obj.myCommonSettingsSaved.parseFile( COMMON_SETTINGS_COPY );
        obj.myCommonSettingsSaved = obj.myCommonSettingsSaved.firstChild;
        
        obj.myListASettingsSaved = new Node();
        obj.myListASettingsSaved.parseFile( LIST_A_SETTINGS_COPY );
        obj.myListASettingsSaved = obj.myListASettingsSaved.firstChild;

        obj.myListBSettingsSaved = new Node();
        obj.myListBSettingsSaved.parseFile( LIST_B_SETTINGS_COPY );
        obj.myListBSettingsSaved = obj.myListBSettingsSaved.firstChild;
        
        ENSURE( "obj.myCommonSettingsResult.toString() " + 
                "== obj.myCommonSettingsSaved.toString()" );
        ENSURE( "obj.myListASettingsResult.toString() " + 
                "== obj.myListASettingsSaved.toString()" );
        ENSURE( "obj.myListBSettingsResult.toString() " + 
                "== obj.myListBSettingsSaved.toString()" );

        obj.myCurrentSettings = myConfigurator.getSettings();
        ENSURE( "obj.myClonedMergedSettings.toString() " + 
                "== obj.myCurrentSettings.toString()" );

        deleteFile( COMMON_SETTINGS_COPY );
        deleteFile( LIST_A_SETTINGS_COPY );
        deleteFile( LIST_B_SETTINGS_COPY );

    }
};

var myTestName = "testConfigurator.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new ConfiguratorUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
