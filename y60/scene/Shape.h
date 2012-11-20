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

#ifndef AC_Y60_SCENE_SHAPE_INCLUDED
#define AC_Y60_SCENE_SHAPE_INCLUDED

#include "y60_scene_settings.h"

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

    class Y60_SCENE_DECL Shape :
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
            void build(const std::map<dom::UniqueId, std::set<y60::VertexDataRole> >& theRolesPerElementToUpdate);
            void updatePrimitives(const std::set<y60::VertexDataRole>& theRolesToUpdate);
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
                    update();
                    bool myResult = false;
                    AC_TRACE << "Intersecting " << _myPrimitives.size() << " primitives";
                    for (PrimitiveVector::size_type i = 0; i < _myPrimitives.size();++i) {
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
            std::map<std::string, unsigned long long> _myVertexDataVersionMap;
            std::map<dom::UniqueId, unsigned long long> _myElementToVersionMap;
            std::multimap<std::string, dom::NodePtr> _myVertexDataNameToElementMap;
            std::multimap<std::string, std::string> _myVertexDataToRolesMap;
            PrimitiveVector    _myPrimitives;
            unsigned long      _myVertexCount;
            unsigned long long _myLastRenderVersion;
    };

    typedef asl::Ptr<Shape, dom::ThreadingModel> ShapePtr;
}

#endif // AC_Y60_SCENE_SHAPE_INCLUDED
