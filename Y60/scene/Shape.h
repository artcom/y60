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

#include <asl/Dashboard.h>

#include <y60/CommonTags.h>
#include <y60/NodeNames.h>
#include <y60/DataTypes.h>

#include <dom/Facade.h>
#include <dom/AttributePlug.h>

#include <map>
#include <vector>
#include <string>

namespace y60 {

    class MaterialBase;

    //                  theTagName      theType       theAttributeName        theDefault
    DEFINE_ATTRIBUT_TAG(RenderStyleTag, RenderStyles, RENDER_STYLE_ATTRIB,    TYPE());

    class Shape :
        public dom::Facade,
        public IdTag::Plug,
        public NameTag::Plug,
        public RenderStyleTag::Plug,
        public dom::FacadeAttributePlug<BoundingBoxTag>
    {
        public:        
            Shape(dom::Node & theNode) : 
                dom::Facade(theNode),
                IdTag::Plug(theNode),
                NameTag::Plug(theNode),
                RenderStyleTag::Plug(theNode),
                dom::FacadeAttributePlug<BoundingBoxTag>(this),
                _myVertexCount(0),
                _myLastRenderVersion(0)
            {
            }

            virtual ~Shape() {
            }

            IMPLEMENT_FACADE(Shape);

            Primitive & createPrimitive(PrimitiveType theType, 
                    MaterialBasePtr theMaterial, unsigned int theDomIndex) 
            {    
                _myPrimitives.push_back(PrimitivePtr(new Primitive(theType, theMaterial, 
                                getNode().getAttributeString("id"), theDomIndex)));    
                return *_myPrimitives.back();
            }

            const PrimitiveVector & getPrimitives() const { 
                return _myPrimitives; 
            }
            PrimitiveVector & getPrimitives() { 
                return _myPrimitives; 
            }
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
                // _myRenderStyles.reset();
                _myVertexCount = 0;
                _myLastRenderVersion = 0;
            }

        private:
            Shape();
            Shape(const Shape &);
            Shape & operator=(const Shape &);

            PrimitiveVector    _myPrimitives;   
            // RenderStyles       _myRenderStyles;
            unsigned long      _myVertexCount;
            unsigned long long _myLastRenderVersion;
    };

    typedef asl::Ptr<Shape, dom::ThreadingModel> ShapePtr;
}

#endif // AC_Y60_SCENE_SHAPE_INCLUDED
