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

//own header
#include "ShaderFeatureSet.h"


#include <y60/scene/IShader.h>
#include <asl/base/string_functions.h>

using namespace std;
using namespace asl;

namespace y60 {

ShaderFeatureSet::ShaderFeatureSet() {
}

ShaderFeatureSet::~ShaderFeatureSet() {
}

unsigned
ShaderFeatureSet::getFeatureCount() const {
    return _myFeatureMap.size();
}

bool
ShaderFeatureSet::hasFeature(const std::string & theFeatureClass) const {
    FeatureMap::const_iterator myFeatureSetIter = _myFeatureMap.find(theFeatureClass);
    if (myFeatureSetIter != _myFeatureMap.end() ) {
        return true;
    } else {
        return false;
    }
}

const VectorOfString *
ShaderFeatureSet::getFeatures(const string & theFeatureClass) const {
    FeatureMap::const_iterator myFeatureSetIter = _myFeatureMap.find(theFeatureClass);
    if (myFeatureSetIter != _myFeatureMap.end() ) {
        return &myFeatureSetIter->second;
    } else {
        return 0;
    }
}

const VectorOfTextureUsage &
ShaderFeatureSet::getTextureFeature() const {
    return _myTextureFeature;
}

void
ShaderFeatureSet::load(const dom::NodePtr theNode) {
    string myNodeName = theNode->nodeName();
    if ((myNodeName == "supports") || (myNodeName == "requires")){
        for (unsigned i = 0; i < theNode->childNodesLength("feature"); ++i) {
            dom::NodePtr myFeatureNode = theNode->childNode("feature", i);
            string myFeatureClass = myFeatureNode->getAttributeString("class");
            VectorOfString myFeatures = myFeatureNode->getAttributeValue<VectorOfString>("values");
            if (myFeatureClass == TEXTURE_LIST_NAME) {
                _myTextureFeature.clear();
                for (VectorOfString::size_type i = 0; i< myFeatures.size(); i++) {
                    TextureUsage myUsage;
                    myUsage.fromString(myFeatures[i]);
                    _myTextureFeature.push_back(myUsage);
                }
            }
            _myFeatureMap[myFeatureClass] = myFeatures;
        }
    } else {
        throw y60::ShaderException(string("Expected node <supports> or <requires> but got <") +
                                              myNodeName + ">.", PLUS_FILE_LINE);
    }
}
} // namespace y60


