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
//   $RCSfile: ShaderFeatureSet.cpp,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/10/18 16:22:56 $
//
//  Description: A simple scene class.
//
//=============================================================================

#include "ShaderFeatureSet.h"

#include <y60/IShader.h>
#include <asl/string_functions.h>

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

void
ShaderFeatureSet::load(const dom::NodePtr theNode) {
    string myNodeName = theNode->nodeName();
    if ((myNodeName == "supports") || (myNodeName == "requires")){
        for (unsigned i = 0; i < theNode->childNodesLength("feature"); ++i) {
            dom::NodePtr myFeatureNode = theNode->childNode("feature", i);
            string myFeatureClass = myFeatureNode->getAttributeString("class");
            VectorOfString myFeatures = myFeatureNode->getAttributeValue<VectorOfString>("values");
            _myFeatureMap[myFeatureClass] = myFeatures;
        }
    } else {
        throw y60::ShaderException(string("Expected node <supports> or <requires> but got <") +
                                              myNodeName + ">.", PLUS_FILE_LINE);
    }
}
} // namespace y60


