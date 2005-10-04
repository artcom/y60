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
//   $RCSfile: GUIUtils.js,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/04/20 16:49:06 $
//
//
//=============================================================================


function getFilenameDialog(theTitle, theAction, thePatterns, theShortcuts, theAdditionalWidget) {

    var myFileChooserDialog = new FileChooserDialog(theTitle, theAction);

    if (thePatterns) {
        for (var i = 0; i < thePatterns.length; ++i) {
            myFileChooserDialog.add_filter_pattern( thePatterns[i].pattern, 
                    thePatterns[i].name + " (" + thePatterns[i].pattern + ")");
        }
    }

    if (theShortcuts) {
        for (var i = 0; i < theShortcuts.length; ++i) {
            myFileChooserDialog.add_shortcut_folder( theShortcuts[i] );
        }
    }

    if (theAdditionalWidget) {
        myFileChooserDialog.vbox.pack_start(theAdditionalWidget, Gtk.PACK_SHRINK);
    }
    myFileChooserDialog.add_button(StockID.CANCEL,Dialog.RESPONSE_CANCEL);
    myFileChooserDialog.add_button(StockID.OK,Dialog.RESPONSE_OK);

    var myRetVal = myFileChooserDialog.run();
     // :-( otherwise dialog won't close before mainwindow gets focus
    myFileChooserDialog.hide();

    var myFilename = null;
    if (myRetVal == Dialog.RESPONSE_OK) {
        myFilename = myFileChooserDialog.get_filename();
    }

    return myFilename;
}


function askUserForFilename(theTitle, theSuffix, theAdditionalWidget) {
    var myWriteItFlag = false;
    while ( true ) {
        var myFilename = getFilenameDialog(theTitle, FileChooserDialog.ACTION_SAVE, null, null, theAdditionalWidget);
        if (myFilename && myFilename.length > 0) {
            if (theSuffix) {
                var myFilenameString = new String( basename( myFilename ));
                if (myFilenameString.lastIndexOf('.') == -1) {
                    myFilename += "." + theSuffix;
                }
            }
            if (fileExists(myFilename)) {
                var myDialog = new MessageDialog("<b>File exists.</b>\nDo you want to overwrite it?",
                                                 true, MessageDialog.MESSAGE_QUESTION, 
                                                 MessageDialog.BUTTONS_YES_NO, true);
                var myResponse = myDialog.run();
                // :-( otherwise dialog won't close before mainwindow gets focus
                myDialog.hide();

                if ( myResponse == Dialog.RESPONSE_YES) {
                    myWriteItFlag = true;
                    break;
                }
            } else {
                myWriteItFlag = true;
                break;
            }
        } else { 
            break;
        }
    }
    if (myWriteItFlag) {
        return myFilename;
    }
    return "";
}
