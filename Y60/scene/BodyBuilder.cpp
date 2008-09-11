#include "BodyBuilder.h"
#include "Body.h"
#include <y60/base/iostream_functions.h>
#include <y60/base/NodeNames.h>
#include <asl/dom/Nodes.h>

namespace y60 {
    BodyBuilder::BodyBuilder(const std::string & theShapeId, const std::string & theName)
        : TransformBuilderBase(BODY_NODE_NAME, theName)
    {
        if (getNode()->hasFacade()) {
            BodyPtr myBody = getNode()->getFacade<Body>();
            myBody->set<ShapeTag>(theShapeId);
        } else {
            getNode()->appendAttribute(BODY_SHAPE_ATTRIB, theShapeId);
        }
    }

    BodyBuilder::~BodyBuilder() {
    }

    void
    BodyBuilder::appendJointReference(std::string theJoint) {
        // TODO: replace the code with facade stuff
        dom::NodePtr mySkeleton = getNode()->getAttribute(SKELETON_ATTRIB);
        if (!mySkeleton) {
            VectorOfString myStringVector;
            mySkeleton = getNode()->appendAttribute(SKELETON_ATTRIB, myStringVector);
        }
        VectorOfString & myJoints = mySkeleton->nodeValueRefOpen<VectorOfString>();
        myJoints.push_back(theJoint);
        mySkeleton->nodeValueRefClose<VectorOfString>();
    }
}
