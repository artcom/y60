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

use("Y60JSSL.js");

function LCDControler_EASER202NRW(theSerialPortNum) {
    this.Constructor(this, theSerialPortNum);
}

LCDControler_EASER202NRW.prototype.Constructor = function(self, theSerialPortNum)
{

    LCDControlerBase.prototype.Constructor(self, theSerialPortNum);
    self.LCDControlerBase = [];
    var _mySerialDevice = self._mySerialDevice; 

    self.reset = function() {
        _mySerialDevice.write("\x1BR");
    }
    
    self.setBacklight = function(theBacklightFlag) {
        if ( theBacklightFlag ) {
            _mySerialDevice.write("\x1BL\x01");
        } else {
            _mySerialDevice.write("\x1BL\x00");
        }
    }

    self.clear = function() {
        _mySerialDevice.write("\x0C");
        msleep(10);
    }

    self.setUserChar = function(theCharCode, theGlyph) {
        if (theCharCode == 0) {
            print("#### WARNING ##### setting char code for ASCII 0x00 which you cannot use in a string !!");
        }
        var myGlyphCommand = [0x1B,0x5A,theCharCode].concat(theGlyph);
        _mySerialDevice.write(myGlyphCommand);
        msleep(10);
    }

    self.setCursorStyle = function(theStyle) {
        var mySequence;
        switch (theStyle) {
            case "OFF":
                mySequence = [0x1B,0x43,0x00]; "\x1BC\x00";
            break;
            case "BLINKING_BLOCK":
                mySequence = "\x1BC\x01";
            break;
            case "UNDERSCORE":
                mySequence = "\x1BC\x02";
            break;
            case "BLINKING_UNDERSCORE":
                mySequence = "\x1BC\x03";
            default:
            return LCDControlerBase.setCursorStyle(theStyle);
        }
        _mySerialDevice.write(mySequence);
        msleep(10);
    }

    self.setCursorPos = function(theLine, theChar) {
        _mySerialDevice.write([0x1B,theChar,theLine]); 
        msleep(10);
    }

/*
    self.testUserChars = function() {
print("TEST");   
self.clear();
        var myGlyphBytes;
        myGlyphBytes = [27,90,1,0x1f,0x1f,0,0,16,16,15,0];
        _mySerialDevice.write(myGlyphBytes);
        self.write("new letter: |" + "\x01" + "|");
        msleep(500);

        myGlyphBytes = [27,90,1,10,4,15,16,16,16,15,0];
        _mySerialDevice.write(myGlyphBytes);
        self.write(" |" + "\x01" + "|");
        msleep(500);

        msleep(5000);
    }
*/

}

