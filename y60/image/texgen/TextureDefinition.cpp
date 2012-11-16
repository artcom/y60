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
//    $RCSfile: TextureDefinition.cpp,v $
//
//     $Author: janbo $
//
//   $Revision: 1.2 $
//
//
// Description:
//
//=============================================================================

#include "TextureDefinition.h"

#include <assert.h>
#include <stdexcept>

#include <asl/dom/Nodes.h>
#include <asl/base/string_functions.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plbitmap.h>
#include <paintlib/planybmp.h>
#include <paintlib/planydec.h>
#include <paintlib/Filter/plfilterresizebilinear.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include "LayerDefinition.h"
#include "XmlHelper.h"

using namespace std;

namespace TexGen {

    TextureDefinition::TextureDefinition (const dom::Node& myNode,
            const string& myDirectory,
            bool showProgress,
            bool myCreateMipMaps, float myIndexTextureSize)
{
    load (myNode, myDirectory, showProgress);
}

TextureDefinition::TextureDefinition (int myIndex)
    : _myIndex (myIndex)
{
}

TextureDefinition::~TextureDefinition() {
   for(std::vector<LayerDefinition*>::size_type i=0;i<_myLayers.size(); delete _myLayers[i++] );
}

void TextureDefinition::setTileSize (double myTileSize) {
    for(std::vector<LayerDefinition*>::size_type i=0;i<_myLayers.size(); _myLayers[i++]->setTileSize (myTileSize) );
}

int TextureDefinition::getIndex () const {
    return _myIndex;
}

void TextureDefinition::addLayer (LayerDefinition* myLayerDefinition) {
    _myLayers.push_back (myLayerDefinition);
}

void TextureDefinition::load (const dom::Node & curNode,
        const string& myDirectory,
        bool showProgress)
{
    using namespace dom;

    _myIndex = getIntXMLParam (curNode, "index");
    int myFileNodeCounter = 0, myColorNodeCounter = 0;
    float myOpacitySum = 0.0f;

    while (const Node& myFileNode = curNode("File", myFileNodeCounter) )
    {
        string myFileName = myDirectory+myFileNode("#text").nodeValue();
        float myOpacity = getFloatXMLParam(myFileNode, "opacity");
        int   mySize    = getDefaultedIntXMLParam (myFileNode, "size", 1);
        if (showProgress) {
            cout << "Loading tile bitmap " << myFileName.c_str() << "..." << endl;
        }
        PLAnyBmp * myBmp = new PLAnyBmp;
        PLAnyPicDecoder theDecoder;
        theDecoder.MakeBmpFromFile (myFileName.c_str(), myBmp);
        assert (myBmp->GetBitsPerPixel() == 32);
        _myLayers.push_back (new LayerDefinition (myBmp, mySize, mySize, myOpacity));
        myOpacitySum += myOpacity;
        myFileNodeCounter++;
    }

    while (const Node& myColorNode = curNode("Color", myColorNodeCounter) )
    {
        float myOpacity = getFloatXMLParam(myColorNode, "opacity");
        if (showProgress) {
            cout << "Creating colored bitmap " << endl;
        }
        PLPixel32 myColor = getXMLColorNode(myColorNode);
        _myLayers.push_back (new LayerDefinition (myColor, myOpacity ));
        myOpacitySum += myOpacity;
        myColorNodeCounter++;
    }

    if (!myFileNodeCounter && !myColorNodeCounter)
    {
        string s;
        s = "Node " + asl::as_string (curNode) + ":\n" +
            "must contain either Color or File field.";
        throw (invalid_argument(s.c_str()));
    }

    if (fabs(myOpacitySum-1.0)>0.0001)
    {
        string s;
        s = "Node " + asl::as_string (curNode) + ":\n" +
            "Sum of opacities must be 1.0";
        throw (invalid_argument(s.c_str()));
    }
}

}

