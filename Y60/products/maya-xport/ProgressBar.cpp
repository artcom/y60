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
//   $RCSfile: ProgressBar.cpp,v $
//   $Author: christian $
//   $Revision: 1.6 $
//   $Date: 2005/04/18 16:46:33 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#include "ProgressBar.h"
#include "ExportExceptions.h"

#include <asl/base/Time.h>
#include <asl/base/string_functions.h>

#include <maya/MString.h>
#include <maya/MGlobal.h>

using namespace std;

ProgressBar::ProgressBar(bool theEnabled) : _myEnabled(theEnabled) {
    // It is neccessary to be able to disable the progress bar, because
    // the maya2x60 batch script does not work with progress bar.
    if (_myEnabled) {
        MGlobal::executeCommand("progressBar -edit -beginProgress -isInterruptable true -status \"Exporting Y60 File\" -maxValue 100 $gMainProgressBar");
    }
}

ProgressBar::~ProgressBar() {
    if (_myEnabled) {
        MGlobal::executeCommand("progressBar -edit -endProgress $gMainProgressBar;");
    }
}

void
ProgressBar::advance(unsigned theStep) {
    if (_myEnabled) {
        string myMelCommand = string("progressBar -edit -step ") + asl::as_string(theStep) + " $gMainProgressBar;";
        MGlobal::executeCommand(myMelCommand.c_str());

        int myResult;
        MGlobal::executeCommand("progressBar -query -isCancelled $gMainProgressBar", myResult);
        if (myResult != 0) {
            throw ExportCancelled("Cancel button was pressed.", PLUS_FILE_LINE);
        }
    }
}

void
ProgressBar::setSize(unsigned theSize) {
    if (_myEnabled) {
        string myMelCommand = string("progressBar -edit -maxValue ") + asl::as_string(theSize) + " $gMainProgressBar";
        MGlobal::executeCommand(myMelCommand.c_str());
    }
}

void
ProgressBar::setStatus(const std::string & theStatus) {
    cerr << theStatus << endl;
    if (_myEnabled) {
        string myMelCommand = string("progressBar -edit -status \"Y60 Exporter: ") + theStatus + "\" $gMainProgressBar;";
        MGlobal::executeCommand(myMelCommand.c_str());
    }
}
