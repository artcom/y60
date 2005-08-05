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
//   $RCSfile: MaterialRequirementList.cpp,v $
//   $Author: pavel $
//   $Revision: 1.7 $
//   $Date: 2005/04/24 00:41:21 $
//
//  Description: A simple scene class.
//
//=============================================================================

#include "MaterialRequirementList.h"

#include <asl/string_functions.h>

using namespace std;
using namespace asl;

#define DB(x) // x

namespace y60 {

MaterialRequirementList::MaterialRequirementList() {
}

MaterialRequirementList::~MaterialRequirementList() {
}

const RequirementMap &
MaterialRequirementList::getRequirements() const {
    return _myRequirementMap;
}

RequirementMap &
MaterialRequirementList::getRequirements() {
    return _myRequirementMap;
}

void
MaterialRequirementList::load(const dom::NodePtr theNode) {
    string myNodeName = theNode->nodeName();
    if (myNodeName == "requires"){
        for (unsigned i = 0; i < theNode->childNodesLength("feature"); ++i) {
            dom::NodePtr myFeatureNode = theNode->childNode("feature", i);
            string myFeatureClass = myFeatureNode->getAttributeString("class");
            VectorOfRankedFeature myFeatures = myFeatureNode->getAttributeValue<VectorOfRankedFeature>("values");
            DB(AC_TRACE << "Adding feature class '" << myFeatureClass << "' with values " << myFeatures << endl);
            _myRequirementMap[myFeatureClass] = myFeatures;
        }
    } else {
        throw MaterialRequirementListException(string("Expected node <supports> or <requires> but got <") +
                                              myNodeName + ">.", PLUS_FILE_LINE);
    }
}
} // namespace y60

