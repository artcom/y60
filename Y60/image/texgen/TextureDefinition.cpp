//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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
#include "LayerDefinition.h"
#include "XmlHelper.h"

#include <asl/dom/Nodes.h>
#include <asl/base/string_functions.h>

#include <paintlib/plbitmap.h>
#include <paintlib/planybmp.h>
#include <paintlib/planydec.h>
#include <paintlib/Filter/plfilterresizebilinear.h>

#include <assert.h>
#include <stdexcept>

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
   for(int i=0;i<_myLayers.size(); delete _myLayers[i++] ); 
}

void TextureDefinition::setTileSize (double myTileSize) {
    for(int i=0;i<_myLayers.size(); _myLayers[i++]->setTileSize (myTileSize) ); 
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

