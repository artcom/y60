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
