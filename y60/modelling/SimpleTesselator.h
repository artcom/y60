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

#ifndef AC_Y60_SIMPLE_TESSELATOR_FUNCTIONS_INCLUDED
#define AC_Y60_SIMPLE_TESSELATOR_FUNCTIONS_INCLUDED

#include "y60_modelling_settings.h"

#include <y60/scene/Scene.h>

#include <asl/dom/Nodes.h>
#include <asl/math/Vector234.h>
#include <asl/base/Exception.h>

#include <string>
#include <iostream>

namespace y60 {
    DEFINE_EXCEPTION(SimpleTesselatorException, asl::Exception);

    class SimpleTesselator {
        public:
            SimpleTesselator();
            virtual ~SimpleTesselator();
            dom::NodePtr createSurface2DFromContour(y60::ScenePtr theScene, const std::string & theMaterialId,
                                                    const VectorOfVector2f & theContour,
                                                    const std::string & theName = "Surface2DShape",
                                                    float theEqualPointsThreshold = 1e-5);

            // triangulate a contour/polygon, places results in STL vector
            // as series of triangles.
            bool process(const VectorOfVector2f &contour, VectorOfVector2f &result);

            // compute area of a contour/polygon
            float area(const VectorOfVector2f &contour);

            // decide if point Px/Py is inside triangle defined by
            // (Ax,Ay) (Bx,By) (Cx,Cy)
            bool insideTriangle(float Ax, float Ay,
                                float Bx, float By,
                                float Cx, float Cy,
                                float Px, float Py);


        private:
          bool snip(const VectorOfVector2f &contour,int u,int v,int w,int n,int *V);


    };
    typedef asl::Ptr<SimpleTesselator, dom::ThreadingModel> SimpleTesselatorPtr;

}
#endif // AC_Y60_SIMPLE_TESSELATOR_FUNCTIONS_INCLUDED

