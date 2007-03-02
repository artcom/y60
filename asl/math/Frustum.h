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
//   $RCSfile: Frustum.h,v $
//   $Author: david $
//   $Revision: 1.1 $
//   $Date: 2005/04/08 17:32:53 $
//
//  Description: A simple scene class.
//
//=============================================================================
#ifndef AC_ASL_FRUSTUM_INCLUDED
#define AC_ASL_FRUSTUM_INCLUDED

#include "Plane.h"
#include "Point234.h"
#include "Matrix4.h"
#include "Box.h"

namespace asl {

DEFINE_EXCEPTION(FrustumException, asl::Exception);
    
class Frustum {
    public:
        enum ProjectionType {
            PERSPECTIVE,
            ORTHO
        };
        Frustum();
        Frustum(double theLeft, double theRight, 
                double theBottom, double theTop,  
                double theNear, double theFar, ProjectionType=PERSPECTIVE);
        Frustum(double theX, // HFov or Width
                double theY, // VFov or Height 
                double theNear, double theFar, ProjectionType=PERSPECTIVE);
        
        virtual ~Frustum();
        void updateCorners(float theNearPlane, float theFarPlane, float theHFov,
                           float theOrthoWidth, float theAspectRatio);
        bool getProjectionMatrix(asl::Matrix4f & theProjectionMatrix) const;

        // values suitable for glFrustum()
        double getLeft() const;
        double getRight() const;
        double getTop() const;
        double getBottom() const;
        double getNear() const;
        double getFar() const;
        
        void setLeft(const double & theValue);
        void setRight(const double & theValue);
        void setTop(const double & theValue);
        void setBottom(const double & theValue);
        void setNear(const double & theValue);
        void setFar(const double & theValue);

        ProjectionType getType() const;

        void updatePlanes(const asl::Matrix4f & theCameraTransform, const asl::Matrix4f & theCameraTransformI);

        const asl::Plane<float> & getLeftPlane() const;
        const asl::Plane<float> & getRightPlane() const;
        const asl::Plane<float> & getTopPlane() const;
        const asl::Plane<float> & getBottomPlane() const;
        const asl::Plane<float> & getNearPlane() const;
        const asl::Plane<float> & getFarPlane() const;

        void getCorners(asl::Point3f & theLTF, asl::Point3f & theRBF, 
                        asl::Point3f & theRTF, asl::Point3f & theLBF,
                        asl::Point3f & theLTBK, asl::Point3f & theRBBK, 
                        asl::Point3f & theRTBK, asl::Point3f & theLBBK) const;

    protected:
    private:
        void setSymmetricPerspective(double theHFov, double theVFov, double theNear, double theFar);
        void setSymmetricOrtho(double theWidth, double theHeight, double theNear, double theFar);

        asl::Vector3f constructPlaneNormal(const asl::Vector3f & v1, 
                                           const asl::Vector3f & v2, 
                                           const asl::Matrix4f & T);
        double _myLeft;
        double _myRight;
        double _myBottom;
        double _myTop;
        double _myNear;
        double _myFar;
        ProjectionType _myProjectionType;

        asl::Plane<float> _myLeftPlane;
        asl::Plane<float> _myRightPlane;
        asl::Plane<float> _myTopPlane;
        asl::Plane<float> _myBottomPlane;
        asl::Plane<float> _myNearPlane;
        asl::Plane<float> _myFarPlane;
};

/// 'normal' intersection test
bool
intersection(const asl::Box3f & theBox, const Frustum & theFrustum);

/// intersect also returns partial overlap - used in culling
bool
intersection(const asl::Box3f & theBox, const Frustum & theFrustum, bool & theOverlapFlag);

std::ostream & operator << (std::ostream & os, const Frustum & theFrustum);
std::istream & operator >> (std::istream & os, Frustum & theFrustum);

} // end of namespace
#endif
