//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

 try {
    use("Exception.js");
    use("SceneViewer.js");

    var myArgs = parseArguments(arguments, { "multisamples" : "%s",
                                             "m"            : "%s" }); 
    var myMultisampling = 8;
    if ("multisamples" in myArgs.options) {
        myMultisampling = Number(myArgs.options.multisamples); 
    }
    
    if ("m" in myArgs.options) {
        myMultisampling = Number(myArgs.options.m); 
    }

    if ("help" in myArgs.options) {
        print("ART+COM SceneViewer Usage: sv filename.[x60/b60] [options]");
        print("available options:");
        print(" --help                         - print this help message");
        print(" --m or --multisamples [number] - specify the Number of multisamples."); 
        print("                                  Must a be power of two.");
    }

    window = new RenderWindow();
    if (operatingSystem() == "LINUX") {
        window.multisamples = Math.min(4, myMultisampling);
    } else {
        window.multisamples = myMultisampling;
    }

    var ourShow = new SceneViewer(arguments);
    var myModel = ourShow.getModelName();

    if (myModel && myModel.search(/\.st./) != -1) {
        plug("y60StlImporter");
    }
    if (myModel && myModel.search(/\.x3d/) != -1) {
        plug("y60X3dImporter");
    }

    ourShow.setup(1024,768);
    ourShow.go();
} catch (ex) {
    reportException(ex);
}
