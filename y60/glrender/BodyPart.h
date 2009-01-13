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
//   $RCSfile: BodyPart.h,v $
//   $Author: pavel $
//   $Revision: 1.10 $
//   $Date: 2005/04/24 00:41:18 $
//
//   Description:
//
//=============================================================================

#ifndef _ac_renderer_BodyPart_h_
#define _ac_renderer_BodyPart_h_

#include "y60_glrender_settings.h"

#include <y60/scene/Body.h>
#include <y60/scene/MaterialBase.h>
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

                        bool operator <(const Key & second) const {
                            if (this->alphaBit_ZDepth < second.alphaBit_ZDepth) {
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

                bool getTransparencyFlag() const {
                    return 0 != (alphaBit_ZDepth >> 15);
                }
                
                asl::Unsigned16      alphaBit_ZDepth;
                const MaterialBase * material;
                const Body         * body;                
            };

            BodyPart(const Body & theBodyPtr, const Shape & theShape, 
                     const Primitive & thePrimitive,
                     const std::vector<asl::Planef> & theClippingPlanes,
                     const asl::Box2f & theScissorBox) :
                _myBody(theBodyPtr), 
                _myShape(theShape),
                _myPrimitive(thePrimitive), 
                _myClippingPlanes(theClippingPlanes),
                _myScissorBox(theScissorBox)
            {}

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

