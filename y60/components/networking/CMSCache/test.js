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

plug("CMSCache");

var myPresentation='<?xml version=\'1.0\' encoding=\'UTF-8\'?>'+ 
'<presentation title="BMW Welt CMS">'+
'    <themepool name="Theme_Pool" lastmodified="Fri, 03 Nov 2006 15:10:38 +0000" server="himmel" trunk="/" backend="OCS" sslport="443" id="aLN/SdlS0UE=" wsdlport="7778">'+
'        <theme lastmodified="Wed, 25 Oct 2006 13:27:02 +0000" type="BMWWeltTheme" id="H9t/SdlS0UE=" name="Designphilosophie">'+
'            <content/>'+
'            <screen lastmodified="Wed, 25 Oct 2006 13:27:02 +0000" type="BMWWeltScreen2VerticalSplit45" id="V+l/SdlS0UE=" variant="normal" name="Designphilosophie_1">'+
'               <content>'+
'                    <externalcontent mimetype="image/png" name="de_leftImage_Designphilosophie_1" language="de" lastmodified="Wed, 25 Oct 2006 13:27:02 +0000" uri="http://himmel.intern.artcom.de/big.b60" field="leftImage" path="big.b60" id="3PV/SdlS0UE="/>'+
'                </content> </screen> </theme> </themepool> </presentation>';

print("downloading to "+getTempDirectory());
var myCMSCache = new CMSCache(getTempDirectory()+"/cmstest", new Node(myPresentation),
                           "OCS", "", "", "" );
myCMSCache.verbose = true;
Logger.info("start");

myCMSCache.synchronize();
var i = 0;
while (! myCMSCache.isSynchronized()) {
    msleep(1000);
    print(++i, " secs")
}
var mySize = readFileAsBlock("/tmp/cmstest/big.b60").size;
print("Rate: ",mySize / (i*1024*1024),"MByte/s");

Logger.info("done, iterationen="+i);


