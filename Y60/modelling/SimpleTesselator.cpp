//============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================


#include "SimpleTesselator.h"

#include <y60/property_functions.h>
#include <y60/ShapeBuilder.h>
#include <y60/ElementBuilder.h>
#include <y60/PropertyNames.h>
#include <asl/Assure.h>
#include <asl/Matrix4.h>
#include <asl/numeric_functions.h>

#define DB(x) // x
#define DB2(x) // x

using namespace std;
using namespace asl;


#define EPSILON 0.0000000001f

namespace y60 {
    

    SimpleTesselator::SimpleTesselator() {}
    
    SimpleTesselator::~SimpleTesselator() {}

    dom::NodePtr 
    SimpleTesselator::createSurface2DFromContour(y60::ScenePtr theScene, const string & theMaterialId,
                                                 const VectorOfVector2f & theContour,
                                                 const string & theName) {
        ShapeBuilder myShapeBuilder(theName);
        ElementBuilder myElementBuilder(PRIMITIVE_TYPE_TRIANGLES, theMaterialId);
        
        theScene->getSceneBuilder()->appendShape(myShapeBuilder);
        
        unsigned myTotalVerticesCount = theContour.size();
        
        myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE, myTotalVerticesCount);
        myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(NORMAL_ROLE, myTotalVerticesCount);
        myShapeBuilder.appendVertexData(NORMAL_ROLE, Vector3f(0,0,1));
        
        unsigned mySegmentCounter = 1;
        
        VectorOfVector2f a;            
        
        for (int myPointIndex = 0; myPointIndex < theContour.size(); myPointIndex++, mySegmentCounter++) {
            Vector2f myTmp = theContour[myPointIndex];
            a.push_back(Vector2f(myTmp[0], myTmp[1]));
        }      
        
        myElementBuilder.createIndex(POSITION_ROLE, POSITIONS, myTotalVerticesCount);
        myElementBuilder.createIndex(NORMAL_ROLE, NORMALS, myTotalVerticesCount);
      
        VectorOfVector2f result;
      
        // Invoke the triangulator to triangulate this polygon.
        process(a,result);
        
        int tcount = result.size()/3;
        
        for (int i=0; i<tcount; i++)
        {
            const Vector2f &p1 = result[i*3+0];
            const Vector2f &p2 = result[i*3+1];
            const Vector2f &p3 = result[i*3+2];
            myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(p1[0], p1[1], 0.0f));            
            myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(p2[0], p2[1], 0.0f));            
            myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(p3[0], p3[1], 0.0f));            
            //printf("Triangle %d => (%0.0f,%0.0f) (%0.0f,%0.0f) (%0.0f,%0.0f)\n",i+1,p1[0],p1[1],p2[0],p2[1],p3[0],p3[1]);

            myElementBuilder.appendIndex(POSITIONS, i*3+0);
            myElementBuilder.appendIndex(POSITIONS, i*3+1);
            myElementBuilder.appendIndex(POSITIONS, i*3+2);
            myElementBuilder.appendIndex(NORMALS, 0);
            myElementBuilder.appendIndex(NORMALS, 0);
            myElementBuilder.appendIndex(NORMALS, 0);
        }      
        myShapeBuilder.appendElements( myElementBuilder );
        return myShapeBuilder.getNode();
    }

    
    float SimpleTesselator::area(const VectorOfVector2f &contour)
    {
    
        int n = contour.size();
        float A=0.0f;
    
        for(int p=n-1,q=0; q<n; p=q++) {
            A+= contour[p][0]*contour[q][1] - contour[q][0]*contour[p][1];
        }
        return A*0.5f;
    }
    
    /*
      InsideTriangle decides if a point P is Inside of the triangle
      defined by A, B, C.
    */
    bool SimpleTesselator::insideTriangle(float Ax, float Ay,
                                          float Bx, float By,
                                          float Cx, float Cy,
                                          float Px, float Py)
    
    {
        float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
        float cCROSSap, bCROSScp, aCROSSbp;
    
        ax = Cx - Bx;  ay = Cy - By;
        bx = Ax - Cx;  by = Ay - Cy;
        cx = Bx - Ax;  cy = By - Ay;
        apx= Px - Ax;  apy= Py - Ay;
        bpx= Px - Bx;  bpy= Py - By;
        cpx= Px - Cx;  cpy= Py - Cy;
    
        aCROSSbp = ax*bpy - ay*bpx;
        cCROSSap = cx*apy - cy*apx;
        bCROSScp = bx*cpy - by*cpx;
    
        return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
    };
    
    bool SimpleTesselator::snip(const VectorOfVector2f &contour,int u,int v,int w,int n,int *V)
    {
        int p;
        float Ax, Ay, Bx, By, Cx, Cy, Px, Py;
    
        Ax = contour[V[u]][0];
        Ay = contour[V[u]][1];
    
        Bx = contour[V[v]][0];
        By = contour[V[v]][1];
    
        Cx = contour[V[w]][0];
        Cy = contour[V[w]][1];

        if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ) return false;

//        if ( asl::almostEqual(((Bx-Ax)*(Cy-Ay)), ((By-Ay)*(Cx-Ax))) ) return false;
        
        for (p=0;p<n;p++) {
            if( (p == u) || (p == v) || (p == w) ) continue;
            Px = contour[V[p]][0];
            Py = contour[V[p]][1];
            if (insideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
        }
        
        return true;
    }
    
    bool SimpleTesselator::process(const VectorOfVector2f &contour,VectorOfVector2f &result)
    {
        /* allocate and initialize list of Vertices in polygon */
    
        int n = contour.size();
        if ( n < 3 ) return false;
    
        int *V = new int[n];
    
        /* we want a counter-clockwise polygon in V */
    
        if ( 0.0f < area(contour) )
            for (int v=0; v<n; v++) V[v] = v;
        else
            for(int v=0; v<n; v++) V[v] = (n-1)-v;
    
        int nv = n;
    
        /*  remove nv-2 Vertices, creating 1 triangle every time */
        int count = 2*nv;   /* error detection */
    
        for(int m=0, v=nv-1; nv>2; ) {
            /* if we loop, it is probably a non-simple polygon */
            if (0 >= (count--)) {
                //** Triangulate: ERROR - probable bad polygon!
                return false;
            }
    
            // three consecutive vertices in current polygon, <u,v,w> 
            int u = v  ; if (nv <= u) u = 0;     // previous 
            v = u+1; if (nv <= v) v = 0;     // new v    
            int w = v+1; if (nv <= w) w = 0;     // next     
            
            if ( snip(contour,u,v,w,nv,V) ) {
                int a,b,c,s,t;
                
                // true names of the vertices 
                a = V[u]; b = V[v]; c = V[w];
                
                // output Triangle 
                result.push_back( contour[a] );
                result.push_back( contour[b] );
                result.push_back( contour[c] );
                
                m++;
                
                //remove v from remaining polygon 
                for(s=v,t=v+1;t<nv;s++,t++) V[s] = V[t]; nv--;
                // resest error detection counter 
                count = 2*nv;
            }
        }
        
        delete V;
        
        return true;
    }    
}
