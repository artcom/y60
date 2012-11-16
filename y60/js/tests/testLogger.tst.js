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
*/

use("UnitTest.js");

function TestLogger() {
    this.Constructor(this, "TestLogger");
};

TestLogger.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        // Control via env i.e. 'AC_LOG_MODULE_VERBOSITY=INFO/ExclusiveOutput' 
        Logger.info("Log this only for severity INFO into channel: 'ExclusiveOutput'", "ExclusiveOutput");        
        Logger.print("Log this only for severity PRINT into channel: 'ExclusiveOutput'", "ExclusiveOutput");        
        Logger.debug("Log this only for severity DEBUG into channel: 'ExclusiveOutput'", "ExclusiveOutput");        
        Logger.trace("Log this only for severity TRACE into channel: 'ExclusiveOutput'", "ExclusiveOutput");        
        Logger.warning("Log this only for severity WARNING into channel: 'ExclusiveOutput'", "ExclusiveOutput");        
        Logger.error("Log this only for severity ERROR into channel: 'ExclusiveOutput'", "ExclusiveOutput");        
        Logger.fatal("Log this only for severity FATAL into channel: 'ExclusiveOutput'", "ExclusiveOutput");        
        
    }
};

var mySuite = new UnitTestSuite("UnitTest");

try {
    mySuite.addTest(new TestLogger);
    mySuite.run();
} catch (e) {
    print("## An unknown exception occured during execution." + e + "");
    mySuite.incrementFailedCount();
}
