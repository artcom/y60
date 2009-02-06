/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
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

#ifndef _asl_geometryUtils_h_
#define _asl_geometryUtils_h_

#include "asl_math_settings.h"

#include "linearAlgebra.h"

#include <vector>
#include <map>


namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */
    
    template <class Number>
    Vector3<Number> generateFaceNormal(const Vector3<Number> & theVertexA, 
                                       const Vector3<Number> & theVertexB, 
                                       const Vector3<Number> & theVertexC) {
        Vector3<Number> myResult = normal(theVertexC - theVertexA, theVertexB - theVertexA);
        return myResult;         
    }

    /**
     * Computes the centroid of the given triangle. The centroid is
     * the center of mass of a triangle. It is the point where the
     * triangles medians intersect.
     * @param theVertexA triangle vertex
     * @param theVertexB triangle vertex
     * @param theVertexC triangle vertex
     * @return centroid of the triangle defined by the parameters
     */
    template <class Number>
    Vector3<Number> generateFaceCentroid(const Vector3<Number> & theVertexA, 
                                         const Vector3<Number> & theVertexB, 
                                         const Vector3<Number> & theVertexC) {
        Vector3<Number> myResult = (theVertexA + theVertexB + theVertexC) / 3;
        return myResult;         
    }

    template <class Number>
    Number computeFaceArea(const Vector3<Number> & theVertexA, 
                           const Vector3<Number> & theVertexB, 
                           const Vector3<Number> & theVertexC) {
        Number myResult = length(cross(theVertexB-theVertexA, theVertexC-theVertexA)) / 2;
        return myResult;         
    }


    template <class Number>
    /**
     * VertexNormalBuilder is a helper class to compute the vertex normals for
     * a shape. It gets filled with vertices and faces and can then be asked to
     * compute a vertices normal vector.
     *
     * @ingroup asl-math
     */
    class VertexNormalBuilder {
    public:        
        VertexNormalBuilder();
        ~VertexNormalBuilder();

        /**
         * adds a vertex to internal list of vertices 
         * @return size of the internal list of vertices
         * @note you can use the return value subtracted by 1 to retrieve
         *       the index of the added vertex
         */
        long addVertex( const Vector3<Number> & theVertex);

        /**
        * adds a triangle faces vertices to the VertexNormalBuilder. 
        * @param theVertexAIndex index of the faces first vertex
        * @param theVertexBIndex index of the faces second vertex
        * @param theVertexCIndex index of the faces third vertex
        */
        void addFace( long theVertexAIndex, long theVertexBIndex, long theVertexCIndex );

        /**
         * computes the vertex normal of the vertex theVertexIndex of the face with the
         * normal theFaceNormal.
         * @param theFaceNormal normal of the face, to identify it. 
         * @param theVertexIndex index of the vertex to compute the normal of.
         * @param theRadAngle angle (in radiants) which demarks the limit.
         * @return vertex normal of the given vertex of the face with normal theFaceNormal.
         */
        Vector3<Number> getVertexNormal(const Vector3<Number> & theFaceNormal, 
                      long theVertexIndex, const double theRadAngle = 0.0);

    private:
        struct Polygon {
            long _myVertexAIndex;
            long _myVertexBIndex;
            long _myVertexCIndex;
            Vector3<Number> _myFaceNormal;
        };
        //Polygon * findAdjacentPolygon(long theCurrentPolygonIndex, std::vector<Polygon*> & thePolygonList);
        std::vector<Polygon>    _myPolygons;
        std::vector<Vector3<Number> >    _myVertices;
        typedef std::multimap<long, long> VertexMap;
        typedef VertexMap::value_type VertexMapValue;
        VertexMap _myVertexMap;
    };

    template <class Number>
    VertexNormalBuilder<Number>::VertexNormalBuilder() {
    }
    template <class Number>
    VertexNormalBuilder<Number>::~VertexNormalBuilder() {
    }

    template <class Number>
    long
    VertexNormalBuilder<Number>::addVertex( const Vector3<Number> & theVertex) 
    {
        _myVertices.push_back(theVertex);
        return _myVertices.size();
    }

    template <class Number>
    void 
    VertexNormalBuilder<Number>::addFace(long theVertexAIndex, long theVertexBIndex, long theVertexCIndex ) 
    {
        Polygon myNewPoly;
        myNewPoly._myVertexAIndex = theVertexAIndex;
        myNewPoly._myVertexBIndex = theVertexBIndex;
        myNewPoly._myVertexCIndex = theVertexCIndex;
        myNewPoly._myFaceNormal   = asl::generateFaceNormal( _myVertices[theVertexAIndex], 
                                                             _myVertices[theVertexBIndex], 
                                                             _myVertices[theVertexCIndex]);
        _myPolygons.push_back(myNewPoly);
        long myFaceIndex = _myPolygons.size()-1;
        // insert the faces adjacent to this vertex into the vertexmap so we can retrieve
        // them later on
        _myVertexMap.insert(VertexMapValue(theVertexAIndex, myFaceIndex));
        _myVertexMap.insert(VertexMapValue(theVertexBIndex, myFaceIndex));
        _myVertexMap.insert(VertexMapValue(theVertexCIndex, myFaceIndex));
    }

    template <class Number>
    Vector3<Number> 
    VertexNormalBuilder<Number>::getVertexNormal( const Vector3<Number> & theFaceNormal, 
                 long theVertexIndex, const double theRadAngle) 
    {
        Vector3<Number> myResult(0,0,0);
        double myCounter = 0;
        double myMinimumAngle = cos(theRadAngle);
        // collect all the polygons, adjacent to the vertex
        VertexMap::const_iterator myBegin = _myVertexMap.lower_bound(theVertexIndex);
        VertexMap::const_iterator myEnd = _myVertexMap.upper_bound(theVertexIndex);
        for (VertexMap::const_iterator i = myBegin; i != myEnd; ++i)
        {
            const long & myIndex = (*i).second;
            Polygon & myCurrentPolygon = _myPolygons[myIndex];
            if (cosAngle(myCurrentPolygon._myFaceNormal, theFaceNormal) >= myMinimumAngle)
            {
                myResult = myResult + myCurrentPolygon._myFaceNormal;
                ++myCounter;
            }
       }

        // if we have only one face to count, take the given facenormal
        if (myCounter <= 1) {
            myResult = theFaceNormal;
        } else  {
            myResult = myResult / myCounter;
        }
        return normalized(myResult);
    }


    ASL_MATH_EXPORT double calculatePolyhedraVolume(const std::vector<Point3f> & theTriangleVertices);
    /* @} */
};

#endif
