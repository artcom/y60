
//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: LCDControlerBase.js,v $
//   $Author: david $
//   $Revision: 1.3 $
//   $Date: 2004/06/03 16:45:55 $
//
//
//=============================================================================

use("LCDControler_NT3881D.js");
use("LCDControler_EASER202NRW.js");

function createLCDControler(theModel, theSerialPortNum) {
    if (theModel == "NT3881D") {
        return new LCDControler_NT3881D(theSerialPortNum);
    } else if (theModel == "EASER202NRW") {
        return new LCDControler_EASER202NRW(theSerialPortNum);
    } else {
        print("### ERROR ### LCD model " + theModel + " not supported.");
        return null;
    }
}


function LCDControlerBase(theSerialPortNum) {
    this.Constructor(this, theSerialPortNum);
}

LCDControlerBase.prototype.Constructor = function(self, theSerialPortNum) {
    // private members
    var _mySerialDevice = new SerialDevice(theSerialPortNum);
    self._mySerialDevice = _mySerialDevice; //protected

    // public methods
    self.setup = function(theBaudRate) {
        if ( _mySerialDevice.isOpen ) {
            print("### WARNING #### Serial port already open. Skipping setup");
            return;
        }

        // we need rts/cts for the LCD terminal
        if (theBaudRate == undefined) {
            theBaudRate = 19200;
        }
        _mySerialDevice.open(theBaudRate);
        if ( ! _mySerialDevice.isOpen ) {
            print("### ERROR #### Failed to open serial port.");
        }
        //self.reset();
    }

    self.reset = function() {
        print("### WARNING ####LCDControler method not supported.");
    }

    self.setBacklight = function(theBacklightFlag) {
        print("### WARNING ####LCDControler method not supported.");
    }

    // theContrast : 0-15
    self.setContrast = function(theContrast) {
        print("### WARNING ####LCDControler method not supported.");
    }


    self.clear = function() {
        print("### WARNING ####LCDControler method not supported.");
    }

    self.clearLine = function() {
        print("### WARNING ####LCDControler method not supported.");
    }

    self.write = function(theText) {
        _mySerialDevice.write(theText);
    }

    self.setUserChar = function(theCharCode, theGlyph) {
        print("### WARNING ####LCDControler method not supported.");
    }

    self.uploadFont = function(theFont) {
        self.clear();
        for (var i=0; i<theFont.length; ++i) {
            self.setUserChar(i+1, theFont[i]); //char code 0 is evil, you cannot use it in strings...
        }
    }

    
    self.setText = function(theText) {
        self.clear();
        self.write(theText);
    }

    // theStyle depends on the model, but the following are common 
    //      "OFF"
    //      "BLINKING_BLOCK"
    //      "UNDERSCORE"
    self.setCursorStyle = function(theStyle) {
        print("### WARNING ####LCDControler method not supported.");
    }

    self.setCursorPos = function(theLine, theChar) {
        print("### WARNING ####LCDControler method not supported.");
    }

    self.setResolution = function(theLineCount, theCharsPerLine) {
        print("### WARNING ####LCDControler method not supported.");
    }

    self.reset = function() {
        print("### WARNING ####LCDControler method not supported.");
    }

    self.restoreDefaults = function() {
        print("### WARNING ####LCDControler method not supported.");
    }

    self.saveConfig = function() {
        print("### WARNING ####LCDControler method not supported.");
    }

    self.shutdown = function() {
        _mySerialDevice.close();
    }
}

function createFont(thePixmaps) {
    var myLCDFont = [];
    for (var i=0; i < thePixmaps.length; ++i) {
        var myLCDChar = [];
        for (var y=0; y < 8; ++y) {
            var myRowBits = 0;
            var myLine = String(thePixmaps[i][y]); 
            for (var x=0; x < 5; ++x) {
                if (myLine.charAt(4-x) != " ") {
                    myRowBits |= 1<<x;
                }
            }
            //print (y + ":" + myLine + "="+ myRowBits);
            myLCDChar.push(myRowBits);
        }
        myLCDFont.push(myLCDChar);
    }
    return myLCDFont;
}

