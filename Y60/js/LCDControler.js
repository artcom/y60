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
//   $RCSfile: LCDControler.js,v $
//   $Author: david $
//   $Revision: 1.3 $
//   $Date: 2004/06/03 16:45:55 $
//
//
//=============================================================================

function LCDControler(theSerialPortNum) {
    this.Constructor(this, theSerialPortNum);
}

LCDControler.prototype.Constructor = function(obj, theSerialPortNum) {
    // private members
    var _mySerialDevice = new SerialDevice(theSerialPortNum);

    // public methods
    obj.setup = function() {
        // we need rts/cts for the LCD terminal
        _mySerialDevice.open(19200);
        if ( ! _mySerialDevice.isOpen ) {
            print("Failed to open serial port.");
        }
    }

    obj.setBacklight = function(theBacklightFlag) {
        if ( theBacklightFlag ) {
            _mySerialDevice.write("\x1BLB1;");
        } else {
            _mySerialDevice.write("\x1BLB0;");
        }
    }

    // theContrast : 0-15
    obj.setContrast = function(theContrast) {
        _mySerialDevice.write("\x1BLK" + theContrast + ";");
    }


    obj.clear = function() {
        // the *real* clear sequence somehow crashes the terminal
        // so I had to roll my own. Unfortunately this one has to take
        // a little nap because the terminal does not accept the commands
        // fast enough. :-( DS
        // _mySerialDevice.write("\x1BLL");
        obj.setCursorPos(2,1);
        msleep(5);
        obj.clearLine();
        msleep(5);
        obj.setCursorPos(1,1);
        msleep(5);
        obj.clearLine();
        msleep(5);
    }

    obj.clearLine = function() {
        _mySerialDevice.write("\x1BLl");
    }

    obj.write = function(theText) {
        _mySerialDevice.write(theText);
    }

    obj.setUserChar = function(theCharCode, theGlyph) {
        var myGlyphCommand = "\x1BLZ"+theCharCode+","+(theCharCode+0x90);
        for (var i=0; i<8; i++) {
            myGlyphCommand += "," + theGlyph[i];
        }
        myGlyphCommand += ";";
        // print (myGlyphCommand);
        obj.write(myGlyphCommand);
        msleep(10);
    }

    obj.uploadFont = function(theFont) {
        obj.clear();
        for (var i=0; i<theFont.length; ++i) {
            obj.setUserChar(i, theFont[i]);
        }
    }

    
    obj.setText = function(theText) {
        obj.clear();
        obj.write(theText);
    }

    // theStyle can be
    //      0 unchanged
    //      1 cursor off
    //      2 cursor underscore
    //      3 blinking block
    obj.setCursorStyle = function(theStyle) {
        _mySerialDevice.write("\x1BLC,," + theStyle + ";");
    }

    obj.setCursorPos = function(theLine, theChar) {
        _mySerialDevice.write("\x1BLC" + theLine + "," + theChar + ",0;");
    }

    obj.setResolution = function(theLineCount, theCharsPerLine) {
        _mySerialDevice.write("\x1BLA" + theLineCount + "," + theCharsPerLine + ";");
    }

    obj.reset = function() {
        _mySerialDevice.write("\x1BKr;");
    }

    obj.restoreDefaults = function() {
        _mySerialDevice.write("\x1BKR;");
    }

    obj.saveConfig = function() {
        _mySerialDevice.write("\x1BKS;");
    }

    obj.shutdown = function() {
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
            // print (y + ":" + myLine + "="+ myRowBits);
            myLCDChar.push(myRowBits);
        }
        myLCDFont.push(myLCDChar);
    }
    return myLCDFont;
}

