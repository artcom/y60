
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
//   $RCSfile: sv.js,v $
//   $Author: valentin $
//   $Revision: 1.23 $
//   $Date: 2005/04/25 14:59:10 $
//
//
//=============================================================================

 try {
    use("Exception.js");
    use("SceneViewer.js");

    window = new RenderWindow();
    window.multisamplingFactor = 4;

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
