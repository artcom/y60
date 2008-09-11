//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: JointBuilder.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: JointBuilder.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description:
//
//
//=============================================================================

#include "JointBuilder.h"
#include <y60/base/NodeNames.h>
#include <asl/dom/Nodes.h>

namespace y60 {

    JointBuilder::JointBuilder(const std::string & theName)
        : TransformBuilderBase(y60::JOINT_NODE_NAME, theName)
    {
        if (!getNode()->hasFacade()) {
            getNode()->appendAttribute(y60::JOINT_ORIENTATION_ATTRIB, asl::Quaternionf(0,0,0,1));
        }
    }

    void
    JointBuilder::setJointOrientation(const asl::Vector3f & theJointOrientation) {
        getNode()->getAttribute(y60::JOINT_ORIENTATION_ATTRIB)->nodeValueAssign(asl::Quaternionf::createFromEuler(theJointOrientation));
    }
    
    void
    JointBuilder::setJointOrientation(const asl::Quaternionf & theJointOrientation) {
        getNode()->getAttribute(y60::JOINT_ORIENTATION_ATTRIB)->nodeValueAssign(theJointOrientation);
    }
}

