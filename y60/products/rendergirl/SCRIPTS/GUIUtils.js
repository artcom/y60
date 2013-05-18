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
//   $RCSfile: GUIUtils.js,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/04/20 16:49:06 $
//
//
//=============================================================================

// new named-parameters usage:
// getFilenameDialog( { param0: value0, param1: value1, ... } );
// params:
//    parent: <window>
//    title: <string>
//    action: Gtk:ACTION*
//    patterns:
//    shortcuts:
//    additionalWidget: <widget>
function getFilenameDialog(theTitleOrParams, theAction) {
    var myArgs = {};
    if ( typeof(theTitleOrParams) == 'string') {
        myArgs.title = theTitleOrParams;
    } else {
        myArgs = theTitleOrParams;
    }

    if (theAction !== undefined ) { myArgs.action = theAction; };

    var myFileChooserDialog = new FileChooserDialog(myArgs.title, myArgs.action);

    if ('patterns' in myArgs && myArgs.patterns) {
        for (var i = 0; i < myArgs.patterns.length; ++i) {
            myFileChooserDialog.add_filter_pattern( myArgs.patterns[i].pattern,
                    myArgs.patterns[i].name + " (" + myArgs.patterns[i].pattern + ")");
        }
    }

    if ('shortcuts' in myArgs && myArgs.shortcuts) {
        for (var i = 0; i < myArgs.shortcuts.length; ++i) {
            myFileChooserDialog.add_shortcut_folder( myArgs.shortcuts[i] );
        }
    }

    if ('additionalWidget' in myArgs && myArgs.additionalWidget) {
        myFileChooserDialog.vbox.pack_start(myArgs.additionalWidget, Gtk.PACK_SHRINK);
    }
    if ('parent' in myArgs) {
        myFileChooserDialog.set_transient_for(myArgs.parent);
        myFileChooserDialog.modal = true;
    }
    myFileChooserDialog.add_button(StockID.CANCEL,Dialog.RESPONSE_CANCEL);
    myFileChooserDialog.add_button(StockID.OK,Dialog.RESPONSE_OK);
    myFileChooserDialog.set_default_response(Dialog.RESPONSE_OK);

    var myRetVal = myFileChooserDialog.run();
     // :-( otherwise dialog won't close before mainwindow gets focus
    myFileChooserDialog.hide();

    var myFilename = null;
    if (myRetVal == Dialog.RESPONSE_OK) {
        myFilename = myFileChooserDialog.get_filename();
    }

    return myFilename;
}


// new named-parameters usage:
// getFilenameDialog( { param0: value0, param1: value1, ... } );
// params:
//    parent: <window>
//    title: <string>
//    suffix:
//    additionalWidget: <widget>
function askUserForFilename(theTitleOrParams, theSuffix) {
    var myArgs = {};
    if ( typeof(theTitleOrParams) == 'string') {
        myArgs.title = theTitleOrParams;
    } else {
        myArgs = theTitleOrParams;
    }
    if (theSuffix !== undefined ) { myArgs.suffix = theSuffix; };

    var myWriteItFlag = false;
    while ( true ) {
        var myDialogArgs = {};
        myDialogArgs.title = myArgs.title;
        myDialogArgs.action = FileChooserDialog.ACTION_SAVE;
        if ('additionalWidget' in myArgs) {
            myDialogArgs.additionalWidget = myArgs.additionalWidget;
        }
        if ('parent' in myArgs) {
            myDialogArgs.parent = myArgs.parent;
        }
        var myFilename = getFilenameDialog(myDialogArgs);
        if (myFilename && myFilename.length > 0) {
            if ('suffix' in myArgs && myArgs.suffix) {
                var myFilenameString = new String( getFilenamePart( myFilename ));
                if (myFilenameString.lastIndexOf('.') == -1) {
                    myFilename += "." + myArgs.suffix;
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

function postMessage(theParentWindow, theMessage, theErrorLevel, theButtons) {
    var myAdditionalCancelFlag = false;
    if (theButtons == undefined) {
        theButtons = MessageDialog.BUTTONS_OK;
    } else if (theButtons == BUTTONS_YES_NO_CANCEL) {
        theButtons = MessageDialog.BUTTONS_YES_NO;
        myAdditionalCancelFlag = true;
    }
    var myDialog = new MessageDialog(theMessage,
                        true, theErrorLevel,
                        theButtons, true);
    if (myAdditionalCancelFlag) {
        myDialog.add_button(StockID.CANCEL, Dialog.RESPONSE_CANCEL);
    }
    var myParentWasSensitive = theParentWindow.sensitive;
    theParentWindow.sensitive = false;
    myDialog.set_transient_for(theParentWindow);
    myDialog.modal = true;
    var myResponse = myDialog.run();
    myDialog.hide();
    theParentWindow.sensitive = myParentWasSensitive;
    return myResponse;
}

function postError(theParentWindow, theMessage, theButtons) {
    return postMessage(theParentWindow, theMessage, MessageDialog.MESSAGE_ERROR, theButtons);
}

function postWarning(theParentWindow, theMessage, theButtons) {
    return postMessage(theParentWindow, theMessage, MessageDialog.MESSAGE_WARNING, theButtons);
}


