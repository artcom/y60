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

#include <y60/Body.h>
#include <y60/MaterialBase.h>
#include <y60/NodeNames.h>

#include <asl/Dashboard.h>

#include <map>

namespace y60 {

    class Primitive;

    class BodyPart {
        public:
            typedef std::pair<asl::Unsigned32, Body*> Key; 
            
            BodyPart(y60::BodyPtr theBodyPtr, const y60::Primitive & thePrimitive,
                     const std::vector<asl::Planef> & theClippingPlanes) :
                _myBody(theBodyPtr), _myPrimitive(thePrimitive), _myClippingPlanes(theClippingPlanes)
            {}

            y60::BodyPtr getBody() const {
                return _myBody;
            }

            const y60::Primitive & getPrimitive() const {
                return _myPrimitive;
            }

            const std::vector<asl::Planef> & getClippingPlanes() const {
                return _myClippingPlanes;
            }
        private:
            y60::BodyPtr             _myBody;
            const y60::Primitive &   _myPrimitive;
            std::vector<asl::Planef> _myClippingPlanes;
    };
    
    inline
    asl::Unsigned16
    makeBodyKey(y60::MaterialBasePtr theMaterial, y60::BodyPtr theBody,
                const asl::Matrix4f & theEyeSpaceTransform, double theNear, double theFar,
                y60::Scene * theScene)
    {
        asl::Unsigned16 myKey = 0;
        if (theMaterial->get<TransparencyTag>()) {
            const asl::Box3f & myBoundingBox = theBody->getShape()->getXmlNode()->getFacade<y60::Shape>()->get<BoundingBoxTag>();
            float myZIndicator = (- 1.0 *  (myBoundingBox.getCenter() * theEyeSpaceTransform))[2];
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

    // key structure (std::pair<Unsigned32, Body*>)
    // ----------------- first -------------------------| ------------ second --------------------  
    // bits    | 1  |     15     |         16           |                  32 or 64              |
    // purpose | tr |     zd     |         mid          |                 bodyPtr                |
    //
    // tr      = transparency bit
    // zd      = Z depth (unused but reserved)
    // mid     = material id
    // bodyPtr = the body pointer

    inline
    BodyPart::Key makeBodyPartKey(asl::Unsigned16 theMaterialID,
                                       y60::Body * theBody,
                                       asl::Signed16 theTransparencyHint)
    {
        return std::make_pair((asl::Unsigned32)theTransparencyHint << 16
                            |(asl::Unsigned32)theMaterialID,
                            theBody);
    }

    inline
    asl::Unsigned16 getMaterialIDFromKey(const BodyPart::Key & theKey) {
        return (asl::Unsigned16)(theKey.first & 0x0000ffff);
    }
    inline
    bool getTransparencyBitFromKey(const BodyPart::Key & theKey) {
        return (theKey.first >> 31);
    }
    
    inline
    y60::Body * getBodyPtrFromKey(const BodyPart::Key & theKey) {
        return theKey.second;
    }

    // Body Parts sorted by material key
    typedef std::multimap<BodyPart::Key, BodyPart> BodyPartMap;
}

#endif

