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

