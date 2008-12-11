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
//
//   $Id: GeometryUtils.cpp,v 1.9 2005/04/24 00:30:18 pavel Exp $
//   $Author: pavel $
//   $Revision: 1.9 $
//   $Date: 2005/04/24 00:30:18 $
//   
//
//  Description: Various geometry utilities
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "GeometryUtils.h"
#include "Triangle.h"
#include <asl/base/string_functions.h>

#define DB(x) // x

using namespace std;

namespace asl {
    

double calculatePolyhedraVolume(const vector<Point3f> & theTriangleVertices)
{
    double myVolume = 0;
    for(vector<Point3f>::size_type i = 0;i < theTriangleVertices.size(); i += 3) {
        const Triangle<float> & myTriangle = asTriangle(theTriangleVertices[i]);
        Vector3f myNormal = myTriangle.normal();
        Vector3f myPosition = myTriangle.centroid(); //this is just fancy. we could take a vertex instead
        
        DB(AC_TRACE << " triangle " << myTriangle << endl);
        DB(AC_TRACE << " P " << myPosition << " N " << myNormal << " A " << myTriangle.area() << endl);
        DB(AC_TRACE << " adding " << dot(myPosition,myNormal)*myTriangle.area()/3 << endl);

        myVolume += dot(myPosition,myNormal)*myTriangle.area()/3;        
    } 
    return myVolume;
    
};

} //namespace

