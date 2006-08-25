//============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef AC_Y60_SIMPLE_TESSELATOR_FUNCTIONS_INCLUDED
#define AC_Y60_SIMPLE_TESSELATOR_FUNCTIONS_INCLUDED

#include <y60/Scene.h>

#include <dom/Nodes.h>
#include <asl/Vector234.h>
#include <asl/Exception.h>

#include <string>
#include <iostream>

namespace y60 {
    DEFINE_EXCEPTION(SimpleTesselatorException, asl::Exception);
    class Vector2d
    {
    public:
      Vector2d(float x,float y)
      {
        Set(x,y);
      };
    
      float GetX(void) const { return mX; };
    
      float GetY(void) const { return mY; };
    
      void  Set(float x,float y)
      {
        mX = x;
        mY = y;
      };
    private:
      float mX;
      float mY;
    };
    // Typedef an STL vector of vertices which are used to represent
    // a polygon/contour and a series of triangles.
    typedef std::vector< Vector2d > Vector2dVector;

    class SimpleTesselator {
        public:

            SimpleTesselator();
            virtual ~SimpleTesselator();
            dom::NodePtr createSurface2DFromContour(y60::ScenePtr theScene, const std::string & theMaterialId,
                                                    const VectorOfVector2f & theContour,
                                                    const std::string & theName = "Surface2DShape");

          // triangulate a contour/polygon, places results in STL vector
          // as series of triangles.
          static bool Process(const Vector2dVector &contour, Vector2dVector &result);
        
          // compute area of a contour/polygon
          static float Area(const Vector2dVector &contour);
        
          // decide if point Px/Py is inside triangle defined by
          // (Ax,Ay) (Bx,By) (Cx,Cy)
          static bool InsideTriangle(float Ax, float Ay,
                              float Bx, float By,
                              float Cx, float Cy,
                              float Px, float Py);
        

        private:
          static bool Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V);


    };
    typedef asl::Ptr<SimpleTesselator, dom::ThreadingModel> SimpleTesselatorPtr;

}
#endif // AC_Y60_SIMPLE_TESSELATOR_FUNCTIONS_INCLUDED

