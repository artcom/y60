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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ac_renderer_BodyPart_h_
#define _ac_renderer_BodyPart_h_

#include "y60_glrender_settings.h"

#include <y60/scene/Body.h>
#include <y60/scene/MaterialBase.h>
#include <y60/scene/World.h>
#include <y60/base/NodeNames.h>

#include <asl/base/Dashboard.h>

#include <map>

namespace y60 {

    class Primitive;

    class BodyPart {
        public:

            // key structure
            // bits    | 1  |     15     |      32 or 64          |      32 or 64      |
            // purpose | tr |     zd     |      material *        |       body *       |
            //
            // tr         = transparency bit
            // zd         = Z depth
            // material * = the material pointer
            // body *     = the body pointer

            struct Key {
                Key(const MaterialBase * theMaterial, const Body * theBody, asl::Unsigned16 theAlphaBit_ZDepth) :
                        alphaBit_ZDepth(theAlphaBit_ZDepth),
                            material(theMaterial),
                            body(theBody)
                        {}
#if 1
                        bool operator <(const Key & second) const {
                            if (this->body->get<RenderOrderTag>() != second.body->get<RenderOrderTag>()) {
                                return this->body->get<RenderOrderTag>() < second.body->get<RenderOrderTag>();
                            } else if (this->alphaBit_ZDepth < second.alphaBit_ZDepth) {
                                return true;
                            } else if (this->alphaBit_ZDepth == second.alphaBit_ZDepth) {
                                if (this->material < second.material) {
                                    return true;
                                } else if (this->material == second.material) {
                                    return (this->body < second.body);
                                }
                            }
                            return false;
                        }
#else
                        // slower, but deterministic rendering order by id
                        bool operator <(const Key & second) const {
                            if (this->alphaBit_ZDepth < second.alphaBit_ZDepth) {
                                return true;
                            } else if (this->alphaBit_ZDepth == second.alphaBit_ZDepth) {
                                if (this->material->get<IdTag>() < second.material->get<IdTag>()) {
                                    return true;
                                } else if (this->material->get<IdTag>() == second.material->get<IdTag>()) {
                                    return (this->body->get<IdTag>() < second.body->get<IdTag>());
                                }
                            }
                            return false;
                        }
#endif

                bool getTransparencyFlag() const {
                    return 0 != (alphaBit_ZDepth >> 15);
                }

                asl::Unsigned16      alphaBit_ZDepth;
                const MaterialBase * material;
                const Body         * body;
            };

            BodyPart(const World & theWorldPtr, const Body & theBodyPtr, const Shape & theShape,
                     const Primitive & thePrimitive,
                     const std::vector<asl::Planef> & theClippingPlanes,
                     const asl::Box2f & theScissorBox) :
                _myWorld(theWorldPtr),
                _myBody(theBodyPtr),
                _myShape(theShape),
                _myPrimitive(thePrimitive),
                _myClippingPlanes(theClippingPlanes),
                _myScissorBox(theScissorBox)
            {}

            const y60::World & getWorld() const {
                return _myWorld;
            }

            const y60::Body & getBody() const {
                return _myBody;
            }

            const Shape & getShape() const {
                return _myShape;
            }

            const y60::Primitive & getPrimitive() const {
                return _myPrimitive;
            }

            const std::vector<asl::Planef> & getClippingPlanes() const {
                return _myClippingPlanes;
            }

            const asl::Box2f & getScissorBox() const {
                return _myScissorBox;
            }
        private:
            const World &            _myWorld;
            const Body &             _myBody;
            const Shape &            _myShape;
            const y60::Primitive &   _myPrimitive;
            std::vector<asl::Planef> _myClippingPlanes;
            asl::Box2f               _myScissorBox;
    };

    inline
    asl::Unsigned16
    makeBodyKey(const y60::MaterialBase & theMaterial, const Shape & theShape,
                const asl::Matrix4f & theEyeSpaceTransform, double theNear, double theFar)
    {
        asl::Unsigned16 myKey = 0;
        if (theMaterial.get<TransparencyTag>()) {
            const asl::Box3f & myBoundingBox = theShape.get<BoundingBoxTag>();
            float myZIndicator = (-1.0f * (myBoundingBox.getCenter() * theEyeSpaceTransform))[2];
            if (myZIndicator > theFar) {
                myZIndicator = float(theFar);
            } else if (myZIndicator < theNear) {
                myZIndicator = float(theNear);
            }
            double myTmp = 1.0 - ((myZIndicator - theNear)  / (theFar - theNear));
            asl::Unsigned16 myZDepth = (asl::Unsigned16)(myTmp * 32767);
            myKey = myZDepth | 1 << 15;
            //AC_TRACE << " ======= Z = " << myZIndicator << " far = " << theFar <<" near = " << theNear <<std::endl;
            //AC_TRACE << " myTmp = " << myTmp << " myZDepth = " << myZDepth <<" myKey = " << myKey <<std::endl;
        }
        return myKey;
    }

    // Body Parts sorted by material key
    typedef std::multimap<BodyPart::Key, BodyPart> BodyPartMap;
}

#endif

