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
//    $RCSfile: Matrix4Base.h,v $
//
//    $Author: david $
//
//    $Revision: 1.30 $
//
//    Description: Base class for matrix - Use Matrix4 for your implentation
//
//=============================================================================

#ifndef _included_asl_Matrix4Base_
#define _included_asl_Matrix4Base_

#include <cstring>

#include "Vector234.h"
#include "Point234.h"

template <class T> class Matrix4BaseUnitTest;
template <class T> class Matrix4UnitTest;

namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    template<class Number>
    class Matrix4Base : public Vector4<Vector4<Number> > {
    typedef Vector4<Vector4<Number> > Base;
    friend class Matrix4BaseUnitTest<Number>;
    friend class Matrix4UnitTest<Number>;

    // The Matrix4Base class should not be used directly. Use a derived
    // class instead (e.g. Matrix4).
    protected:
        Matrix4Base() : Vector4<Vector4<Number> >() {
        }

        Matrix4Base(const Matrix4Base & a) : Vector4<Vector4<Number> >() {
            assign(a);
        }
        Matrix4Base(const Number * a) : Vector4<Vector4<Number> >() {
            assign(a);
        }

    public:
        typedef Number Angle;
        typedef Vector4<Number> Row;
        typedef Vector4<Number> Column;

        Row & operator[](int i) {
            return this->val[i];
        }

        const Row & operator[](int i) const {
            return this->val[i];
        }

        void assign(Number a00, Number a01, Number a02, Number a03,
                    Number a10, Number a11, Number a12, Number a13,
                    Number a20, Number a21, Number a22, Number a23,
                    Number a30, Number a31, Number a32, Number a33)
        {
            // a00 a01 a02 a03
            // a10 a11 a12 a13
            // a20 a21 a22 a23
            // a30 a31 a32 a33
            this->val[0][0] = a00;
            this->val[0][1] = a01;
            this->val[0][2] = a02;
            this->val[0][3] = a03;
            this->val[1][0] = a10;
            this->val[1][1] = a11;
            this->val[1][2] = a12;
            this->val[1][3] = a13;
            this->val[2][0] = a20;
            this->val[2][1] = a21;
            this->val[2][2] = a22;
            this->val[2][3] = a23;
            this->val[3][0] = a30;
            this->val[3][1] = a31;
            this->val[3][2] = a32;
            this->val[3][3] = a33;
        }

        void assign(const Matrix4Base<Number> & a) {
            memcpy(this->val, a.val, 16 * sizeof(Number));
        }
        void assign(const Number * a) {
            memcpy(this->val, a, 16 * sizeof(Number));
        }
        void assignRow(int n, const Row & theRow) {
            this->val[n][0] = theRow[0];
            this->val[n][1] = theRow[1];
            this->val[n][2] = theRow[2];
            this->val[n][3] = theRow[3];
        }
        void assignRow(int n, const FixedVector<4, Number> & theVector) {
            this->val[n][0] = theVector[0];
            this->val[n][1] = theVector[1];
            this->val[n][2] = theVector[2];
            this->val[n][3] = theVector[3];
        }
        void assignColumn(int n, const Column & theColumn) {
            this->val[0][n] = theColumn[0];
            this->val[1][n] = theColumn[1];
            this->val[2][n] = theColumn[2];
            this->val[3][n] = theColumn[3];
        }
        void assignColumn(int n, const FixedVector<4, Number> & theVector) {
            this->val[0][n] = theVector[0];
            this->val[2][n] = theVector[1];
            this->val[3][n] = theVector[2];
            this->val[4][n] = theVector[3];
        }

        Row getRow(int i) const {
            return Vector4<Number>(this->val[i][0],this->val[i][1],this->val[i][2],this->val[i][3]);
        }
        Column getColumn(int i) const {
            return Vector4<Number>(this->val[0][i],this->val[1][i],this->val[2][i],this->val[3][i]);
        }
        Vector4<Number> getDiagonal() {
            return Vector4<Number>(this->val[0][0],this->val[1][1],this->val[2][2],this->val[3][3]);
        }

        void transpose() {
            swap(this->val[0][1], this->val[1][0]);
            swap(this->val[0][2], this->val[2][0]);
            swap(this->val[0][3], this->val[3][0]);
            swap(this->val[1][2], this->val[2][1]);
            swap(this->val[1][3], this->val[3][1]);
            swap(this->val[2][3], this->val[3][2]);
        }

        // CH/DS: We think this should be called transposeLinear ...?
        // UZ: I think so too.
        // PM: I changed it
        void transposeLinear() {
            swap(this->val[0][1], this->val[1][0]);
            swap(this->val[0][2], this->val[2][0]);
            swap(this->val[1][2], this->val[2][1]);
        }
        void transposeTranslating() {
            swap(this->val[3][0], this->val[0][3]);
            swap(this->val[3][1], this->val[1][3]);
            swap(this->val[3][2], this->val[2][3]);
        }
        bool invert() {
            return invertCramer();
        }

        Number getDeterminant() {
            return   this->val[0][0] * this->val[1][1] * this->val[2][2] * this->val[3][3]
                   - this->val[0][0] * this->val[1][1] * this->val[2][3] * this->val[3][2]
                   + this->val[0][0] * this->val[1][2] * this->val[2][3] * this->val[3][1]
                   - this->val[0][0] * this->val[1][2] * this->val[2][1] * this->val[3][3]
                   + this->val[0][0] * this->val[1][3] * this->val[2][1] * this->val[3][2]
                   - this->val[0][0] * this->val[1][3] * this->val[2][2] * this->val[3][1]
                   - this->val[0][1] * this->val[1][2] * this->val[2][3] * this->val[3][0]
                   + this->val[0][1] * this->val[1][2] * this->val[2][0] * this->val[3][3]
                   - this->val[0][1] * this->val[1][3] * this->val[2][0] * this->val[3][2]
                   + this->val[0][1] * this->val[1][3] * this->val[2][2] * this->val[3][0]
                   - this->val[0][1] * this->val[1][0] * this->val[2][2] * this->val[3][3]
                   + this->val[0][1] * this->val[1][0] * this->val[2][3] * this->val[3][2]
                   + this->val[0][2] * this->val[1][3] * this->val[2][0] * this->val[3][1]
                   - this->val[0][2] * this->val[1][3] * this->val[2][1] * this->val[3][0]
                   + this->val[0][2] * this->val[1][0] * this->val[2][1] * this->val[3][3]
                   - this->val[0][2] * this->val[1][0] * this->val[2][3] * this->val[3][1]
                   + this->val[0][2] * this->val[1][1] * this->val[2][3] * this->val[3][0]
                   - this->val[0][2] * this->val[1][1] * this->val[2][0] * this->val[3][3]
                   - this->val[0][3] * this->val[1][0] * this->val[2][1] * this->val[3][2]
                   + this->val[0][3] * this->val[1][0] * this->val[2][2] * this->val[3][1]
                   - this->val[0][3] * this->val[1][1] * this->val[2][2] * this->val[3][0]
                   + this->val[0][3] * this->val[1][1] * this->val[2][0] * this->val[3][2]
                   - this->val[0][3] * this->val[1][2] * this->val[2][0] * this->val[3][1]
                   + this->val[0][3] * this->val[1][2] * this->val[2][1] * this->val[3][0];
        }

        // from Gems II, page 342
        bool invertAffine() {
            // Calc determinant of upper-left 3x3 submatrix
            Number myDeterminant = det3x3(this->val[0][0], this->val[0][1], this->val[0][2],
                                          this->val[1][0], this->val[1][1], this->val[1][2],
                                          this->val[2][0], this->val[2][1], this->val[2][2]);

            // Is the submatrix A singular?
            // not using almostEqual - its too paraniod in this case.
            // just prevent a division-by-zero error
            if ( myDeterminant == 0.0f) {
                // Matrix M has no inverse
                return false;
            }

            // Calculate inverse(A) = adj(A) / det(A)
            myDeterminant = Number(1.0) / myDeterminant;
            Matrix4Base<Number> out;

            out.val[0][0] =   ( this->val[1][1] * this->val[2][2] - this->val[1][2] * this->val[2][1] ) * myDeterminant;
            out.val[1][0] = - ( this->val[1][0] * this->val[2][2] - this->val[1][2] * this->val[2][0] ) * myDeterminant;
            out.val[2][0] =   ( this->val[1][0] * this->val[2][1] - this->val[1][1] * this->val[2][0] ) * myDeterminant;
            out.val[0][1] = - ( this->val[0][1] * this->val[2][2] - this->val[0][2] * this->val[2][1] ) * myDeterminant;
            out.val[1][1] =   ( this->val[0][0] * this->val[2][2] - this->val[0][2] * this->val[2][0] ) * myDeterminant;
            out.val[2][1] = - ( this->val[0][0] * this->val[2][1] - this->val[0][1] * this->val[2][0] ) * myDeterminant;
            out.val[0][2] =   ( this->val[0][1] * this->val[1][2] - this->val[0][2] * this->val[1][1] ) * myDeterminant;
            out.val[1][2] = - ( this->val[0][0] * this->val[1][2] - this->val[0][2] * this->val[1][0] ) * myDeterminant;
            out.val[2][2] =   ( this->val[0][0] * this->val[1][1] - this->val[0][1] * this->val[1][0] ) * myDeterminant;

            // Calculate - C * inverse(A)
            out.val[3][0] = - ( this->val[3][0] * out.val[0][0] + this->val[3][1] * out.val[1][0] +
                    this->val[3][2] * out.val[2][0] );
            out.val[3][1] = - ( this->val[3][0] * out.val[0][1] + this->val[3][1] * out.val[1][1] +
                    this->val[3][2] * out.val[2][1] );
            out.val[3][2] = - ( this->val[3][0] * out.val[0][2] + this->val[3][1] * out.val[1][2] +
                    this->val[3][2] * out.val[2][2] );

            // Fill in last column
            out.val[0][3] = out.val[1][3] = out.val[2][3] = 0.0;
            out.val[3][3] = 1.0;

            assign(out);
            return true;
        }

        bool invertCramer() {
            // calculate the 4x4 determinant. If the determinant is zero,
            // then the inverse matrix is not unique.
            Number myDeterminant = getDeterminant();

            // not using almostEqual - its too paraniod in this case.
            // just prevent a division-by-zero error
            if ( myDeterminant == 0.0f) {
                // Matrix has no inverse
                return false;
            }

            Matrix4Base<Number> out;
            myDeterminant = Number(1.0) / myDeterminant;

            // calculate the adjoint matrix
            getAdjoint(*this, out);

            // scale the adjoint matrix to get the inverse
            this->val[0][0] = out.val[0][0] * myDeterminant;
            this->val[0][1] = out.val[0][1] * myDeterminant;
            this->val[0][2] = out.val[0][2] * myDeterminant;
            this->val[0][3] = out.val[0][3] * myDeterminant;

            this->val[1][0] = out.val[1][0] * myDeterminant;
            this->val[1][1] = out.val[1][1] * myDeterminant;
            this->val[1][2] = out.val[1][2] * myDeterminant;
            this->val[1][3] = out.val[1][3] * myDeterminant;

            this->val[2][0] = out.val[2][0] * myDeterminant;
            this->val[2][1] = out.val[2][1] * myDeterminant;
            this->val[2][2] = out.val[2][2] * myDeterminant;
            this->val[2][3] = out.val[2][3] * myDeterminant;

            this->val[3][0] = out.val[3][0] * myDeterminant;
            this->val[3][1] = out.val[3][1] * myDeterminant;
            this->val[3][2] = out.val[3][2] * myDeterminant;
            this->val[3][3] = out.val[3][3] * myDeterminant;

            return true;
        }

        bool invertLinear() {
            // calc determinant of upper-left 3x3 submatrix
            Number myDeterminant = det3x3(this->val[0][0], this->val[0][1], this->val[0][2],
                                          this->val[1][0], this->val[1][1], this->val[1][2],
                                          this->val[2][0], this->val[2][1], this->val[2][2]);

            // Is the 3x3 submatrix singular?
            // not using almostEqual - its too paraniod in this case.
            // just prevent a division-by-zero error
            if ( myDeterminant == 0.0f) {
                // Matrix has no inverse
                return false;
            }

            myDeterminant = Number(1.0) / myDeterminant;
            Matrix4Base<Number> out;

            out.val[0][0] =   this->val[1][1] * this->val[2][2] - this->val[1][2] * this->val[2][1];
            out.val[1][0] = -(this->val[1][0] * this->val[2][2] - this->val[1][2] * this->val[2][0]);
            out.val[2][0] =   this->val[1][0] * this->val[2][1] - this->val[1][1] * this->val[2][0];
            out.val[0][1] = -(this->val[0][1] * this->val[2][2] - this->val[0][2] * this->val[2][1]);
            out.val[1][1] =   this->val[0][0] * this->val[2][2] - this->val[0][2] * this->val[2][0];
            out.val[2][1] = -(this->val[0][0] * this->val[2][1] - this->val[0][1] * this->val[2][0]);
            out.val[0][2] =   this->val[0][1] * this->val[1][2] - this->val[0][2] * this->val[1][1];
            out.val[1][2] = -(this->val[0][0] * this->val[1][2] - this->val[0][2] * this->val[1][0]);
            out.val[2][2] =   this->val[0][0] * this->val[1][1] - this->val[0][1] * this->val[1][0];

            // scale the adjoint matrix to get the inverse
            this->val[0][0] = out.val[0][0] * myDeterminant;
            this->val[0][1] = out.val[0][1] * myDeterminant;
            this->val[0][2] = out.val[0][2] * myDeterminant;

            this->val[1][0] = out.val[1][0] * myDeterminant;
            this->val[1][1] = out.val[1][1] * myDeterminant;
            this->val[1][2] = out.val[1][2] * myDeterminant;

            this->val[2][0] = out.val[2][0] * myDeterminant;
            this->val[2][1] = out.val[2][1] * myDeterminant;
            this->val[2][2] = out.val[2][2] * myDeterminant;

            return true;
        }

        bool invertOrtho() {
            transpose();
            return true;
        }

        bool invertTranslating() {
            this->val[3][0] = -this->val[3][0];
            this->val[3][1] = -this->val[3][1];
            this->val[3][2] = -this->val[3][2];
            return true;
        }

        Number norm() {
            Number result = 0;
            result += this->val[0][0] * this->val[0][0];
            result += this->val[0][1] * this->val[0][1];
            result += this->val[0][2] * this->val[0][2];
            result += this->val[0][3] * this->val[0][3];
            result += this->val[1][0] * this->val[1][0];
            result += this->val[1][1] * this->val[1][1];
            result += this->val[1][2] * this->val[1][2];
            result += this->val[1][3] * this->val[1][3];
            result += this->val[2][0] * this->val[2][0];
            result += this->val[2][1] * this->val[2][1];
            result += this->val[2][2] * this->val[2][2];
            result += this->val[2][3] * this->val[2][3];
            result += this->val[3][0] * this->val[3][0];
            result += this->val[3][1] * this->val[3][1];
            result += this->val[3][2] * this->val[3][2];
            result += this->val[3][3] * this->val[3][3];
            result = static_cast<Number>(sqrt(result));
            return result;
        }

        void makeIdentity() {
            this->val[0][0] = 1; this->val[0][1] = 0; this->val[0][2] = 0; this->val[0][3] = 0;
            this->val[1][0] = 0; this->val[1][1] = 1; this->val[1][2] = 0; this->val[1][3] = 0;
            this->val[2][0] = 0; this->val[2][1] = 0; this->val[2][2] = 1; this->val[2][3] = 0;
            this->val[3][0] = 0; this->val[3][1] = 0; this->val[3][2] = 0; this->val[3][3] = 1;
            // 1 0 0 0
            // 0 1 0 0
            // 0 0 1 0
            // 0 0 0 1
        }
        //
        // The following make_xxxxx - function assume the current Matrix is
        // already IDENTITY or same type as before assignment !
        //
        void makeXRotating(Number c, Number s) {
            this->val[1][1] = c;
            this->val[1][2] = s;
            this->val[2][1] = -s;
            this->val[2][2] = c;
            // 1    0      0    0
            // 0  cos(a) sin(a) 0
            // 0 -sin(a) cos(a) 0
            // 0    0      0    1
        }
        void makeXRotating(Angle a){
            makeXRotating(static_cast<Number>(cos(a)),static_cast<Number>(sin(a)));
        }
        void makeYRotating(Number c, Number s) {
            this->val[0][0] = c;
            this->val[0][2] = -s;
            this->val[2][0] = s;
            this->val[2][2] = c;
            // cos(a) 0 -sin(a) 0
            //   0    1    0    0
            // sin(a) 0  cos(a) 0
            //   0    0    0    1
        }
        void makeYRotating(Angle a){
            makeYRotating(static_cast<Number>(cos(a)),static_cast<Number>(sin(a)));
        }
        void makeZRotating(Number c, Number s) {
            this->val[0][0] = c;
            this->val[0][1] = s;
            this->val[1][0] = -s;
            this->val[1][1] = c;
            //  cos(a) sin(a) 0 0
            // -sin(a) cos(a) 0 0
            //    0      0    1 0
            //    0      0    0 1
        }
        void makeZRotating(Angle a){
            makeZRotating(static_cast<Number>(cos(a)),static_cast<Number>(sin(a)));
        }

        // GEMS Pg.466
        // The axis must be normalized, for this function to work.
        void makeRotating(const Vector3<Number> & axis, Angle a) {
            Number c = static_cast<Number>(cos(a));
            Number s = static_cast<Number>(sin(a));
            Number t = 1 - c;
            Number x = axis[0];
            Number y = axis[1];
            Number z = axis[2];

            Number tx = t*x;
            Number txz = tx*z;
            Number txy = tx*y;
            Number ty = t*y;
            Number tyz = ty*z;
            Number sx = s*x;
            Number sy = s*y;
            Number sz = s*z;

            this->val[0][0] = tx*x + c;
            this->val[0][1] = txy + sz;
            this->val[0][2] = txz - sy;
            this->val[0][3] = 0;

            this->val[1][0] = txy - sz;
            this->val[1][1] = ty*y + c;
            this->val[1][2] = tyz + sx;
            this->val[1][3] = 0;

            this->val[2][0] = txz + sy;
            this->val[2][1] = tyz - sx;
            this->val[2][2] = t*z*z + c;
            this->val[2][3] = 0;

            this->val[3][0] = 0;
            this->val[3][1] = 0;
            this->val[3][2] = 0;
            this->val[3][3] = 1;
            // tx^2+c  txy+sz txz-sy 0
            // txy-sz  ty^2+c tyz+sx 0
            // txz+sy  tyz-sx tz^2+c 0
            //   0        0      0   1
        }

        // From 3d engine desing, page 19
        // This is equithis->valent to calling:
        // myMatrix.makeZRotating(theVector[2]);
        // myMatrix.rotateY(theVector[1]);
        // myMatrix.rotateX(theVecotr[0]);
        void makeXYZRotating(const Vector3<Number> & theVector) {
            Number cx = static_cast<Number>(cos(theVector[0]));
            Number sx = static_cast<Number>(sin(theVector[0]));
            Number cy = static_cast<Number>(cos(theVector[1]));
            Number sy = static_cast<Number>(sin(theVector[1]));
            Number cz = static_cast<Number>(cos(theVector[2]));
            Number sz = static_cast<Number>(sin(theVector[2]));

            this->val[0][0] =  cy * cz;
            this->val[0][1] =  cz * sx * sy + cx * sz;
            this->val[0][2] = -cx * cz * sy + sx * sz;
            this->val[0][3] =  0;
            this->val[1][0] = -cy * sz;
            this->val[1][1] =  cx * cz - sx * sy * sz;
            this->val[1][2] =  cz * sx + cx * sy * sz;
            this->val[1][3] =  0;
            this->val[2][0] =  sy;
            this->val[2][1] = -cy * sx;
            this->val[2][2] =  cx * cy;
            this->val[2][3] =  0;
            this->val[3][0] =  0;
            this->val[3][1] =  0;
            this->val[3][2] =  0;
            this->val[3][3] =  1;
        }

        // From 3d engine desing, page 23
        // This is equithis->valent to calling:
        // myMatrix.makeXRotating(theVector[2]);
        // myMatrix.rotateY(theVector[1]);
        // myMatrix.rotateZ(theVecotr[0]);
        void makeZYXRotating(const Vector3<Number> & theVector) {
            Number cx = static_cast<Number>(cos(theVector[0]));
            Number sx = static_cast<Number>(sin(theVector[0]));
            Number cy = static_cast<Number>(cos(theVector[1]));
            Number sy = static_cast<Number>(sin(theVector[1]));
            Number cz = static_cast<Number>(cos(theVector[2]));
            Number sz = static_cast<Number>(sin(theVector[2]));

            this->val[0][0] =  cy * cz;
            this->val[0][1] =  cy * sz;
            this->val[0][2] = -sy;
            this->val[0][3] =  0;
            this->val[1][0] =  cz * sx * sy - cx * sz;
            this->val[1][1] =  cx * cz + sx * sy * sz;
            this->val[1][2] =  cy * sx;
            this->val[1][3] =  0;
            this->val[2][0] =  cx * cz * sy + sx * sz;
            this->val[2][1] = -cz * sx + cx * sy * sz;
            this->val[2][2] =  cx * cy;
            this->val[2][3] =  0;
            this->val[3][0] =  0;
            this->val[3][1] =  0;
            this->val[3][2] =  0;
            this->val[3][3] =  1;
        }

        Angle getXRotation() const {
            throw NotYetImplemented(JUST_FILE_LINE);
            //return 0.0;
        }
        Angle getYRotation() const {
            throw NotYetImplemented(JUST_FILE_LINE);
            //return 0.0;
        }
        Angle getZRotation() const {
            throw NotYetImplemented(JUST_FILE_LINE);
            //return 0.0;
        }
        // GEMS PG 466
        // find axis and angle of rotation from a given matrix
        bool getRotation(Vector3<Number> & axis, Angle & a) const {
            Number cos_a = (this->val[0][0] +  this->val[1][1] + this->val[2][2] - 1) / 2;
            a = static_cast<Number>(acos(cos_a));
            Number sin_a = static_cast<Number>(sin(a));
            if (sin_a!=0) {
                Number sin_a2 = sin_a * 2;
                axis[0] = (this->val[1][2] -  this->val[2][1]) / sin_a2;
                axis[1] = (this->val[2][0] -  this->val[0][2]) / sin_a2;
                axis[2] = (this->val[0][1] -  this->val[1][0]) / sin_a2;
                return true;
            }
            // when the angle is too small there is no axis
            return false;
        }
        void makeScaling(const Vector3<Number> & s) {
            this->val[0][0]=s[0];
            this->val[1][1]=s[1];
            this->val[2][2]=s[2];
            // sx 0  0  0
            // 0  sy 0  0
            // 0  0  sz 0
            // 0  0  0  1
        }
        Vector3<Number> getScaling() const {
            return Vector3<Number>(this->val[0][0],this->val[1][1],this->val[2][2]);
        }
        void makeTranslating(const Vector3<Number> & t) {
            this->val[3][0]=t[0];
            this->val[3][1]=t[1];
            this->val[3][2]=t[2];
            // 1  0  0  0
            // 0  1  0  0
            // 0  0  1  0
            // tx ty tz 1
        }
        Vector3<Number> getTranslation() const {
            return Vector3<Number>(this->val[3][0],this->val[3][1],this->val[3][2]);
        }
        void setTranslation(const Vector3<Number> & theTranslation) {
            this->val[3][0] = theTranslation[0];
            this->val[3][1] = theTranslation[1];
            this->val[3][2] = theTranslation[2];
        }
		Vector3<Number> getScale() const {
            return Vector3<Number>(this->val[0][0],this->val[1][1],this->val[2][2]);
		}        
		void setScale(const Vector3<Number> & theScale) {
		    this->val[0][0] = theScale[0];
		    this->val[1][1] = theScale[1];
		    this->val[2][2] = theScale[2];            
		}                
        void rotateX(Number cosAngle, Number sinAngle) {
            Number t = this->val[0][1];
            this->val[0][1]= t * cosAngle - this->val[0][2] * sinAngle;
            this->val[0][2]= t * sinAngle + this->val[0][2] * cosAngle;
            t = this->val[1][1];
            this->val[1][1]= t * cosAngle - this->val[1][2] * sinAngle;
            this->val[1][2]= t * sinAngle + this->val[1][2] * cosAngle;
            t = this->val[2][1];
            this->val[2][1]= t * cosAngle - this->val[2][2] * sinAngle;
            this->val[2][2]= t * sinAngle + this->val[2][2] * cosAngle;
            t = this->val[3][1];
            this->val[3][1]= t * cosAngle - this->val[3][2] * sinAngle;
            this->val[3][2]= t * sinAngle + this->val[3][2] * cosAngle;

        }
        void rotateX(Angle a) {
            rotateX(static_cast<Number>(cos(a)), static_cast<Number>(sin(a)));
        }
        void rotateY(Number cosAngle, Number sinAngle) {
            Number t = this->val[0][0];
            this->val[0][0]= t * cosAngle + this->val[0][2] * sinAngle;
            this->val[0][2]= this->val[0][2] * cosAngle - t * sinAngle;
            t = this->val[1][0];
            this->val[1][0]= t * cosAngle + this->val[1][2] * sinAngle;
            this->val[1][2]= this->val[1][2] * cosAngle - t * sinAngle;
            t = this->val[2][0];
            this->val[2][0]= t * cosAngle + this->val[2][2] * sinAngle;
            this->val[2][2]= this->val[2][2] * cosAngle - t * sinAngle;
            t = this->val[3][0];
            this->val[3][0]= t * cosAngle + this->val[3][2] * sinAngle;
            this->val[3][2]= this->val[3][2] * cosAngle - t * sinAngle;
        }
        void rotateY(Angle a) {
            rotateY(static_cast<Number>(cos(a)),static_cast<Number>(sin(a)));
        }
        void rotateZ(Number cosAngle, Number sinAngle){
            Number t = this->val[0][0];
            this->val[0][0]= t * cosAngle - this->val[0][1] * sinAngle;
            this->val[0][1]= t * sinAngle + this->val[0][1] * cosAngle;
            t = this->val[1][0];
            this->val[1][0]= t * cosAngle - this->val[1][1] * sinAngle;
            this->val[1][1]= t * sinAngle + this->val[1][1] * cosAngle;
            t = this->val[2][0];
            this->val[2][0]= t * cosAngle - this->val[2][1] * sinAngle;
            this->val[2][1]= t * sinAngle + this->val[2][1] * cosAngle;
            t = this->val[3][0];
            this->val[3][0]= t * cosAngle - this->val[3][1] * sinAngle;
            this->val[3][1]= t * sinAngle + this->val[3][1] * cosAngle;
        }
        void rotateZ(Angle a) {
            rotateZ(static_cast<Number>(cos(a)),static_cast<Number>(sin(a)));
        }
        void rotate(const Vector3<Number> & theAxis, Angle theAngle) {
            Matrix4Base<Number> a = *this;
            Matrix4Base<Number> b;
            b.makeRotating(theAxis, theAngle);
            multiplyUnkownLinear(a, b, *this);
        }
        void scale(const Vector3<Number> & s) {
            this->val[0][0] *= s[0];
            this->val[0][1] *= s[1];
            this->val[0][2] *= s[2];

            this->val[1][0] *= s[0];
            this->val[1][1] *= s[1];
            this->val[1][2] *= s[2];

            this->val[2][0] *= s[0];
            this->val[2][1] *= s[1];
            this->val[2][2] *= s[2];

            this->val[3][0] *= s[0];
            this->val[3][1] *= s[1];
            this->val[3][2] *= s[2];
        }
        void translate(const Vector3<Number> & t) {
            this->val[0][0] += this->val[0][3] * t[0];
            this->val[0][1] += this->val[0][3] * t[1];
            this->val[0][2] += this->val[0][3] * t[2];

            this->val[1][0] += this->val[1][3] * t[0];
            this->val[1][1] += this->val[1][3] * t[1];
            this->val[1][2] += this->val[1][3] * t[2];

            this->val[2][0] += this->val[2][3] * t[0];
            this->val[2][1] += this->val[2][3] * t[1];
            this->val[2][2] += this->val[2][3] * t[2];

            this->val[3][0] += this->val[3][3] * t[0];
            this->val[3][1] += this->val[3][3] * t[1];
            this->val[3][2] += this->val[3][3] * t[2];
        }
        void multiply(const Matrix4Base<Number> & b) {
            Matrix4Base<Number> a = *this;
            multiplyFull(a,b,*this);
        }
        void preMultiply(const Matrix4Base<Number> & a) {
            Matrix4Base<Number> b = *this;
            multiplyFull(a,b,*this);
        }
        static void multiplyFull(const Matrix4Base<Number> & a,const Matrix4Base<Number> & b,Matrix4Base<Number> & c) {
            c[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0];
            c[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1];
            c[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2];
            c[0][3] = a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3];

            c[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0];
            c[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1];
            c[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2];
            c[1][3] = a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3];

            c[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0];
            c[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1];
            c[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2];
            c[2][3] = a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3];

            c[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0];
            c[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1];
            c[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2];
            c[3][3] = a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3];
        }

        // GEMS Pg. 454
        // Attention: Do not call these functions directly.
        // c must be set to a in advance for this to work.
        // Use the Matrix4<T> class instead
        static void multiplyLinear(const Matrix4Base<Number> & a,const Matrix4Base<Number> & b,Matrix4Base<Number> & c) {
            c[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0];
            c[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1];
            c[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2];

            c[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0];
            c[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1];
            c[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2];

            c[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0];
            c[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1];
            c[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2];
            // result is linear
        }
        static void multiplyAffine(const Matrix4Base<Number> & a,const Matrix4Base<Number> & b,Matrix4Base<Number> & c) {
            c[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0];
            c[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1];
            c[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2];

            c[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0];
            c[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1];
            c[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2];

            c[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0];
            c[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1];
            c[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2];

            c[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0];
            c[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1];
            c[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2];

            c[3][0] += b[3][0];
            c[3][1] += b[3][1];
            c[3][2] += b[3][2];
            // result is affine
        }
        static void multiplyLinearTranslating(const Matrix4Base<Number> & a,const Matrix4Base<Number> & b,Matrix4Base<Number> & c) {
            c.assignRow(0, a[0]);
            c.assignRow(1, a[1]);
            c.assignRow(2, a[2]);
            c.assignRow(3, b[3]);
            // result is affine
        }
        static void multiplyTranslatingLinear(const Matrix4Base<Number> & a,const Matrix4Base<Number> & b,Matrix4Base<Number> & c) {
            c.assignRow(0, b[0]);
            c.assignRow(1, b[1]);
            c.assignRow(2, b[2]);
            Number c_3_0 = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0];
            Number c_3_1 = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1];
            Number c_3_2 = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2];
            c[3][0] = c_3_0;
            c[3][1] = c_3_1;
            c[3][2] = c_3_2;
            c[3][3] = 1;
            // result is affine
        }
        static void multiplyUnkownLinear(const Matrix4Base<Number> & a,const Matrix4Base<Number> & b,Matrix4Base<Number> & c) {
            c[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0];
            c[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1];
            c[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2];
            c[0][3] = a[0][3];

            c[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0];
            c[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1];
            c[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2];
            c[1][3] = a[1][3];

            c[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0];
            c[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1];
            c[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2];
            c[2][3] = a[2][3];

            c[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0];
            c[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1];
            c[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2];
            c[3][3] = a[3][3];
            // result is unknown
        }
        static void add(const Matrix4Base<Number> & a,const Matrix4Base<Number> & b,Matrix4Base<Number> & c) {
            c[0][0] = a[0][0] + b[0][0];
            c[0][1] = a[0][1] + b[0][1];
            c[0][2] = a[0][2] + b[0][2];
            c[0][3] = a[0][3] + b[0][3];

            c[1][0] = a[1][0] + b[1][0];
            c[1][1] = a[1][1] + b[1][1];
            c[1][2] = a[1][2] + b[1][2];
            c[1][3] = a[1][3] + b[1][3];

            c[2][0] = a[2][0] + b[2][0];
            c[2][1] = a[2][1] + b[2][1];
            c[2][2] = a[2][2] + b[2][2];
            c[2][3] = a[2][3] + b[2][3];

            c[3][0] = a[3][0] + b[3][0];
            c[3][1] = a[3][1] + b[3][1];
            c[3][2] = a[3][2] + b[3][2];
            c[3][3] = a[3][3] + b[3][3];
        }

        void adjoint() {
            Matrix4Base<Number> myTempM(*this);
            getAdjoint(myTempM, *this);
        }

        static void getAdjoint(const Matrix4Base<Number> & in, Matrix4Base<Number> & out) {
            Number a1, a2, a3, a4, b1, b2, b3, b4;
            Number c1, c2, c3, c4, d1, d2, d3, d4;

            // assign to individual variable names to aid  selecting correct this->values
                a1 = in.val[0][0]; b1 = in.val[0][1];
                c1 = in.val[0][2]; d1 = in.val[0][3];

                a2 = in.val[1][0]; b2 = in.val[1][1];
                c2 = in.val[1][2]; d2 = in.val[1][3];

                a3 = in.val[2][0]; b3 = in.val[2][1];
                c3 = in.val[2][2]; d3 = in.val[2][3];

                a4 = in.val[3][0]; b4 = in.val[3][1];
                c4 = in.val[3][2]; d4 = in.val[3][3];


            // row column labeling reversed since we transpose rows & columns
            out.val[0][0]  =   det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4);
            out.val[1][0]  = - det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4);
            out.val[2][0]  =   det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4);
            out.val[3][0]  = - det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);

            out.val[0][1]  = - det3x3( b1, b3, b4, c1, c3, c4, d1, d3, d4);
            out.val[1][1]  =   det3x3( a1, a3, a4, c1, c3, c4, d1, d3, d4);
            out.val[2][1]  = - det3x3( a1, a3, a4, b1, b3, b4, d1, d3, d4);
            out.val[3][1]  =   det3x3( a1, a3, a4, b1, b3, b4, c1, c3, c4);

            out.val[0][2]  =   det3x3( b1, b2, b4, c1, c2, c4, d1, d2, d4);
            out.val[1][2]  = - det3x3( a1, a2, a4, c1, c2, c4, d1, d2, d4);
            out.val[2][2]  =   det3x3( a1, a2, a4, b1, b2, b4, d1, d2, d4);
            out.val[3][2]  = - det3x3( a1, a2, a4, b1, b2, b4, c1, c2, c4);

            out.val[0][3]  = - det3x3( b1, b2, b3, c1, c2, c3, d1, d2, d3);
            out.val[1][3]  =   det3x3( a1, a2, a3, c1, c2, c3, d1, d2, d3);
            out.val[2][3]  = - det3x3( a1, a2, a3, b1, b2, b3, d1, d2, d3);
            out.val[3][3]  =   det3x3( a1, a2, a3, b1, b2, b3, c1, c2, c3);
        }

    private:
        void swap(Number & a, Number & b) {
            Number h = a;
            a = b;
            b = h;
        }
    };

    template <class T>
    bool almostEqual(const asl::Matrix4Base<T> & myMatrix1,
                     const asl::Matrix4Base<T> & myMatrix2,
                     T theTolerance = 1e-5)
    {
        return (almostEqual(myMatrix1[0], myMatrix2[0], theTolerance) &&
                almostEqual(myMatrix1[1], myMatrix2[1], theTolerance) &&
                almostEqual(myMatrix1[2], myMatrix2[2], theTolerance) &&
                almostEqual(myMatrix1[3], myMatrix2[3], theTolerance));
    }

    /* @} */
}

#endif

