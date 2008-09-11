//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef AC_Y60_SCENE_SHAPE_INCLUDED
#define AC_Y60_SCENE_SHAPE_INCLUDED

#include "Primitive.h"

#include <asl/base/Dashboard.h>

#include <y60/base/CommonTags.h>
#include <y60/base/NodeNames.h>
#include <y60/base/DataTypes.h>

#include <asl/dom/Facade.h>
#include <asl/dom/AttributePlug.h>

#include <map>
#include <vector>
#include <string>

namespace y60 {

    class MaterialBase;

    DEFINE_EXCEPTION(ShapeException, asl::Exception);

    class Scene;

    class Shape :
        public dom::Facade,
        public IdTag::Plug,
        public NameTag::Plug,
        public RenderStylesTag::Plug,
        public dom::FacadeAttributePlug<BoundingBoxTag>,
        public dom::FacadeAttributePlug<LastActiveFrameTag>
    {
        public:        
            Shape(dom::Node & theNode) : 
                dom::Facade(theNode),
                IdTag::Plug(theNode),
                NameTag::Plug(theNode),
                RenderStylesTag::Plug(theNode),
                dom::FacadeAttributePlug<BoundingBoxTag>(this),
                dom::FacadeAttributePlug<LastActiveFrameTag>(this),
                _myVertexCount(0),
                _myLastRenderVersion(0)
            {
            }

            virtual ~Shape() {
            }

            IMPLEMENT_FACADE(Shape);
#if 0
            Primitive & createPrimitive(PrimitiveType theType, 
                    MaterialBasePtr theMaterial, unsigned int theDomIndex) 
            {    
                _myPrimitives.push_back(PrimitivePtr(new Primitive(theType, theMaterial, 
                                getNode().getAttributeString("id"), theDomIndex)));    
                return *_myPrimitives.back();
            }
#endif
            const PrimitiveVector & getPrimitives() const {
                const_cast<Shape*>(this)->update(); 
                return _myPrimitives; 
            }
            PrimitiveVector & getPrimitives() {
                update(); 
                return _myPrimitives; 
            }
            void update();
            void build();
            unsigned findMaxIndexSize(dom::NodePtr theElementsNode);
            void calculateBoundingBox();
            dom::NodePtr getVertexDataNode(const std::string & theDataName);
            void reverseUpdate();
            /*
            const RenderStyles & getRenderStyles() const { 
                return _myRenderStyles; 
            }
            RenderStyles & getRenderStyles() { 
                return _myRenderStyles; 
            }
            */
            dom::NodePtr getXmlNode() { 
                return getNode().self().lock();
            }
            const dom::NodePtr getXmlNode() const { 
                return getNode().self().lock();
            }
            unsigned long getVertexCount() const { 
                return _myVertexCount;
            }
            unsigned long long getLastRenderVersion() const {
                return _myLastRenderVersion;
            }
            void setLastRenderVersion(unsigned long long theVersion) {
                _myLastRenderVersion = theVersion;
            }
            void enforceReload() {
                _myLastRenderVersion = 0;
            }
            void setVertexCount(unsigned long theVertexCount) {
                _myVertexCount = theVertexCount;
            }
            template <class LINE>
                bool intersect(const LINE & theStick,
                        Primitive::IntersectionList & theIntersectionInfo)
                {
                    MAKE_SCOPE_TIMER(Shape_intersect);
                    bool myResult = false;
                    AC_TRACE << "Intersecting " << _myPrimitives.size() << " primitives";
                    for (int i = 0; i < _myPrimitives.size();++i) {
                        if (_myPrimitives[i]->intersect(theStick, theIntersectionInfo)) {
                            myResult = true;
                        }
                    }
                    return myResult;
                }

            bool collide(const asl::Sphere<float> & theSphere,
                    const asl::Vector3f & theMotion,
                    const asl::Matrix4f & theTransformation,
                    Primitive::SphereContactsList & theSphereContacts);

            bool collide(const asl::Sphere<float> & theSphere,
                    const asl::Vector3f & theMotion,
                    const asl::Matrix4f & theTransformation,
                    const asl::SweptSphereContact<float> & theCurrentContact,
                    Primitive::SphereContacts & theNewContactInfo);

            void clear() {
                _myPrimitives.clear();
                 _myVertexCount = 0;
                _myLastRenderVersion = 0;
            }
            Scene & getScene();
            const Scene & getScene() const;
            void registerDependenciesRegistrators();
        protected:
            virtual void registerDependenciesForBoundingBox();
        private:
            Shape();
            Shape(const Shape &);
            Shape & operator=(const Shape &);

            PrimitiveVector    _myPrimitives;   
            unsigned long      _myVertexCount;
            unsigned long long _myLastRenderVersion;
    };

    typedef asl::Ptr<Shape, dom::ThreadingModel> ShapePtr;
}

#endif // AC_Y60_SCENE_SHAPE_INCLUDED
