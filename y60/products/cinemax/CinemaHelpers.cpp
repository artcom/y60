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
//   $RCSfile: CinemaHelpers.cpp,v $
//   $Author: martin $
//   $Revision: 1.4 $
//   $Date: 2004/08/19 10:52:56 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#include <asl/base/string_functions.h>
#include <asl/base/file_functions.h>

#include "CinemaHelpers.h"   //include order is mandatory
#include <c4d.h>


using namespace asl;

void
displayMessage(const String & theMessage) {
    displayMessage(getString(theMessage));
}

void displayMessage(const char * theMessage) {
    displayMessage(std::string(theMessage));
}

void
displayMessage(const std::string & theMessage) {
    class MyDialog : public GeModalDialog {
    public:
        void setMessage(const std::string & theMessage) {
            _myMessage = theMessage;
        }

    private:
        Bool CreateLayout() {
            SetTitle("ART+COM Y60 Exporter");

            GroupBegin(100010, 0, 1, 0, "", 0); {
                GroupSpace(4, 4);
                GroupBorderSpace(40, 20, 40, 20);

                AddStaticText(100011, 0, 0, 0, _myMessage.c_str(), 0);
                GePrint("### " + String(_myMessage.c_str()));
            }
            GroupEnd();

            AddDlgGroup(DLG_OK);
            return TRUE;
        }

        std::string _myMessage;
    };

    GePrint(theMessage.c_str());

    MyDialog myDialog;
    myDialog.setMessage(theMessage);
    myDialog.Open();
}

const std::string &
getString(const String & theC4dString) {
    static int mySize = 1000;
    static std::string myString;
    static char * myBuffer = new char [mySize];

    if (theC4dString.GetLength() > mySize) {
        mySize = theC4dString.GetLength();
        delete [] myBuffer;
        myBuffer = new char [mySize];
    }

    theC4dString.GetCString(myBuffer, theC4dString.GetLength() + 1, St7bit);

    myString = myBuffer;

    return myString;
}

const String
getTreeName(BaseObject * theNode) {
    BaseObject* myNode = theNode;
    String myTreeName = myNode->GetName();
    while (myNode->GetUp()) {
        myNode = myNode->GetUp();
        myTreeName = myNode->GetName()+ "/" + myTreeName;
    }
    return myTreeName;
}

void
displayChannelProperties(BaseChannel * theChannel) {
    BaseContainer myBaseContainer = theChannel->GetData();
    std::string myMessage("Channel Properties - ");

    Vector myColor = myBaseContainer.GetVector(BASECHANNEL_COLOR_EX);
    myMessage += "Color: [" + as_string(myColor.x) + "," + as_string(myColor.y) + "," + as_string(myColor.z) + "] | ";

    Real myBrightness = myBaseContainer.GetReal(BASECHANNEL_BRIGHTNESS_EX);
    myMessage += "Brightness: " + as_string(myBrightness) + " | ";

    String myTexture = myBaseContainer.GetString(BASECHANNEL_TEXTURE);
    myMessage += "Texture: " + getString(myTexture) + " | ";

    Real myBlurOffset = myBaseContainer.GetReal(BASECHANNEL_BLUR_OFFSET);
    myMessage += "Blur offset: " + as_string(myBlurOffset) + " | ";

    Real myBlurStrength = myBaseContainer.GetReal(BASECHANNEL_BLUR_STRENGTH);
    myMessage += "Blur strength: " + as_string(myBlurStrength) + " | ";

    LONG myInterpolation = myBaseContainer.GetReal(BASECHANNEL_INTERPOLATION);
    myMessage += "Interpolation: " + as_string(myInterpolation) + " | ";

    LONG myMixmode = myBaseContainer.GetReal(BASECHANNEL_MIXMODE_EX);
    myMessage += "Mix mode: " + as_string(myMixmode) + " | ";

    Real myMixStrength = myBaseContainer.GetReal(BASECHANNEL_MIXSTRENGTH_EX);
    myMessage += "Mix strength: " + as_string(myMixStrength);

    displayMessage(myMessage);
}

