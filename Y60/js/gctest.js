//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: gctest.js,v $
//   $Author: christian $
//   $Revision: 1.3 $
//   $Date: 2003/12/04 18:55:44 $
//
//
//=============================================================================

print("Garbage Collection Test launching...");

var myElement = Node.createElement("elem");
myElement.attr = "value";
myElement.dynattr = "dynvalue";
print(myElement);

function test() {
    print("<test>");
    var myAttr = myElement.attributes.item(0).nodeValueTyped;
    print("myAttr="+myAttr);
    var myDynAttr = myElement.dynattr;
    print("myDynAttr="+myDynAttr);
    var myNoisy = myElement.noisy;
    print("</test>");
}

print("Garbage Collection Test running...");

for (var i = 0; i < 2; ++i) {
    test();
    print("<gc>");
    gc();
    print("</gc>");
}

var ex = bla;

