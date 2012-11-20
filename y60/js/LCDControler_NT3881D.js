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

