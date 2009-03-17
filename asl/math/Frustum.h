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

#include "asl_math_settings.h"

#include "Plane.h"
#include "Point234.h"
#include "Matrix4.h"
#include "Box.h"

#include <asl/base/Enum.h>

namespace asl {

/** @relates Frustum */
DEFINE_EXCEPTION(FrustumException, asl::Exception);
    
/** @relates Frustum */
enum ProjectionTypeEnum {
    PERSPECTIVE,
    ORTHONORMAL,
    ProjectionTypeEnum_MAX
};

/** @relates Frustum */
DEFINE_ENUM( ProjectionType, ProjectionTypeEnum, ASL_MATH_DECL );

/** @relates Frustum
 * The resize olicy is used to change the aspect ratio of the frustum if the viewports
 * or projective textures aspect changes.
 *
 * NO_ADAPTION - Dont change the frustum aspect, use this in conjunction with onResize
 *               for weird setups.
 * ADAPT_VERTICAL - adapt the vertical size of the frustum. (default)
 * ADAPT_HORIZONTAL - adapt the horizontal size of the frustum.
 */
enum ResizePolicyEnum {
    NO_ADAPTION,
    ADAPT_VERTICAL, // 
    ADAPT_HORIZONTAL, // ... the other way round
    ResizePolicyEnum_MAX
};

DEFINE_ENUM(ResizePolicy, ResizePolicyEnum, ASL_MATH_DECL);

/** This class models perspective and orthonormal viewing volumes. */
class ASL_MATH_DECL Frustum {
    public:
        Frustum();
        Frustum(const Frustum & otherFrustum);
        Frustum(float theLeft, float theRight, 
                float theBottom, float theTop,  
                float theNear, float theFar, ProjectionType=PERSPECTIVE);
        Frustum(float theX, // HFov or Width
                float theY, // VFov or Height 
                float theNear, float theFar, ProjectionType=PERSPECTIVE);
        
        virtual ~Frustum();
        /*
        void updateCorners(float theNearPlane, float theFarPlane, float theHFov,
                           float theOrthoWidth, float theAspectRatio);
        */

        const Frustum & operator = (const Frustum & other);

        ProjectionType getType() const;
        void setType( ProjectionType theProjection);

        /** Return the width of the frustum on the near plane */
        float getWidth() const;
        /** Set the width of the frustum on the near plane */
        void setWidth( const float & theWidth);
        /** Return the height of the frustum on the near plane */
        float getHeight() const;
        /** Set the height of the frustum on the near plane */
        void setHeight( const float & theHeight);

        /** Get the horizontal field of view in degrees. Throws an exception
         *  if the frustum isn't perspective */
        float getHFov() const;
        /** Set the horizontal field of view in degrees. The frustum is switched
         * to perspective mode automatically*/
        void setHFov(const float & theFOV );
        /** Get the vertical field of view in degrees. Throws an exception
         *  if the frustum isn't perspective */
        float getVFov() const;
        /** Set the vertical field of view in degrees. The frustum is switched
         * to perspective mode automatically*/
        void setVFov(const float & theFOV );

        void setHShift(const float & theShift);
        float getHShift() const;
        void setVShift(const float & theShift);
        float getVShift() const;

        void set(float theLeft, float theRight, float theBottom, float theTop, float theNear, float theFar);

        void setSymmetricPerspective(float theHFov, float theVFov, float theNear, float theFar);
        void setSymmetricOrtho(float theWidth, float theHeight, float theNear, float theFar);
        void changeAspectRatio( ResizePolicy thePolicy, float theNewAspect);

        bool getProjectionMatrix(asl::Matrix4f & theProjectionMatrix) const;
        Matrix4f getProjectionMatrix() const;

        void updatePlanes(const asl::Matrix4f & theCameraTransform,
                          const asl::Matrix4f & theCameraTransformI);

        void getCorners(asl::Point3f & theLTF, asl::Point3f & theRBF, 
                        asl::Point3f & theRTF, asl::Point3f & theLBF,
                        asl::Point3f & theLTBK, asl::Point3f & theRBBK, 
                        asl::Point3f & theRTBK, asl::Point3f & theLBBK) const;

        // values suitable for glFrustum()
        const float & getLeft() const;
        void setLeft(const float & theValue);
        const float & getRight() const;
        void setRight(const float & theValue);
        const float & getTop() const;
        void setTop(const float & theValue);
        const float & getBottom() const;
        void setBottom(const float & theValue);
        const float & getNear() const;
        void setNear(const float & theValue);
        const float & getFar() const;
        void setFar(const float & theValue);

        const asl::Plane<float> & getLeftPlane() const;
        const asl::Plane<float> & getRightPlane() const;
        const asl::Plane<float> & getTopPlane() const;
        const asl::Plane<float> & getBottomPlane() const;
        const asl::Plane<float> & getNearPlane() const;
        const asl::Plane<float> & getFarPlane() const;


        // [DS] Used only by stream operators. Maybe we should make them private and make the
        // operators our friends
        typedef asl::FixedVector<6, float> TupleT;
        TupleT asTuple() const;
        void fromTuple(const TupleT & theTuple);
    protected:
    private:

        asl::Vector3f constructPlaneNormal(const asl::Vector3f & v1, 
                                           const asl::Vector3f & v2, 
                                           const asl::Matrix4f & T);


        float _myLeft;
        float _myTop;
        float _myRight;
        float _myBottom;
        float _myNear;
        float _myFar;
        ProjectionType _myProjectionType;

        asl::Plane<float> _myLeftPlane;
        asl::Plane<float> _myRightPlane;
        asl::Plane<float> _myTopPlane;
        asl::Plane<float> _myBottomPlane;
        asl::Plane<float> _myNearPlane;
        asl::Plane<float> _myFarPlane;
};

/// 'normal' intersection test
ASL_MATH_DECL bool
intersection(const asl::Box3f & theBox, const Frustum & theFrustum);

/// intersect also returns partial overlap - used in culling
ASL_MATH_DECL bool
intersection(const asl::Box3f & theBox, const Frustum & theFrustum, bool & theOverlapFlag);

ASL_MATH_DECL std::ostream & operator << (std::ostream & os, const Frustum & theFrustum);
ASL_MATH_DECL std::istream & operator >> (std::istream & os, Frustum & theFrustum);

} // end of namespace
#endif
