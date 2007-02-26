//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "NodeNames.h"

namespace y60 {

bool 
isAnalyticGeometry(const std::string & theNodeName ) {
    if (theNodeName == PLANE_NODE_NAME ||
        theNodeName == POINT_NODE_NAME ||
        theNodeName == LINE_SEGMENT_NODE_NAME ||
        theNodeName == VECTOR_NODE_NAME) {
        return true;
    }
    return false;
}

}
