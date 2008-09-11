//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
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
    for(int i = 0;i < theTriangleVertices.size(); i += 3) {
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

