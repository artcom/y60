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
//    $RCSfile: Matrix4.h,v $
//
//    $Author: david $
//
//    $Revision: 1.57 $
//
//    Description: Matrix class
//
//=============================================================================

#ifndef _included_asl_Matrix4_
#define _included_asl_Matrix4_

#include "asl_math_settings.h"

#include "Matrix4Base.h"
#include "Quaternion.h"
#include "Point234.h"
#include "numeric_functions.h"

#include <asl/base/Enum.h>
#include <asl/base/Logger.h>

namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    /** @relates Matrix4
     * - Identity: @f[ I = \left( \begin{array}{cccc}
     *                           1 & 0 & 0 & 0 \\
     *                           0 & 1 & 0 & 0 \\
     *                           0 & 0 & 1 & 0 \\
     *                           0 & 0 & 0 & 1 \\
     *                      \end{array} \right) @f]
     *
     * All rotations are performed clockwise.
     *  - X rotating: @f[ M_{XRot} = \left( \begin{array}{cccc}
     *                          1 &  0            & 0            & 0 \\
     *                          0 &  \cos(\alpha) & \sin(\alpha) & 0 \\
     *                          0 & -\sin(\alpha) & \cos(\alpha) & 0 \\
     *                          0 &  0            & 0            & 1 \\
     *                    \end{array} \right) @f]
     *
     * - Y rotating: @f[ M_{YRot} = \left( \begin{array}{cccc}
     *                        \cos(\alpha) & 0 & -\sin(\alpha) & 0 \\
     *                        0            & 1 &  0            & 0 \\
     *                        \sin(\alpha) & 0 &  \cos(\alpha) & 0 \\
     *                        0            & 0 &  0            & 1 \\
     *                   \end{array} \right) @f]
     *
     * - Z rotating: @f[ M_{ZRot} = \left( \begin{array}{cccc}
     *                      \cos(\alpha)  & \sin(\alpha) & 0 & 0 \\
     *                      -\sin(\alpha) & \cos(\alpha) & 0 & 0 \\
     *                      0             & 0            & 1 & 0 \\
     *                      0             & 0            & 0 & 1 \\
     *                   \end{array} \right) @f]
     *
     * - Rotating:
     *   Any combination of X, Y and Z rotating matrices
     *
     * - Scaling: @f[ M_{Scale} = \left( \begin{array}{cccc}
     *                      ? & 0 & 0 & 0 \\
     *                      0 & ? & 0 & 0 \\
     *                      0 & 0 & ? & 0 \\
     *                      0 & 0 & 0 & 1 \\
     *                   \end{array} \right) @f]
     *
     * - Orthogonal:  @f[ M_{ortho} = \left( \begin{array}{cccc}
     *                      ? & ? & ? & 0 \\
     *                      ? & ? & ? & 0 \\
     *                      ? & ? & ? & 0 \\
     *                      0 & 0 & 0 & 1 \\
     *                   \end{array} \right) @f]
     *     - a matrix is orthogonal if @f$ MM^T = I @f$ where @f$M^T@f$ is
     *       the transpose of @f$M@f$ and @f$I@f$ is the identity matrix.
     *     - scaling * rotating = orthogonal
     *     - orthogonal * orthogonal = orthogonal
     *
     * - Linear: @f[ M_{linear} = \left( \begin{array}{cccc}
     *                    ? & ? & ? & 0 \\
     *                    ? & ? & ? & 0 \\
     *                    ? & ? & ? & 0 \\
     *                    0 & 0 & 0 & 1 \\
     *               \end{array} \right) @f]
     *     - linear * linear = linear
     *
     * - Translating: M_{Trans} = @f[ \left( \begin{array}{cccc}
     *                          1 & 0 & 0 & 0 \\
     *                          0 & 1 & 0 & 0 \\
     *                          0 & 0 & 1 & 0 \\
     *                          ? & ? & ? & 1 \\
     *                    \end{array} \right) @f]
     *
     * - Affine: @f[ M_{Affine} = \left( \begin{array}{cccc}
     *                      ? & ? & ? & 0 \\
     *                      ? & ? & ? & 0 \\
     *                      ? & ? & ? & 0 \\
     *                      ? & ? & ? & 1 \\
     *               \end{array} \right) @f]
     *     - affine * affine = affine
     *
     *  - Unknown: @f[ M = \left( \begin{array}{cccc}
     *                      ? & ? & ? & ? \\
     *                      ? & ? & ? & ? \\
     *                      ? & ? & ? & ? \\
     *                      ? & ? & ? & ? \\
     *                 \end{array} \right) @f]
     */
     // Make sure to add a string to MatrixTypeStrings[] in the cpp file for each enum
     enum MatrixTypeEnum {
         IDENTITY,
         X_ROTATING,
         Y_ROTATING,
         Z_ROTATING,
         ROTATING,
         SCALING,
         ORTHOGONAL,
         LINEAR,
         TRANSLATING,
         AFFINE,
         UNKNOWN,
         MatrixTypeEnum_MAX
     };

     //template class ASL_MATH_DECL asl::Enum<MatrixTypeEnum, MatrixTypeEnum_MAX>;

     DEFINE_ENUM( MatrixType, MatrixTypeEnum, ASL_MATH_DECL );


    template <class Number>
    class Matrix4 : protected Matrix4Base<Number> {
        typedef Matrix4Base<Number> Base;
    public:
        // These friends of us need to access the storage directly or call
        // functions that take a base as an agrument. Therefore we allow
        // them to call getBase()
        template<class T> friend
        std::istream & parseMatrix(std::istream & is, asl::Matrix4<T> & theMatrix,
                               const char theStartToken,
                               const char theEndToken,
                               const char theDelimiter);

        template<class T> friend
        std::ostream & printMatrix(std::ostream & os, const asl::Matrix4<T> & theMatrix);


        template <class T> friend
        Point3<T> product(const Point3<T> & p, const Matrix4<T> & m);

        template <class T> friend
        Vector4<T> product(const Vector4<T> & v,
                           const Matrix4<T> & theMatrix);

        template <class T> friend
        bool equal(const asl::Matrix4<T> & M, const asl::Matrix4<T> & N);

        template <class T> friend
        bool unequal(const asl::Matrix4<T> & M, const asl::Matrix4<T> & N);

        template <class T> friend
        bool almostEqual(const asl::Matrix4<T> & myMatrix1,
                         const asl::Matrix4<T> & myMatrix2,
                         T theTolerance);

        friend class Matrix4UnitTest<Number>;

        typedef Number Angle;

        // avoids gcc 3.2 warning about implicit
        typedef typename Matrix4Base<Number>::Row Row;
        typedef typename Matrix4Base<Number>::Column Column;


        static const Matrix4<Number> & Identity() {
            static Matrix4<Number> myIdentity;
            if (myIdentity.getType() != IDENTITY) {
                myIdentity.makeIdentity();
            }
            return myIdentity;
        }

        Matrix4() {
            _myType = UNKNOWN;
        }

        Matrix4(const Matrix4<Number> & a) : Matrix4Base<Number>(a) {
            _myType = a.getType();
        }
        Matrix4<Number> & operator=(const Matrix4<Number> & a) {
            assign(a);
            return *this;
        }

        /*
         * Build a rotation matrix, given a quaternion rotation.
        */
        Matrix4(const Quaternion<Number> & q) {
            const Number & qx = q[0];
            const Number & qy = q[1];
            const Number & qz = q[2];
            const Number & qw = q[3];

            this->val[0][0] = Number(1.0 - 2.0 * (qy * qy + qz * qz));
            this->val[0][1] = Number(2.0 * (qx * qy - qz * qw));
            this->val[0][2] = Number(2.0 * (qz * qx + qy * qw));
            this->val[0][3] = Number(0.0);

            this->val[1][0] = Number(2.0 * (qx * qy + qz * qw));
            this->val[1][1] = Number(1.0 - 2.0 * (qz * qz + qx * qx));
            this->val[1][2] = Number(2.0 * (qy * qz - qx * qw));
            this->val[1][3] = Number(0.0);

            this->val[2][0] = Number(2.0 * (qz * qx - qy * qw));
            this->val[2][1] = Number(2.0 * (qy * qz + qx * qw));
            this->val[2][2] = Number(1.0 - 2.0 * (qy * qy + qx * qx));
            this->val[2][3] = Number(0.0);

            this->val[3][0] = Number(0.0);
            this->val[3][1] = Number(0.0);
            this->val[3][2] = Number(0.0);
            this->val[3][3] = Number(1.0);

            _myType = ROTATING;
        }

        const Row & operator[](int i) const {
            return this->val[i];
        }
        Row & operator[](int i) {
            return this->val[i];
        }

        void assign(const Matrix4<Number> & a) {
            Matrix4Base<Number>::assign(a);
            _myType = a.getType();
        }

        void assign(Number a00, Number a01, Number a02, Number a03, Number a10, Number
            a11, Number a12, Number a13, Number a20, Number a21, Number a22,
            Number a23, Number a30, Number a31, Number a32, Number a33, MatrixType theType)
        {
            Matrix4Base<Number>::assign(a00, a01, a02, a03, a10, a11, a12, a13, a20, a21, a22,
                                a23, a30, a31, a32, a33);
            _myType = theType;
        }

        // Automatically calculates the matrix type
        void assign(Number a00, Number a01, Number a02, Number a03, Number a10, Number
            a11, Number a12, Number a13, Number a20, Number a21, Number a22,
            Number a23, Number a30, Number a31, Number a32, Number a33)
        {
            Matrix4Base<Number>::assign(a00, a01, a02, a03, a10, a11, a12, a13, a20, a21, a22,
                                a23, a30, a31, a32, a33);
            _myType = classifyMatrix();
        }

        void assignRow(int n,const Row & r) {
            Matrix4Base<Number>::assignRow(n, r);
            _myType = classifyMatrix();
        }

        void assignColumn(int n,const Column & r) {
            Matrix4Base<Number>::assignColumn(n, r);
            _myType = classifyMatrix();
        }

        const Number * getData() const {
            return Base::begin()->begin();
        }

        Vector4<Number> getRow(int i) const {
            return Matrix4Base<Number>::getRow(i);
        }

        Vector4<Number> getColumn(int i) const {
            return Matrix4Base<Number>::getColumn(i);
        }

        MatrixType getType() const {
            return _myType;
        }

        void setType(MatrixType theType) {
            _myType = theType;
        }

		Vector3<Number> getTranslation() const {
            return Matrix4Base<Number>::getTranslation();
		}

		void setTranslation(const Vector3<Number> & theTranslation) {
            Matrix4Base<Number>::setTranslation(theTranslation);

            if (_myType == IDENTITY) {
                _myType = TRANSLATING;
            } else if (_myType < TRANSLATING) {
                _myType = AFFINE;
            }
		}

        // BEWARE: getScale only work for non-rotating matrices
        // any other matrix type use matrix::decompose instead!!
		Vector3<Number> getScale() const {
            return Matrix4Base<Number>::getScale();
		}
		void setScale(const Vector3<Number> & theScale) {
            Matrix4Base<Number>::setScale(theScale);

            if (_myType == IDENTITY) {
                _myType = SCALING;
            } else if (_myType < LINEAR && _myType != SCALING) {
                _myType = LINEAR;
            } else if (_myType == TRANSLATING) {
                _myType = AFFINE;
            }
		}

        std::string getTypeString() const {
            return _myType.asString();
        }

        void makeIdentity() {
            if (_myType != IDENTITY) {
                base::makeIdentity();
                _myType = IDENTITY;
            }
        }
        void makeXRotating(Angle a) {
            if (_myType != IDENTITY) {
                base::makeIdentity();
            }
            if (a == Angle(0)) {
                _myType = IDENTITY;
                return;
            }
            base::makeXRotating(a);
            _myType = X_ROTATING;
        }
        void makeYRotating(Angle a) {
            if (_myType != IDENTITY) {
                base::makeIdentity();
            }
            if (a == Angle(0)) {
                _myType = IDENTITY;
                return;
            }
            base::makeYRotating(a);
            _myType = Y_ROTATING;
        }
        void makeZRotating(Angle a) {
            if (_myType != IDENTITY) {
                base::makeIdentity();
            }
            if (a == Angle(0)) {
                _myType = IDENTITY;
                return;
            }
            base::makeZRotating(a);
            _myType = Z_ROTATING;
        }
        void makeRotating(const Vector3<Number> & axis, Angle a) {
            if (a == Angle(0)) {
                base::makeIdentity();
                _myType = IDENTITY;
                return;
            }
            base::makeRotating(axis,a);
            _myType = ROTATING;
        }
        void makeScaling(const Vector3<Number> & s) {
            if (_myType != IDENTITY) {
                base::makeIdentity();
            }
            if (s[0] != Number(1) || s[1] != Number(1) || s[2] != Number(1)) {
                base::makeScaling(s);
                _myType = SCALING;
            } else {
                _myType = IDENTITY;
            }
            // sx 0  0  0
            // 0  sy 0  0
            // 0  0  sz 0
            // 0  0  0  1
        }
        void makeTranslating(const Vector3<Number> & t) {
            if (_myType != IDENTITY && _myType != TRANSLATING) {
                base::makeIdentity();
            }
            base::makeTranslating(t);
            if (t[0] != Number(0) || t[1] != Number(0) || t[2] != Number(0)) {
                _myType = TRANSLATING;
            } else {
                _myType = IDENTITY;
            }
            // 1  0  0  0
            // 0  1  0  0
            // 0  0  1  0
            // tx ty tz 1
        }

        void makeLookAt(const Vector3<Number> &theEyePos,
                        const Vector3<Number> &theCenterPos,
                        const Vector3<Number> &theUpVector) {
            Vector3<Number> f = normalized(theCenterPos - theEyePos);
            Vector3<Number> up = normalized(theUpVector);
            Vector3<Number> s = cross(f,up);
            Vector3<Number> u = cross(s,f);

            assign(s[0], s[1], s[2], 0,
                   u[0], u[1], u[2], 0,
                   -f[0], -f[1], -f[2], 0,
                   -theEyePos[0], -theEyePos[1], -theEyePos[2], 1);

        }

        void makePerspective(Number fovy, Number aspect, Number zNear, Number zFar) {
            if (fovy == 0 || aspect == 0) {
                throw asl::Exception("Matrix4::makePerspective: fovy, aspect must not be 0.", PLUS_FILE_LINE);
            }
            if (zFar <= zNear) {
                throw asl::Exception("Matrix4::makePerspective: zFar must be greater than zNear.", PLUS_FILE_LINE);
            }
            Number f = Number(cos(fovy/2.0) / sin(fovy/2.0));
            assign(f / aspect, 0, 0, 0,
                    0, f, 0, 0,
                    0, 0, (zFar + zNear) / (zNear - zFar), 2*zFar*zNear / (zNear - zFar),
                    0, 0, -1, 0);

        }

        void rotateX(Number cosAngle, Number sinAngle){
            if (_myType == IDENTITY) {
               base::makeXRotating(cosAngle, sinAngle);
                _myType = X_ROTATING;
                return;
            }
            base::rotateX(cosAngle, sinAngle);
            if (_myType != X_ROTATING) {
                if (_myType <= ROTATING) {
                    _myType = ROTATING;
                } else if (_myType < LINEAR) {
                    _myType = LINEAR;
                } else if (_myType == TRANSLATING) {
                    _myType = AFFINE;
                }
            }
        }
		void rotateX(Angle a){
            if (a != Angle(0)) {
                rotateX(static_cast<Number>(cos(a)),static_cast<Number>(sin(a)));
            }
		}
        void rotateY(Number cosAngle, Number sinAngle){
            if (_myType == IDENTITY) {
                base::makeYRotating(cosAngle, sinAngle);
                _myType = Y_ROTATING;
                return;
            }
            base::rotateY(cosAngle, sinAngle);
            if (_myType != Y_ROTATING) {
                if (_myType <= ROTATING) {
                    _myType = ROTATING;
                } else if (_myType < LINEAR) {
                    _myType = LINEAR;
                } else if (_myType == TRANSLATING) {
                    _myType = AFFINE;
                }
            }
        }
        void rotateY(Angle a){
            if (a != Angle(0)) {
                rotateY(static_cast<Number>(cos(a)),static_cast<Number>(sin(a)));
            }
        }
        void rotateZ(Number cosAngle, Number sinAngle){
            if (_myType == IDENTITY) {
                base::makeZRotating(cosAngle, sinAngle);
                _myType = Z_ROTATING;
                return;
            }
            base::rotateZ(cosAngle, sinAngle);
            if (_myType != Z_ROTATING) {
                if (_myType <= ROTATING) {
                    _myType = ROTATING;
                } else if (_myType < LINEAR) {
                    _myType = LINEAR;
                } else if (_myType == TRANSLATING) {
                    _myType = AFFINE;
                }
            }
        }
        void rotateZ(Angle a){
            if (a != Angle(0)) {
                rotateZ(static_cast<Number>(cos(a)),static_cast<Number>(sin(a)));
            }
        }
        void rotate(const Vector3<Number> & axis, Angle a) {
            if (a == Number(0)) {
                return;
            }
            if (_myType == IDENTITY) {
                base::makeRotating(axis,a);
                _myType = ROTATING;
                return;
            }
            if (axis == Vector3<Number>(1,0,0)) {
                rotateX(a);
                return;
            }
            if (axis == Vector3<Number>(0,1,0)) {
                rotateY(a);
                return;
            }
            if (axis == Vector3<Number>(0,0,1)) {
                rotateZ(a);
                return;
            }
            base::rotate(axis,a);
            if (_myType <= ROTATING) {
                _myType = ROTATING;
            } else if (_myType < LINEAR) {
                _myType = LINEAR;
            } else if (_myType == TRANSLATING) {
                _myType = AFFINE;
            }
        }

        void makeXYZRotating(const Vector3<Number> & theRotation) {
            base::makeXYZRotating(theRotation);
            _myType = ROTATING;
        }

        void makeZYXRotating(const Vector3<Number> & theRotation) {
            base::makeZYXRotating(theRotation);
            _myType = ROTATING;
        }

        void rotateXYZ(const Vector3<Number> & theEulerVector) {
            rotateX(theEulerVector[0]);
            rotateY(theEulerVector[1]);
            rotateZ(theEulerVector[2]);
        }

        void rotate(const Quaternion<Number> & theRotation) {
            Matrix4<Number> myRotation(theRotation);
            this->postMultiply(myRotation);
        }

        bool getRotation(Vector3<Number> & axis, Angle & a) const {
            return base::getRotation(axis,a);
        }

        enum RotationOrder {
            ROTATION_ORDER_XYZ,
            ROTATION_ORDER_XZY,
            ROTATION_ORDER_YXZ,
            ROTATION_ORDER_YZX,
            ROTATION_ORDER_ZXY,
            ROTATION_ORDER_ZYX
        };

        // BEWARE: getRotation methods only work for non-scaled matrices
        // any other matrix type use matrix::decompose instead!!
        // From 3D-Game Engine Design, page 19
        void getRotation(Vector3<Number> & theRotation, RotationOrder theOrder = ROTATION_ORDER_XYZ) const {
            Angle myX = 0;
            Angle myY = 0;
            Angle myZ = 0;

            switch (theOrder) {
                case ROTATION_ORDER_XYZ:
                    myY = asin(this->val[0][2]);
                    if (myY < PI_2) {
                        if (myY > - PI_2) {
                            myX = atan2(-this->val[1][2], this->val[2][2]);
                            myZ = atan2(-this->val[0][1], this->val[0][0]);
                        } else {
                            myX = - atan2(this->val[1][0], this->val[1][1]);
                            myZ = 0;
                        }
                    } else {
                        myX = atan2(this->val[1][0], this->val[1][1]);
                        myZ = 0;
                    }
                    break;
                case ROTATION_ORDER_XZY:
                    myZ = asin(-this->val[0][1]);
                    if (myZ < PI_2) {
                        if (myZ > - PI_2) {
                            myX = atan2(this->val[2][1], this->val[1][1]);
                            myY = atan2(this->val[0][2], this->val[0][0]);
                        } else {
                            myX = - atan2(-this->val[2][0], this->val[2][2]);
                            myY = 0;
                        }
                    } else {
                        myX = atan2(-this->val[2][0], this->val[2][2]);
                        myY = 0;
                    }
                    break;
                case ROTATION_ORDER_YXZ:
                    myX = asin(-this->val[1][2]);
                    if (myX < PI_2) {
                        if (myX > - PI_2) {
                            myY = atan2(this->val[0][2], this->val[2][2]);
                            myZ = atan2(this->val[1][0], this->val[1][1]);
                        } else {
                            myY = - atan2(-this->val[0][1], this->val[0][0]);
                            myZ = 0;
                        }
                    } else {
                        myY = atan2(-this->val[0][1], this->val[0][0]);
                        myZ = 0;
                    }
                    break;
                case ROTATION_ORDER_YZX:
                    myZ = asin(this->val[1][0]);
                    if (myZ < PI_2) {
                        if (myZ > - PI_2) {
                            myY = atan2(-this->val[2][0], this->val[0][0]);
                            myX = atan2(-this->val[1][2], this->val[1][1]);
                        } else {
                            myY = - atan2(this->val[2][1], this->val[2][2]);
                            myX = 0;
                        }
                    } else {
                        myY = atan2(this->val[2][1], this->val[2][2]);
                        myX = 0;
                    }
                    break;
                case ROTATION_ORDER_ZXY:
                    myX = asin(this->val[2][1]);
                    if (myX < PI_2) {
                        if (myX > - PI_2) {
                            myZ = atan2(-this->val[0][1], this->val[1][1]);
                            myY = atan2(-this->val[2][0], this->val[2][2]);
                        } else {
                            myZ = - atan2(this->val[0][2], this->val[0][0]);
                            myY = 0;
                        }
                    } else {
                        myZ = atan2(this->val[0][2], this->val[0][0]);
                        myY = 0;
                    }
                    break;
                case ROTATION_ORDER_ZYX:
                    myY = asin(-this->val[2][0]);
                    if (myY < PI_2) {
                        if (myY > - PI_2) {
                            myZ = atan2(this->val[1][0], this->val[0][0]);
                            myX = atan2(this->val[2][1], this->val[2][2]);
                        } else {
                            myZ = - atan2(-this->val[0][1], this->val[0][2]);
                            myX = 0;
                        }
                    } else {
                        myZ = atan2(-this->val[0][1], this->val[0][2]);
                        myX = 0;
                    }
                    break;
            }

            // convert from counter-clockwise to clockwise angles
            theRotation[0] = - myX;
            theRotation[1] = - myY;
            theRotation[2] = - myZ;
        }

        // BEWARE: getRotation methods only work for non-scaling matrices
        // any other matrix type use matrix::decompose instead!!
        void getRotation(Quaternion<Number> & theOrientation) const {
            float trace = static_cast<float>( this->val[0][0] + this->val[1][1] + this->val[2][2] + 1.0f );
            Number & qw = theOrientation[3];
            Number & qx = theOrientation[0];
            Number & qy = theOrientation[1];
            Number & qz = theOrientation[2];

            if( trace > 0 && !almostEqual(trace,0) ) {
                float s = sqrtf(trace) * 2;
                qw = 0.25f * s;
                qx = ( this->val[2][1] - this->val[1][2] ) / s;
                qy = ( this->val[0][2] - this->val[2][0] ) / s;
                qz = ( this->val[1][0] - this->val[0][1] ) / s;
            } else {
                if ( this->val[0][0] > this->val[1][1] && this->val[0][0] > this->val[2][2] ) {
                    float s = 2.0f * sqrtf( 1.0f + static_cast<float>( this->val[0][0] - this->val[1][1] - this->val[2][2] ) );
                    qx = 0.25f * s;
                    qy = (this->val[0][1] + this->val[1][0] ) / s;
                    qz = (this->val[0][2] + this->val[2][0] ) / s;
                    qw = (this->val[2][1] - this->val[1][2] ) / s;

                } else if (this->val[1][1] > this->val[2][2]) {
                    float s = 2.0f * sqrtf( 1.0f + static_cast<float>( this->val[1][1] - this->val[0][0] - this->val[2][2] ) );
                    qx = (this->val[0][1] + this->val[1][0] ) / s;
                    qy = 0.25f * s;
                    qz = (this->val[1][2] + this->val[2][1] ) / s;
                    qw = (this->val[0][2] - this->val[2][0] ) / s;
                } else {
                    float s = 2.0f * sqrtf( 1.0f + static_cast<float>( this->val[2][2] - this->val[0][0] - this->val[1][1] ) );
                    qx = (this->val[0][2] + this->val[2][0] ) / s;
                    qy = (this->val[1][2] + this->val[2][1] ) / s;
                    qz = 0.25f * s;
                    qw = (this->val[1][0] - this->val[0][1] ) / s;
                }
            }
            theOrientation.normalize();
        }


        void scaleDispatch(const Vector3<Number> & s) {
            if (_myType == IDENTITY) {
                base::makeScaling(s);
                _myType = SCALING;
                return;
            }
            if (_myType == SCALING) {
                this->val[0][0] *= s[0];
                this->val[1][1] *= s[1];
                this->val[2][2] *= s[2];
                _myType = SCALING;
                return;
            }
            base::scale(s);
            if (_myType < LINEAR) {
                _myType = LINEAR;
            } else if(_myType == TRANSLATING) {
                _myType = AFFINE;
            }
        }
        void scale(const Vector3<Number> & s) {
            if (s[0] != Number(1) || s[1] != Number(1) || s[2] != Number(1)) {
                scaleDispatch(s);
            }
        }
       void translateDispatch(const Vector3<Number> & t) {
            if (_myType == IDENTITY) {
                base::makeTranslating(t);
                _myType = TRANSLATING;
                return;
            }
            if (_myType == TRANSLATING) {
                this->val[3][0] += t[0];
                this->val[3][1] += t[1];
                this->val[3][2] += t[2];
                return;
            }
            base::translate(t);
            if (_myType!=TRANSLATING) {
                if (_myType<AFFINE) _myType = AFFINE;
            }
        }
        void translate(const Vector3<Number> & t) {
            if (t[0] != Number(0) || t[1] != Number(0) || t[2] != Number(0)) {
                translateDispatch(t);
            }
        }
        void transpose() {
            if (_myType == IDENTITY || _myType == SCALING) {
                return;
            }
            if (_myType <= LINEAR) {
                base::transposeLinear();
                return;
            }
            if (_myType == TRANSLATING) {
                base::transposeTranslating();
                _myType = UNKNOWN;
                return;
            }
            base::transpose();
            _myType = UNKNOWN;
        }

        // x_rotating:
        // 1    0      0    0
        // 0  cos(a) sin(a) 0
        // 0 -sin(a) cos(a) 0
        // 0    0      0    1
        //
        // y_rotating:
        // cos(a) 0 -sin(a) 0
        //   0    1    0    0
        // sin(a) 0  cos(a) 0
        //   0    0    0    1
        //
        // z_rotating:
        //  cos(a) sin(a) 0 0
        // -sin(a) cos(a) 0 0
        //    0      0    1 0
        //    0      0    0 1
        bool invert() {
            switch (_myType) {
                case IDENTITY:
                    return true;
                case X_ROTATING:
                    this->val[1][2] = -this->val[1][2];
                    this->val[2][1] = -this->val[2][1];
                    return true;
                case Y_ROTATING:
                    this->val[0][2] = -this->val[0][2];
                    this->val[2][0] = -this->val[2][0];
                    return true;
                case Z_ROTATING:
                    this->val[0][1] = -this->val[0][1];
                    this->val[1][0] = -this->val[1][0];
                    return true;
                case ROTATING:
                    base::transpose();
                    return true;
                case SCALING:
                    if (this->val[0][0] && this->val[1][1] && this->val[2][2]) {
                        this->val[0][0] = 1 / this->val[0][0];
                        this->val[1][1] = 1 / this->val[1][1];
                        this->val[2][2] = 1 / this->val[2][2];
                        return true;
                    } else {
                        return false;
                    }
                case ORTHOGONAL:
                    return Base::invertOrtho();
                case LINEAR:
                    return Base::invertLinear();
                case TRANSLATING:
                    this->val[3][0] *= -1;
                    this->val[3][1] *= -1;
                    this->val[3][2] *= -1;
                    return true;
                case AFFINE:
                    return Base::invertAffine();
                default:
                    return Base::invertCramer();
            }
        }
#ifdef ORIG
        void postMultiply(const Matrix4<Number> & b) {
            if (b.getType() == IDENTITY) {
                return;
            }
            if (_myType == IDENTITY) {
                assign(b);
                return;
            }
            if ((_myType <= LINEAR) && (b.getType() == TRANSLATING)) {
                multiplyLinearTranslating(*this, b, *this);
                _myType = AFFINE;
                return;
            }
            if ((_myType == TRANSLATING) && (b.getType() <= LINEAR)) {
                multiplyTranslatingLinear(*this, b, *this);
                _myType = AFFINE;
                return;
            }
            if (b.getType() == TRANSLATING) {
                translateDispatch(b.getTranslation());
                return;
            }
            if (b.getType() == SCALING) {
                scaleDispatch(b.getScaling());
                return;
            }

            if (b.getType() == X_ROTATING) {
                rotateX(b[1][1], b[1][2]);
                return;
            }
            if (b.getType() == Y_ROTATING) {
                rotateY(b[0][0], b[2][0]);
                return;
            }
            if (b.getType() == Z_ROTATING) {
                rotateZ(b[0][0], b[0][1]);
                return;
            }

            if ((_myType <= LINEAR) && (b.getType() <= LINEAR)) {
                Matrix4<Number> a;
                a[0][0] = this->val[0][0];
                a[0][1] = this->val[0][1];
                a[0][2] = this->val[0][2];
                a[1][0] = this->val[1][0];
                a[1][1] = this->val[1][1];
                a[1][2] = this->val[1][2];
                a[2][0] = this->val[2][0];
                a[2][1] = this->val[2][1];
                a[2][2] = this->val[2][2];

                multiplyLinear(a,b,*this);

                if (b.getType() == ROTATING && _myType <= ROTATING) {
                    _myType = ROTATING;
                } else {
                    _myType = LINEAR;
                }
                return;
            }

            if ((_myType <= AFFINE) && (b.getType() <= AFFINE)) {
                Matrix4<Number> a;
                a[0][0] = this->val[0][0];
                a[0][1] = this->val[0][1];
                a[0][2] = this->val[0][2];
                a[1][0] = this->val[1][0];
                a[1][1] = this->val[1][1];
                a[1][2] = this->val[1][2];
                a[2][0] = this->val[2][0];
                a[2][1] = this->val[2][1];
                a[2][2] = this->val[2][2];
                a[3][0] = this->val[3][0];
                a[3][1] = this->val[3][1];
                a[3][2] = this->val[3][2];
                multiplyAffine(a, b, *this);

                if (b.getType() == ROTATING && _myType <= ROTATING) {
                    _myType = ROTATING;
                } else {
                    _myType = AFFINE;
                }
                return;
            }

            // (Unknown or Affine) * Linear
            if (b.getType() == LINEAR) {
                Matrix4<Number> a;
                a = *this;
                multiplyUnkownLinear(a, b, *this);

                // Type does not change
                return;
            }

            Matrix4<Number> a = *this;
            multiplyFull(a,b,*this);
            _myType = UNKNOWN;
        }
#else
          void postMultiply(const Matrix4<Number> & b) {
              switch (b.getType()) {
                case IDENTITY:
                    return;
                case TRANSLATING:
                    translateDispatch(b.getTranslation());
                    return;
                case SCALING:
                    scaleDispatch(b.getScaling());
                    return;
                case X_ROTATING:
                    rotateX(b[1][1], b[1][2]);
                    return;
                case Y_ROTATING:
                    rotateY(b[0][0], b[2][0]);
                    return;
                case Z_ROTATING:
                    rotateZ(b[0][0], b[0][1]);
                    return;
                case UNKNOWN:
                    if (_myType != IDENTITY) goto unknown;
                    assign(b);
                    return;
                default: // ROTATING, ORTHOGONAL, LINEAR, AFFINE
                    break;
            }
            switch(_myType) {
                case IDENTITY:
                    assign(b);
                    return;
                case TRANSLATING:
                    if (b.getType() <= LINEAR) {
                        multiplyTranslatingLinear(*this, b, *this);
                        _myType = AFFINE;
                        return;
                    }
                    break;
                case UNKNOWN:
                    goto unknown;
                default: // *_ROTATING, ROTATING, SCALING, ORTHOGONAL, LINEAR, AFFINE
                    break;
            }
            if ((_myType <= LINEAR) && (b.getType() == TRANSLATING)) {
                multiplyLinearTranslating(*this, b, *this);
                _myType = AFFINE;
                return;
            }

            if ((_myType <= LINEAR) && (b.getType() <= LINEAR)) {
                Matrix4<Number> a;
                a[0][0] = this->val[0][0];
                a[0][1] = this->val[0][1];
                a[0][2] = this->val[0][2];
                a[1][0] = this->val[1][0];
                a[1][1] = this->val[1][1];
                a[1][2] = this->val[1][2];
                a[2][0] = this->val[2][0];
                a[2][1] = this->val[2][1];
                a[2][2] = this->val[2][2];

                multiplyLinear(a,b,*this);

                if (b.getType() == ROTATING && _myType <= ROTATING) {
                    _myType = ROTATING;
                } else {
                    _myType = LINEAR;
                }
                return;
            }

            if ((_myType <= AFFINE) && (b.getType() <= AFFINE)) {
                Matrix4<Number> a;
                a[0][0] = this->val[0][0];
                a[0][1] = this->val[0][1];
                a[0][2] = this->val[0][2];
                a[1][0] = this->val[1][0];
                a[1][1] = this->val[1][1];
                a[1][2] = this->val[1][2];
                a[2][0] = this->val[2][0];
                a[2][1] = this->val[2][1];
                a[2][2] = this->val[2][2];
                a[3][0] = this->val[3][0];
                a[3][1] = this->val[3][1];
                a[3][2] = this->val[3][2];
                multiplyAffine(a, b, *this);

                if (b.getType() == ROTATING && _myType <= ROTATING) {
                    _myType = ROTATING;
                } else {
                    _myType = AFFINE;
                }
                return;
            }
unknown:
            // (Unknown or Affine) * Linear
            if (b.getType() == LINEAR) {
                Matrix4<Number> a;
                a = *this;
                multiplyUnkownLinear(a, b, *this);

                // Type does not change
                return;
            }

            Matrix4<Number> a = *this;
            multiplyFull(a,b,*this);
            _myType = UNKNOWN;
        }
#endif

        // Decomposes affine matrices into a transformation sequence:
        // M = Scale * ShearXY * ShearXZ * ShearYZ * RotateX * RotateY * RotateZ * Translate
        // Be aware: Combinations of rotation and scale (and possibly others) cannot be decomposed
        //           to the original values, but to equivalent ones. (e.g. scale [-1,0,0] might return
        //           rotation [0,PI_2,0] or [0,0,PI_2])
        // From gems II, page 320

        bool decompose(Vector3<Number> & theScale,
                       Vector3<Number> & theShear,
                       Vector3<Number> & theOrientation,
                       Vector3<Number> & thePosition,
                       RotationOrder theOrder = ROTATION_ORDER_XYZ) const
        {
            return decomposeEuler(theScale, theShear, theOrientation, thePosition, theOrder);
        }

        bool decomposeEuler(Vector3<Number> & theScale,
                       Vector3<Number> & theShear,
                       Vector3<Number> & theOrientation,
                       Vector3<Number> & thePosition,
                       RotationOrder theOrder = ROTATION_ORDER_XYZ) const
        {
            if (_myType == UNKNOWN) {
                return false;
            }
            thePosition = getTranslation();

            // Get the upper left 3x3 matrix
            typedef Vector3<Vector3<Number> > Matrix3;
            Matrix3 myM3 = Matrix3(
                Vector3<Number>(this->val[0][0],this->val[0][1],this->val[0][2]),
                Vector3<Number>(this->val[1][0],this->val[1][1],this->val[1][2]),
                Vector3<Number>(this->val[2][0],this->val[2][1],this->val[2][2]));

            // Compute X scale factor and normalize first row.
            theScale[0] = length(myM3[0]);
            myM3[0] = normalized(myM3[0]);

            // Compute XY shear factor and make 2nd row orthogonal to 1st.
            theShear[0] = dot(myM3[0], myM3[1]);
            myM3[1][0] = myM3[1][0] - theShear[0] * myM3[0][0];
            myM3[1][1] = myM3[1][1] - theShear[0] * myM3[0][1];
            myM3[1][2] = myM3[1][2] - theShear[0] * myM3[0][2];

            // Now, compute Y scale and normalize 2nd row.
            theScale[1] = length(myM3[1]);
            myM3[1] = normalized(myM3[1]);

            if(theScale[1] != 0) {
                theShear[0] /= theScale[1];
            }

            // Compute XZ and YZ shears, orthogonalize 3rd row.
            theShear[1] = dot(myM3[0], myM3[2]);
            myM3[2][0] = myM3[2][0] - theShear[1] * myM3[0][0];
            myM3[2][1] = myM3[2][1] - theShear[1] * myM3[0][1];
            myM3[2][2] = myM3[2][2] - theShear[1] * myM3[0][2];

            theShear[2] = dot(myM3[1], myM3[2]);
            myM3[2][0] = myM3[2][0] - theShear[2] * myM3[1][0];
            myM3[2][1] = myM3[2][1] - theShear[2] * myM3[1][1];
            myM3[2][2] = myM3[2][2] - theShear[2] * myM3[1][2];

            // Next, get Z scale and normalize 3rd row.
            theScale[2] = length(myM3[2]);
            myM3[2] = normalized(myM3[2]);
            if(theScale[2] != 0) {
                theShear[1] /= theScale[2];
                theShear[2] /= theScale[2];
            }

            // At this point, the matrix is orthonormal.
            // Check for a coordinate system flip.  If the determinant
            // is -1, then negate the matrix and the scaling factors.
            if (dot(myM3[0], cross(myM3[1], myM3[2])) < 0) {
                for (unsigned i = 0; i < 3; ++i) {
                    myM3[i] *= -1;
                }
                theScale *= -1;
            }

            // Now, get the rotations (from "Eberly - 3D Game Engine Design", page 19)(vs)

            switch (theOrder) {
                case ROTATION_ORDER_XYZ:
                    if (myM3[0][2] < -1.0) {
                        myM3[0][2] = -1.0;
                    }
                    if (myM3[0][2] > 1.0) {
                        myM3[0][2] = 1.0;
                    }

                    theOrientation[1] = asin(myM3[0][2]);
                    if (theOrientation[1] < PI_2) {
                        if (theOrientation[1] > - PI_2) {
                            theOrientation[0] = atan2(-myM3[1][2], myM3[2][2]);
                            theOrientation[2] = atan2(-myM3[0][1], myM3[0][0]);
                        } else {
                            theOrientation[0] = - atan2(myM3[1][0], myM3[1][1]);
                            theOrientation[2] = 0;
                        }
                    } else {
                        theOrientation[0] = atan2(myM3[1][0], myM3[1][1]);
                        theOrientation[2] = 0;
                    }

                    break;

                case ROTATION_ORDER_ZYX:
                    if (myM3[2][0] < -1.0) {
                        myM3[2][0] = -1.0;
                    }
                    if (myM3[2][0] > 1.0) {
                        myM3[2][0] = 1.0;
                    }

                    theOrientation[1] = asin(-myM3[2][0]);
                    if (theOrientation[1] < PI_2) {
                        if (theOrientation[1] > - PI_2) {
                            theOrientation[0] = atan2(myM3[2][1], myM3[2][2]);
                            theOrientation[2] = atan2(myM3[1][0], myM3[0][0]);
                        } else {
                            // not a unique solution
                            theOrientation[0] = -atan2(-myM3[0][1], myM3[1][1]);
                            theOrientation[2] = 0;
                        }
                    } else {
                        // not a unique solution
                        theOrientation[0] = atan2(-myM3[0][1], myM3[1][1]);
                        theOrientation[2] = 0;
                    }

                    break;
                case ROTATION_ORDER_XZY:
                    if (myM3[0][1] < -1.0) {
                        myM3[0][1] = -1.0;
                    }
                    if (myM3[0][1] > 1.0) {
                        myM3[0][1] = 1.0;
                    }

                    theOrientation[2] = asin(-myM3[0][1]);
                    if (theOrientation[2] < PI_2) {
                        if (theOrientation[2] > - PI_2) {
                            theOrientation[0] = atan2(myM3[2][1], myM3[1][1]);
                            theOrientation[1] = atan2(myM3[0][2], myM3[0][0]);
                        } else {
                            theOrientation[0] = - atan2(-myM3[2][0], myM3[2][2]);
                            theOrientation[1] = 0;
                        }
                    } else {
                        theOrientation[0] = atan2(-myM3[2][0], myM3[2][2]);
                        theOrientation[1] = 0;
                    }
                    break;
                case ROTATION_ORDER_YXZ:
                    if (myM3[1][2] < -1.0) {
                        myM3[1][2] = -1.0;
                    }
                    if (myM3[1][2] > 1.0) {
                        myM3[1][2] = 1.0;
                    }
                    theOrientation[0] = asin(-myM3[1][2]);
                    if (theOrientation[0] < PI_2) {
                        if (theOrientation[0] > - PI_2) {
                            theOrientation[1] = atan2(myM3[0][2], myM3[2][2]);
                            theOrientation[2] = atan2(myM3[1][0], myM3[1][1]);
                        } else {
                            theOrientation[1] = - atan2(-myM3[0][1], myM3[0][0]);
                            theOrientation[2] = 0;
                        }
                    } else {
                        theOrientation[1] = atan2(-myM3[0][1], myM3[0][0]);
                        theOrientation[2] = 0;
                    }
                    break;
                case ROTATION_ORDER_YZX:
                    if (myM3[1][0] < -1.0) {
                        myM3[1][0] = -1.0;
                    }
                    if (myM3[1][0] > 1.0) {
                        myM3[1][0] = 1.0;
                    }
                    theOrientation[2] = asin(myM3[1][0]);
                    if (theOrientation[2] < PI_2) {
                        if (theOrientation[2] > - PI_2) {
                            theOrientation[1] = atan2(-myM3[2][0], myM3[0][0]);
                            theOrientation[0] = atan2(-myM3[1][2], myM3[1][1]);
                        } else {
                            theOrientation[1] = - atan2(myM3[2][1], myM3[2][2]);
                            theOrientation[0] = 0;
                        }
                    } else {
                        theOrientation[1] = atan2(myM3[2][1], myM3[2][2]);
                        theOrientation[0] = 0;
                    }
                    break;
                case ROTATION_ORDER_ZXY:
                    if (myM3[2][1] < -1.0) {
                        myM3[2][1] = -1.0;
                    }
                    if (myM3[2][1] > 1.0) {
                        myM3[2][1] = 1.0;
                    }
                    theOrientation[0] = asin(myM3[2][1]);
                    if (theOrientation[0] < PI_2) {
                        if (theOrientation[0] > - PI_2) {
                            theOrientation[2] = atan2(-myM3[0][1], myM3[1][1]);
                            theOrientation[1] = atan2(-myM3[2][0], myM3[2][2]);
                        } else {
                            theOrientation[2] = - atan2(myM3[0][2], myM3[0][0]);
                            theOrientation[1] = 0;
                        }
                    } else {
                        theOrientation[2] = atan2(myM3[0][2], myM3[0][0]);
                        theOrientation[1] = 0;
                    }
                    break;
            }
            theOrientation[0] = - theOrientation[0];
            theOrientation[1] = - theOrientation[1];
            theOrientation[2] = - theOrientation[2];

            return true;
        }

        // Decomposes affine matrices into a transformation sequence
        // shearing is not taken in account yet!
        bool decompose(Vector3<Number> & theScale,
                       Vector3<Number> & theShear,
                       Quaternion<Number> & theOrientation,
                       Vector3<Number> & thePosition) const
        {
            return decomposeQuaterion(theScale, theShear, theOrientation, thePosition);
        }

        bool decomposeQuaterion(Vector3<Number> & theScale,
                       Vector3<Number> & theShear,
                       Quaternion<Number> & theOrientation,
                       Vector3<Number> & thePosition) const
        {
            if (_myType == UNKNOWN) {
                return false;
            }

            thePosition = getTranslation();

            // Get the upper left 3x3 matrix
            typedef Vector3<Vector3<Number> > Matrix3;
            Matrix3 myM3 = Matrix3(
                Vector3<Number>(this->val[0][0],this->val[0][1],this->val[0][2]),
                Vector3<Number>(this->val[1][0],this->val[1][1],this->val[1][2]),
                Vector3<Number>(this->val[2][0],this->val[2][1],this->val[2][2]));

            // Compute scale factors
            theScale[0] = length(myM3[0]);
            theScale[1] = length(myM3[1]);
            theScale[2] = length(myM3[2]);


            if(theScale[0] != 0) {
                myM3[0][0] /= theScale[0];
                myM3[0][1] /= theScale[0];
                myM3[0][2] /= theScale[0];
            }
            if(theScale[1] != 0) {
                myM3[1][0] /= theScale[1];
                myM3[1][1] /= theScale[1];
                myM3[1][2] /= theScale[1];
            }
            if(theScale[2] != 0) {
                myM3[2][0] /= theScale[2];
                myM3[2][1] /= theScale[2];
                myM3[2][2] /= theScale[2];
            }

            // Check for a coordinate system flip.  If the determinant
            // is -1, then negate the matrix and the scaling factors.
            if (dot(myM3[0], cross(myM3[1], myM3[2])) < 0) {
                myM3[0] *= -1;
                myM3[1] *= -1;
                myM3[2] *= -1;
                theScale *= -1;
            }

            Matrix4<Number> myM4;
            myM4[0][0] = myM3[0][0];
            myM4[0][1] = myM3[0][1];
            myM4[0][2] = myM3[0][2];
            myM4[0][3] = 0;
            myM4[1][0] = myM3[1][0];
            myM4[1][1] = myM3[1][1];
            myM4[1][2] = myM3[1][2];
            myM4[1][3] = 0;
            myM4[2][0] = myM3[2][0];
            myM4[2][1] = myM3[2][1];
            myM4[2][2] = myM3[2][2];
            myM4[3][0] = 0;
            myM4[3][1] = 0;
            myM4[3][2] = 0;
            myM4[3][3] = 1;
            myM4.getRotation(theOrientation);
            return true;
        }

        void adjoint() {
            base::adjoint();
        }

    protected:
        Matrix4Base<Number> & getBase() {
            return *this;
        }
        const Matrix4Base<Number> & getBase() const {
            return *this;
        }

        MatrixType _myType;
        typedef Matrix4Base<Number> base;

        bool is_linear(MatrixType t) {
            return (t>=IDENTITY) && (t<=LINEAR);
        }
        bool is_linear() {
            return is_linear(_myType);
        }

        void shiftAngleToTwoPi(Number & theAngle) {
            if (theAngle > 0) {
                while (theAngle > 2 * PI) {
                    theAngle -= Number(2 * PI);
                }
            } else {
                while (theAngle < 0) {
                    theAngle += Number(2 * PI);
                }
            }
        }

        bool isCosSinAngleEqual(const Number & theCosinus, const Number & theSinus) {
            Number theCosinusAngle = Number(acos(double(theCosinus)));
            Number theSinusAngle   = Number(asin(double(theSinus)));

            if (theSinus < 0) {
                theCosinusAngle = Number(2 * PI) - theCosinusAngle;
            }

            if (theCosinus < 0) {
                theSinusAngle = Number(PI) - theSinusAngle;
            }

            shiftAngleToTwoPi(theCosinusAngle);
            shiftAngleToTwoPi(theSinusAngle);

            return almostEqual(theCosinusAngle, theSinusAngle);
        }

        MatrixType classifyMatrix() {
            // Check for unknown matrixes
            if (!almostEqual(getColumn(3), Vector4<Number>(0, 0, 0, 1))) {
                return UNKNOWN;
            }

            // Check for not linear
            if (!almostEqual(getRow(3), Vector4<Number>(0, 0, 0, 1))) {
                // Check for affine / translating
                if (almostEqual(this->val[0][0], 1) && almostEqual(this->val[0][1], 0) && almostEqual(this->val[0][2], 0) &&
                    almostEqual(this->val[1][0], 0) && almostEqual(this->val[1][1], 1) && almostEqual(this->val[1][2], 0) &&
                    almostEqual(this->val[2][0], 0) && almostEqual(this->val[2][1], 0) && almostEqual(this->val[2][2], 1))
                {
                    return TRANSLATING;
                } else {
                    return AFFINE;
                }
            }

            // Check for zeros left and right of the diagonal
            if (almostEqual(this->val[0][1], 0) && almostEqual(this->val[0][2], 0) &&
                almostEqual(this->val[1][0], 0) && almostEqual(this->val[1][2], 0) &&
                almostEqual(this->val[2][0], 0) && almostEqual(this->val[2][1], 0))
            {
                // Check for identity or scaling
                if (almostEqual(this->val[0][0], 1) && almostEqual(this->val[1][1], 1) &&
                    almostEqual(this->val[2][2], 1))
                {
                    return IDENTITY;
                } else {
                    return SCALING;
                }
            }

            // Check for x_rotating
            if (almostEqual(this->val[0][0], 1) && almostEqual(this->val[0][1], 0) && almostEqual(this->val[0][2], 0) &&
                almostEqual(this->val[1][0], 0) && almostEqual(this->val[1][1], this->val[2][2]) &&
                almostEqual(this->val[2][0], 0) && almostEqual(this->val[2][1], -this->val[1][2]) &&
                isCosSinAngleEqual(this->val[1][1], this->val[1][2]))
            {
                return X_ROTATING;
            }

            // Check for y_rotating
            if (almostEqual(this->val[0][0], this->val[2][2]) && almostEqual(this->val[0][1], 0) && almostEqual(this->val[0][2], -this->val[2][0]) &&
                almostEqual(this->val[1][0], 0) && almostEqual(this->val[1][1], 1) && almostEqual(this->val[1][2], 0) &&
                almostEqual(this->val[2][1], 0) &&
                isCosSinAngleEqual(this->val[0][0], this->val[2][0]))
            {
                return Y_ROTATING;
            }

            // Check for z_rotating
            if (almostEqual(this->val[0][0], this->val[1][1]) && almostEqual(this->val[0][1], -this->val[1][0]) && almostEqual(this->val[0][2], 0) &&
                almostEqual(this->val[1][2], 0) &&
                almostEqual(this->val[2][0], 0) && almostEqual(this->val[2][1], 0) && almostEqual(this->val[2][2], 1) &&
                isCosSinAngleEqual(this->val[0][0], this->val[0][1]))
            {
                return Z_ROTATING;
            }

            // Check for rotating
            // [CH/DS]: We found this algorithm ourselves and are not sure if it is perfect
            Point3<Number> myPoint(1, 0, 0);
            myPoint = myPoint * (*this);
            if (almostEqual(myPoint.getSquaredLength(), 1)) {
                return ROTATING;
            }

            return LINEAR;
        }

    };
/*
   template <class Number>
   void multiply(const Matrix4<Number> & a, const Matrix4<Number> & b, Matrix4<Number> & myResult) {
   }
  */


    typedef Matrix4<float> Matrix4f;
    typedef Matrix4<double> Matrix4d;

   template <class Number>
   void multiply(const Point3<Number> & p, const Matrix4<Number> & m, Point3<Number> & myResult) {
        if (m.getType() == IDENTITY) {
            //myResult = p;
            myResult[0] = p[0];
            myResult[1] = p[1];
            myResult[2] = p[2];
            return;
        }
        if (m.getType() == TRANSLATING) {
            myResult[0] = p[0] + m[3][0];
            myResult[1] = p[1] + m[3][1];
            myResult[2] = p[2] + m[3][2];
            return;
        }
        if (m.getType() == SCALING) {
            myResult[0] = p[0] * m[0][0];
            myResult[1] = p[1] * m[1][1];
            myResult[2] = p[2] * m[2][2];
            return;
        }
        if (m.getType() <= LINEAR) {
            myResult[0] = p[0] * m[0][0] + p[1] * m[1][0] + p[2] * m[2][0];
            myResult[1] = p[0] * m[0][1] + p[1] * m[1][1] + p[2] * m[2][1];
            myResult[2] = p[0] * m[0][2] + p[1] * m[1][2] + p[2] * m[2][2];
            return;
        }
        if (m.getType() == AFFINE) {
            myResult[0] = p[0] * m[0][0] + p[1] * m[1][0] + p[2] * m[2][0] + m[3][0];
            myResult[1] = p[0] * m[0][1] + p[1] * m[1][1] + p[2] * m[2][1] + m[3][1];
            myResult[2] = p[0] * m[0][2] + p[1] * m[1][2] + p[2] * m[2][2] + m[3][2];
            return;
        }

        myResult[0] = p[0] * m[0][0] + p[1] * m[1][0] + p[2] * m[2][0] + m[3][0];
        myResult[1] = p[0] * m[0][1] + p[1] * m[1][1] + p[2] * m[2][1] + m[3][1];
        myResult[2] = p[0] * m[0][2] + p[1] * m[1][2] + p[2] * m[2][2] + m[3][2];
        Number w    = p[0] * m[0][3] + p[1] * m[1][3] + p[2] * m[2][3] + m[3][3];
        if (w != 0) {
            myResult[0] /= w;
            myResult[1] /= w;
            myResult[2] /= w;
        }
   }

    template <class Number>
    Point3<Number> product(const Point3<Number> & p, const Matrix4<Number> & m) {
        Point3<Number> result;
        multiply(p, m, result);
        return result;
    }

    template <class Number>
    Point3<Number> fullproduct(const Point3<Number> & p, const Matrix4<Number> & m) {
        Point3<Number> result;
        result[0] = (p[0] * m[0][0]) + (p[1] * m[1][0]) + (p[2] * m[2][0]) + m[3][0];
        result[1] = (p[0] * m[0][1]) + (p[1] * m[1][1]) + (p[2] * m[2][1]) + m[3][1];
        result[2] = (p[0] * m[0][2]) + (p[1] * m[1][2]) + (p[2] * m[2][2]) + m[3][2];
        Number w  = (p[0] * m[0][3]) + (p[1] * m[1][3]) + (p[2] * m[2][3]) + m[3][3];
        if (w != 0) {
            result[0] /= w;
            result[1] /= w;
            result[2] /= w;
        }
        return result;
    }

    template <class Number>
    void multiply(const Vector4<Number> & v, const Matrix4<Number> & m, Vector4<Number> & myResult) {
        if (m.getType() == IDENTITY) {
            myResult = v;
            return;
        }
        if (m.getType() == TRANSLATING) {
            myResult[0] = v[0] + v[3] * m[3][0];
            myResult[1] = v[1] + v[3] * m[3][1];
            myResult[2] = v[2] + v[3] * m[3][2];
            myResult[3] = v[3];
            return;
        }
        if (m.getType() == SCALING) {
            myResult[0] = v[0] * m[0][0];
            myResult[1] = v[1] * m[1][1];
            myResult[2] = v[2] * m[2][2];
            myResult[3] = v[3];
            return;
        }
        if (m.getType() <= LINEAR) {
            myResult[0] = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0];
            myResult[1] = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1];
            myResult[2] = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2];
            myResult[3] = v[3];
            return;
        }
        if (m.getType() == AFFINE) {
            myResult[0] = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + v[3] * m[3][0];
            myResult[1] = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + v[3] * m[3][1];
            myResult[2] = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + v[3] * m[3][2];
            myResult[3] = v[3];
            return;
        }
        myResult[0] = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + v[3] * m[3][0];
        myResult[1] = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + v[3] * m[3][1];
        myResult[2] = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + v[3] * m[3][2];
        myResult[3] = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + v[3] * m[3][3];
    }

    template <class Number>
    Vector4<Number> product(const Vector4<Number> & v, const Matrix4<Number> & m) {
        Vector4<Number> result;
        multiply(v, m, result);
        return result;
    }
    template <class Number>
    Matrix4<Number> product(const Matrix4<Number> & m1, const Matrix4<Number> & m2) {
        Matrix4<Number> m(m1);
        m.postMultiply(m2);
        return m;
    }
    template <class Number>
    Vector4<Number> fullproduct(const Vector4<Number> & v, const Matrix4<Number> & m) {
        Vector4<Number> result;
        result[0] = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + v[3] * m[3][0];
        result[1] = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + v[3] * m[3][1];
        result[2] = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + v[3] * m[3][2];
        result[3] = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + v[3] * m[3][3];
        return result;
    }

    template <class Number>
    Vector3<Number> transformedVector(const Vector3<Number> & v, const Matrix4<Number> & m) {
        if (m.getType() == IDENTITY ||
            m.getType() == TRANSLATING ||
           (m.getType() == SCALING && m[0][0] == m[1][1] && m[1][1] == m[2][2]))
        {
            return v;
        }

        if (m.getType() <= ROTATING) {
            return asVector(asPoint(v) * m);
        }

        Matrix4<Number> myAdjointTransposed;

        myAdjointTransposed[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
        myAdjointTransposed[0][1] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
        myAdjointTransposed[0][2] = m[1][0] * m[2][1] - m[1][1] * m[2][0];
        myAdjointTransposed[0][3] = 0;
        myAdjointTransposed[1][0] = m[2][1] * m[0][2] - m[2][2] * m[0][1];
        myAdjointTransposed[1][1] = m[2][2] * m[0][0] - m[2][0] * m[0][2];
        myAdjointTransposed[1][2] = m[2][0] * m[0][1] - m[2][1] * m[0][0];
        myAdjointTransposed[1][3] = 0;
        myAdjointTransposed[2][0] = m[0][1] * m[1][2] - m[0][2] * m[1][1];
        myAdjointTransposed[2][1] = m[0][2] * m[1][0] - m[0][0] * m[1][2];
        myAdjointTransposed[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];
        myAdjointTransposed[2][3] = 0;
        myAdjointTransposed[3][0] = 0;
        myAdjointTransposed[3][1] = 0;
        myAdjointTransposed[3][2] = 0;
        myAdjointTransposed[3][3] = 1;

        myAdjointTransposed.setType(LINEAR);

        return asVector(asPoint(v) * myAdjointTransposed);
    }

    template <class Number>
    Vector3<Number> transformedNormal(const Vector3<Number> & v, const Matrix4<Number> & m) {
        return normalized( transformedVector(v, m));
    }

    template<class Number>
    Vector4<Number> operator*(const Vector4<Number> & v, const Matrix4<Number> & m) {
        return product(v, m);
    }

    template<class Number>
    Point3<Number> operator*(const Point3<Number> & p, const Matrix4<Number> & m) {
        return product(p, m);
    }
    template <class T>
    bool equal(const asl::Matrix4<T> & M, const asl::Matrix4<T> & N)
    {
        return equal(M.getBase(), N.getBase());
    }
    template <class T>
    bool operator==(const asl::Matrix4<T> & M, const asl::Matrix4<T> & N) {
        return equal(M, N);
    }
    template <class T>
    bool unequal(const asl::Matrix4<T> & M, const asl::Matrix4<T> & N)
    {
        return unequal(M.getBase(), N.getBase());
    }
    template <class T>
    bool operator!=(const asl::Matrix4<T> & M, const asl::Matrix4<T> & N) {
        return unequal(M, N);
    }


    template <class T>
    bool almostEqual(const asl::Matrix4<T> & myMatrix1,
                     const asl::Matrix4<T> & myMatrix2,
                     T theTolerance = 1e-5)
    {
        return almostEqual(myMatrix1.getBase(), myMatrix2.getBase(), theTolerance);
    }

    template <class Number>
    Matrix4<Number> inverse(const Matrix4<Number> & theMatrix) {
        Matrix4<Number> myResult = theMatrix;
        myResult.invert();
        return myResult;
    }
    template <class Number>
    Matrix4<Number> transposed(const Matrix4<Number> & theMatrix) {
        Matrix4<Number> myResult = theMatrix;
        myResult.transpose();
        return myResult;
    }
    template <class Number>
    Matrix4<Number> adjoint(const Matrix4<Number> & theMatrix) {
        Matrix4<Number> myResult = theMatrix;
        myResult.adjoint();
        return myResult;
    }


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

    template<class Number>
    std::ostream & printMatrix( std::ostream & os, const asl::Matrix4<Number> & theMatrix) {
        os << theMatrix.getType();
        if (os.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return printVector(os, theMatrix.getBase(),
                    0 != os.iword(FixedVectorStreamFormatter::ourOneElementPerLineFlagIndex),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourDelimiterIndex)) );
        } else {
            return printVector(os, theMatrix.getBase());
        }
    }

    template<class Number>
    std::istream & parseMatrix(std::istream & is, asl::Matrix4<Number> & theMatrix,
                               const char theStartToken = '[',
                               const char theEndToken = ']',
                               const char theDelimiter = ',')
    {
        /*
        std::string myMatrixType;
        char c;
        // 1. we need to parse things like a_translating, so check for first '[' for the end of matrix typesstrings
        // 2. Add a check, for incorrect strings (input should read like this 'matrix_type[matrix_base]), everything else
        //    throws an exception
        while (is >> c && c != theStartToken) {
            myMatrixType += c;
            if (c == theEndToken || c == theDelimiter) {
                is.setstate(std::ios::failbit);
                return is;
            }
        }
        is.unget();
        if (is && myMatrixType.size()) {
            theMatrix._myType = (typename asl::Matrix4<Number>::MatrixType)asl::getEnumFromString(myMatrixType, MatrixTypeStrings);
        } else {
            theMatrix._myType = asl::Matrix4<Number>::UNKNOWN;
        }
        */
        is >> theMatrix._myType;
        return parseVector(is, theMatrix.getBase(), theStartToken, theEndToken, theDelimiter);
    }


    template<class Number>
    inline std::ostream & operator<<(std::ostream & os, const asl::Matrix4<Number> & theMatrix) {
        return printMatrix(os, theMatrix);
    }

    template <class Number>
    std::istream & operator>>(std::istream & is, asl::Matrix4<Number> & theMatrix) {
        if (is.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return parseMatrix(is, theMatrix,
                    static_cast<char>(is.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                    static_cast<char>(is.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                    static_cast<char>(is.iword(FixedVectorStreamFormatter::ourDelimiterIndex)) );
        } else {
            return parseMatrix(is, theMatrix);
        }
    }

    /* @} */

}

#endif
