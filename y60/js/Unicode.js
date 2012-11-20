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
//   $RCSfile: Unicode.js,v $
//   $Author: danielk $
//   $Revision: 1.8 $
//   $Date: 2005/04/04 12:18:37 $
//
//
//=============================================================================

Logger.error("This is now obsolete and should not be used. All strings are internally stored as UTF-8");

var UNICODE = [];
UNICODE["Ä"] = String.fromCharCode(195,132);
UNICODE["Ö"] = String.fromCharCode(195,150);
UNICODE["Ü"] = String.fromCharCode(195,156);
UNICODE["ä"] = String.fromCharCode(195,164);
UNICODE["ö"] = String.fromCharCode(195,182);
UNICODE["ü"] = String.fromCharCode(195,188);
UNICODE["ß"] = String.fromCharCode(195,159);
UNICODE["„"] = String.fromCharCode(226,128,158);
UNICODE["”"] = String.fromCharCode(226,128,157);
UNICODE["™"] = String.fromCharCode(226,132,162);
UNICODE["®"] = String.fromCharCode(194,174);
UNICODE["©"] = String.fromCharCode(194,169);
UNICODE["€"] = String.fromCharCode(226,130,172);
UNICODE["o2"] = String.fromCharCode(0,0); // ?
UNICODE["´"] = String.fromCharCode(194,180);
UNICODE["`"] = String.fromCharCode(96);
UNICODE["«"] = String.fromCharCode(194,171);
UNICODE["»"] = String.fromCharCode(194,187);
UNICODE["°"] = String.fromCharCode(194,176);
UNICODE["­"] = String.fromCharCode(194,173);
UNICODE["—"] = String.fromCharCode(226,128,148);
UNICODE["hoch2"] = String.fromCharCode(226,128,148);

UNICODE["Hindi ka"] = String.fromCharCode(224, 164, 149);

var UNICODE_TESTSTRING = "ÄÖÜäöüß„”™®©€´`«»°­—"

function asCharCode(theText) {
    var myResult = "";
    for (var i = 0; i < theText.length; ++i) {
        myResult += theText.charCodeAt(i);
        if (i < theText.length - 1) {
            myResult += "|";
        }
    }
    return myResult;
}

function asUnicodeString(theText) {
    var myResult = [];
    var myUnicodeChar = "";
    for (var i = 0; i < theText.length; ++i) {
        var myCharCode = theText.charCodeAt(i);

        if (myCharCode < 128 || myCharCode >= 192) {
            if (myUnicodeChar.length) {
                myResult.push(myUnicodeChar);
            }
            myUnicodeChar = theText[i];
        } else {
            myUnicodeChar += theText[i];
        }
    }
    if (myUnicodeChar.length) {
        myResult.push(myUnicodeChar);
    }

    return myResult;
}


//coded copied from selfhtml...

function encode_utf8(rohtext) {
    // dient der Normalisierung des Zeilenumbruchs
    rohtext = rohtext.replace(/\r\n/g,"\n");
    var utftext = "";
    for(var n=0; n<rohtext.length; n++)
    {
        // ermitteln des Unicodes des  aktuellen Zeichens
        var c=rohtext.charCodeAt(n);
        // alle Zeichen von 0-127 => 1byte
        if (c<128)
            utftext += String.fromCharCode(c);
        // alle Zeichen von 127 bis 2047 => 2byte
        else if((c>127) && (c<2048)) {
            utftext += String.fromCharCode((c>>6)|192);
            utftext += String.fromCharCode((c&63)|128);}
            // alle Zeichen von 2048 bis 66536 => 3byte
        else {
            utftext += String.fromCharCode((c>>12)|224);
            utftext += String.fromCharCode(((c>>6)&63)|128);
            utftext += String.fromCharCode((c&63)|128);}
    }
    return utftext;
}

function decode_utf8(utftext) {
    var plaintext = ""; var i=0;
    // while-Schleife, weil einige Zeichen uebersprungen werden
    while(i<utftext.length)
    {
        var c = utftext.charCodeAt(i);
        if (c<128) {
            plaintext += String.fromCharCode(c);
            i++;}
        else if((c>191) && (c<224)) {
            var c2 = utftext.charCodeAt(i+1);
            plaintext += String.fromCharCode(((c&31)<<6) | (c2&63));
            i+=2;}
        else {
            c2 = utftext.charCodeAt(i+1);
            var c3 = utftext.charCodeAt(i+2);
            plaintext += String.fromCharCode(((c&15)<<12) | ((c2&63)<<6) | (c3&63));
            i+=3;}
    }
    return plaintext;
}


function testAsUnicodeString() {
    var myTestString = "---abc" + UNICODE["Ä"] + UNICODE["Ö"] + "x" +
        UNICODE["Ü"] + UNICODE["Hindi ka"] + "a" + UNICODE["Hindi ka"] + "bc---";
    var myUnicodeString = asUnicodeString(myTestString);
    print(myUnicodeString);
}


function HowToWriteUTF8() {
    var myIso88591String = "M�in ISO-8859-1 s�per� �tring.";
    var c1 = ("�").charCodeAt(0);
    var c2 = ("�").charCodeAt(0);
    var c3 = ("�").charCodeAt(0);
    var c4 = ("�").charCodeAt(0);
    print ("convert following decimals to octal: " + c1 + " " + c2 + " " + c3 +  " " + c4);
    //here use c1..c4 as octal values
    var myUtf8String = "M\344in raw utf-8 s\374per\262 \337tring.";

    //all following prints should output the same special chars

    print(myUtf8String);
    print (myIso88591String);

    //if you need the encoded string
    //e.g. because somebody else like pango is going to print it...
    var myUtf8EncodedString = encode_utf8(myUtf8String);

    print(decode_utf8(myUtf8EncodedString));


}

//HowToWriteUTF8();

