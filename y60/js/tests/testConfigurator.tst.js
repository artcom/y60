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

use("UnitTest.js");
use("Configurator.js");

function ConfiguratorUnitTest() {
    this.Constructor(this, "ConfiguratorUnitTest");
};

const COMMON_SETTINGS = searchFile("fixtures/common_settings.xml");
const MERGED_SETTINGS = searchFile("fixtures/merged_settings.xml");
const LIST_A_SETTINGS = searchFile("fixtures/settings_list_a.xml");
const LIST_B_SETTINGS = searchFile("fixtures/settings_list_b.xml");

ConfiguratorUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        try {

            var myConfigurator = new Configurator( null, COMMON_SETTINGS,
                                                   [LIST_A_SETTINGS, LIST_B_SETTINGS] );
            obj.mySettingsFileList = myConfigurator.getSettingsFileList();
            ENSURE( "obj.mySettingsFileList[0] == LIST_A_SETTINGS && " +
                    "obj.mySettingsFileList[1] == LIST_B_SETTINGS" );

            var myPath = searchFile(COMMON_SETTINGS);
            testLoadSettings();
            testSaveSettings();

        } catch (e) {
            print( e );
            obj.incrementFailedCount();
        }
    }

    function testLoadSettings() {
        try {
            var myConfigurator = new Configurator( null, COMMON_SETTINGS,
                                                   [LIST_A_SETTINGS, LIST_B_SETTINGS] );

            obj.mySettings = myConfigurator.getSettings();
            obj.myResult = new Node();
            obj.myResult.parseFile( MERGED_SETTINGS );
            obj.myResult = obj.myResult.firstChild;
            ENSURE("obj.mySettings.toString() == obj.myResult.toString()");
        } catch (e) {
            print( e );
            obj.incrementFailedCount();
        }
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
