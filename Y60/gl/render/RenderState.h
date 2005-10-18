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
//   $RCSfile: RenderState.h,v $
//   $Author: danielk $
//   $Revision: 1.2 $
//   $Date: 2005/03/23 15:08:20 $
//
//  Description:
//
//=============================================================================

#ifndef _ac_y60_RendererState_h_
#define _ac_y60_RendererState_h_

#include <asl/Plane.h>

namespace y60 {

    class RenderState {
        public:
            RenderState() :
                _myWireframeFlag(false),
                _myLightingFlag(true),
                _myBackfaceCullingFlag(false),
                _myFlatShadingFlag(false),
                _myTexturingFlag(true),
                _myDrawNormalsFlag(false),
                _myEnabledClippingPlanes(0)
            {
                init();
            }

            void init();

            void setWireframe(bool theFlag) {
                if (theFlag != _myWireframeFlag) {
                    commitWireframe(theFlag);
                }
            }

            bool getLighting() const {
                return _myLightingFlag;
            }

            void setLighting(bool theFlag) {
                if (theFlag != _myLightingFlag) {
                    commitLighting(theFlag);
                }
            }

            bool getBackfaceCulling() const {
                return _myBackfaceCullingFlag;
            }

            void setBackfaceCulling(bool theFlag) {
                if (theFlag != _myBackfaceCullingFlag) {
                    commitBackfaceCulling(theFlag);
                }
            }

            bool getFlatShading() const {
                return _myFlatShadingFlag;
            }

            void setFlatShading(bool theFlag) {
                if (theFlag != _myFlatShadingFlag) {
                    commitFlatShading(theFlag);
                }
            }

            bool getTexturing() const {
                return _myTexturingFlag;
            }

            void setTexturing(bool theFlag) {
                if (theFlag != _myTexturingFlag) {
                    commitTexturing(theFlag);
                }
            }

            void setDrawNormals(bool theFlag) {
                _myDrawNormalsFlag = theFlag;
            }

            bool getDrawNormals() const {
                return _myDrawNormalsFlag;
            }

            void setClippingPlanes(const std::vector<asl::Planef> & thePlanes);

        private:
            void commitWireframe(bool theFlag);
            void commitLighting(bool theFlag);
            void commitBackfaceCulling(bool theFlag);
            void commitFlatShading(bool theFlag);
            void commitTexturing(bool theFlag);

            bool _myWireframeFlag;
            bool _myLightingFlag;
            bool _myBackfaceCullingFlag;
            bool _myFlatShadingFlag;
            bool _myTexturingFlag;
            bool _myDrawNormalsFlag;
            int  _myEnabledClippingPlanes;
    };
}

#endif

