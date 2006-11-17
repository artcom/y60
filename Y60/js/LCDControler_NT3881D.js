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

function LCDControler_NT3881D(theSerialPortNum) {
    this.Constructor(this, theSerialPortNum);
}

LCDControler_NT3881D.prototype.Constructor = function(self, theSerialPortNum) 
{

    LCDControlerBase.prototype.Constructor(self, theSerialPortNum);
    self.LCDControlerBase = [];
    var _mySerialDevice = self._mySerialDevice;

    self.setBacklight = function(theBacklightFlag) {
        if ( theBacklightFlag ) {
            _mySerialDevice.write("\x1BLB1;");
        } else {
            _mySerialDevice.write("\x1BLB0;");
        }
    }

    // theContrast : 0-15
    self.setContrast = function(theContrast) {
        _mySerialDevice.write("\x1BLK" + theContrast + ";");
    }


    self.clear = function() {
        // the *real* clear sequence somehow crashes the terminal
        // so I had to roll my own. Unfortunately this one has to take
        // a little nap because the terminal does not accept the commands
        // fast enough. :-( DS
        // _mySerialDevice.write("\x1BLL");
        self.setCursorPos(2,1);
        msleep(5);
        self.clearLine();
        msleep(5);
        self.setCursorPos(1,1);
        msleep(5);
        self.clearLine();
        msleep(5);
    }

    self.clearLine = function() {
        _mySerialDevice.write("\x1BLl");
    }

    self.write = function(theText) {
        _mySerialDevice.write(theText);
    }

    self.setUserChar = function(theCharCode, theGlyph) {
        var myGlyphCommand = "\x1BLZ"+theCharCode+","+(theCharCode+0x90);
        for (var i=0; i<8; i++) {
            myGlyphCommand += "," + theGlyph[i];
        }
        myGlyphCommand += ";";
        // print (myGlyphCommand);
        self.write(myGlyphCommand);
        msleep(10);
    }

    // theStyle can be
    //      0 unchanged
    //      1 cursor off
    //      2 cursor underscore
    //      3 blinking block
    self.setCursorStyle = function(theStyle) {
        var mySequence;
        switch (theStyle) {
            case "OFF":
                mySequence = "\x1BLC,,1;";
            break;
            case "BLINKING_BLOCK":
                mySequence = "\x1BLC,,3;";
            break;
            case "UNDERSCORE":
                mySequence = "\x1BLC,,2;";
            break;
            case "UNCHANGED":
                mySequence = "\x1BLC,,0;";
            default:
            return LCDControlerBase.setCursorStyle(theStyle);
        }
        _mySerialDevice.write(mySequence);
    }

    self.setCursorPos = function(theLine, theChar) {
        _mySerialDevice.write("\x1BLC" + theLine + "," + theChar + ",0;");
    }

    self.setResolution = function(theLineCount, theCharsPerLine) {
        _mySerialDevice.write("\x1BLA" + theLineCount + "," + theCharsPerLine + ";");
    }

    self.reset = function() {
        _mySerialDevice.write("\x1BKr;");
    }

    self.restoreDefaults = function() {
        _mySerialDevice.write("\x1BKR;");
    }

    self.saveConfig = function() {
        _mySerialDevice.write("\x1BKS;");
    }
}

