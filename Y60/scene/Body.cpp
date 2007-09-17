//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Body.cpp,v $
//   $Author: pavel $
//   $Revision: 1.3 $
//   $Date: 2005/04/18 19:59:30 $
//
//  Description: Body class.
//
//=============================================================================

#include <asl/Logger.h>
#include <asl/Matrix4.h>
#include <asl/GeometryUtils.h>
#include "Body.h"
#include "Scene.h"

using namespace std;
using namespace asl;

namespace y60 {

    Body::Body(dom::Node & theNode) : TransformHierarchyFacade(theNode),
        ShapeTag::Plug(theNode),
        SkeletonTag::Plug(theNode)
    {}

    void
    Body::registerDependenciesRegistrators() {
        TransformHierarchyFacade::registerDependenciesRegistrators();
        BoundingBoxTag::Plug::setReconnectFunction(&Body::registerDependenciesForBoundingBox);
    }

    void
    Body::registerDependenciesForBoundingBox() {        
        if (getNode()) {
            TransformHierarchyFacade::registerDependenciesForBoundingBox();
            BoundingBoxTag::Plug::dependsOn<ShapeTag>(*this);
            BoundingBoxTag::Plug::dependsOn<BoundingBoxTag>(getShape());
            BodyPtr mySelf = dynamic_cast_Ptr<Body>(getSelf());
            BoundingBoxTag::Plug::getValuePtr()->setCalculatorFunction(mySelf, &Body::recalculateBoundingBox);
        }
    }


    Body::~Body() {
        AC_TRACE << "Body DTOR " << this << endl;
    }
    
    static bool isSupportedPrimitive(const Primitive &thePrimitive) {
        switch (thePrimitive.getType()) {
            case TRIANGLES:
            case TRIANGLE_STRIP:
            case TRIANGLE_FAN:
                return true;
            default:
                return false;
        }
    }    
	ShapePtr 
	Body::getShapePtr() {
        dom::NodePtr myShapeNode = getNode().getElementById(get<ShapeTag>());
        if (!myShapeNode) {
            throw asl::Exception(string("Body ") + get<NameTag>() + ": Could not find shape with id: " + get<ShapeTag>(), PLUS_FILE_LINE);
        } else {
            return myShapeNode->getFacade<Shape>();
        }         
	}
	const ShapePtr 
	Body::getShapePtr() const {
        const dom::NodePtr myShapeNode = getNode().getElementById(get<ShapeTag>());
        if (!myShapeNode) {
            throw asl::Exception(string("Body ") + get<NameTag>() + ": Could not find shape with id: " + get<ShapeTag>(), PLUS_FILE_LINE);
        } else {
            return myShapeNode->getFacade<Shape>();
        }         
	}

    Shape & 
    Body::getShape() { 
        return *(getShapePtr());
    }
    
    const Shape & 
    Body::getShape() const { 
        return *(getShapePtr());
    }

    double Body::calculateVolume(const Scene & theScene) const {
        double myVolume = 0;

        //Logger::get().setVerbosity(SEV_DEBUG);
        AC_DEBUG << " for body " << get<NameTag>() << endl; 
        
        const Shape & myShape = getShape();
        const PrimitiveVector & myPrimitives = myShape.getPrimitives();
        
        //AC_DEBUG << " analyzing " << myShape.get<NameTag>() 
        //         << myPrimitives.size() << " primitives " << endl; 
        
        for (int i = 0; i < myPrimitives.size(); ++i) {
            
        
            if ( ! isSupportedPrimitive(*myPrimitives[i])) {
                /*AC_ERROR << " at shape " << myShape.get<NameTag>()
                         << " skipping unsupported primitive of type " 
                         << PrimitiveTypeString[ myPrimitives[i].getType() ]
                         << endl;*/
                continue;
            }
            
            const Matrix4f & myBodyMatrix = get<GlobalMatrixTag>();
            
            asl::Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = myPrimitives[i]->getConstLockingPositionsAccessor();
            const VertexData3f & myPositions = myPositionAccessor->get();
            AC_DEBUG << " have " << myPositions.size() << " positions " << endl; 
            AC_DEBUG << " applying global transform " << myBodyMatrix << endl; 

            vector<Point3f> myTriangles(myPositions.size());
            for(int j = 0;j < myPositions.size(); ++j) {
                myTriangles[j] = product( asPoint(myPositions[j]), myBodyMatrix);
                //AC_DEBUG << "P " << myPositions[j] << " - " << myTriangles[j] << endl; 
            }
            

            myVolume += calculatePolyhedraVolume(myTriangles);
            AC_DEBUG << " and got a volume " << endl; 
            
        }
        return myVolume;
    }
 
    void
    Body::recalculateBoundingBox() {  
        const Shape & myShape = getShape();
        // Get shape bounding box
        const asl::Box3f & myShapeBoundingBox = myShape.get<BoundingBoxTag>();

        // Multiply with global matrix to transform shape bb into world coordinates
        asl::Box3f myBoundingBox = myShapeBoundingBox * get<GlobalMatrixTag>();

        // Extend by child bounding boxes
        TransformHierarchyFacade::recalculateBoundingBox();
        myBoundingBox.extendBy(get<BoundingBoxTag>());
        set<BoundingBoxTag>(myBoundingBox);
    }

    BodyPtr 
    Body::create(dom::NodePtr theParent, const std::string & theShapeId) {
        dom::NodePtr myNode = dom::NodePtr(new dom::Element(BODY_NODE_NAME));
        myNode->appendAttribute(BODY_SHAPE_ATTRIB, theShapeId);
        myNode = theParent->appendChild(myNode);
        return myNode->getFacade<Body>();
    }
}




