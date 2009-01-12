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
//    $RCSfile: Box.h,v $
//
//     $Author: david $
//
//   $Revision: 1.25 $
//
// Description: Box2<Number> and Box3<Number> class
//
//=============================================================================

#ifndef _included_asl_Box_
#define _included_asl_Box_

#include "asl_math_settings.h"

#include "Vector234.h"
#include "Point234.h"
#include "numeric_functions.h"

#include <asl/base/Logger.h>
#include <math.h>

#undef min
#undef max

namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    /** An axis aligned two dimensional box formerly known as rectangle.
     * A Box2 is defined by a min and a max point. Because the box is axis
     * aligned that is all it needs to define its position and size.
     */
    template<class Number>
    class Box2 : public PairOf<Point2<Number> > {
    public:
        typedef PairOf<Point2<Number> > Base;
        enum { MIN = 0, MAX = 1 , DIMENSIONS = 2};
        typedef Number value_type;
        typedef Point2<Number> PointType;
        typedef Vector2<Number> VectorType;

        /** Default constructor */
        Box2() { }

        /** Copy constructor */
        Box2(const Box2 & theOtherBox) : Base(theOtherBox) { }

        /** Constructor given bounds */
        Box2(Number xmin, Number ymin, Number xmax, Number ymax)
            : Base(Point2<Number>(xmin, ymin), Point2<Number>(xmax, ymax))
        { }

        /** Constructor given minimum and maximum points */
        Box2(const Point2<Number> & theMinCorner, const Point2<Number> & theMaxCorner )
            : Base(theMinCorner, theMaxCorner)
        { }

        /** Destructor */
        ~Box2()
        { }

        /** Assignment operator */
        Box2 & operator=(const Box2 & t) {
            assign(t);
            return *this;
        }

        /** Returns the min point. */
        const Point2<Number> & getMin() const {
            return this->val[MIN];
        }
        /** Returns the max point. */
        const Point2<Number> & getMax() const {
            return this->val[MAX];
        }
        /** Returns the min point as a vector. */
        const Vector2<Number> & getMinVector() const  {
            return asVector(this->val[MIN]);
        }
        /** Returns the max point as a vector. */
        const Vector2<Number> & getMaxVector() const  {
            return asVector(this->val[MAX]);
        }
        /** Sets the min point.  */
        void setMin(const Point2<Number> & theValue){
            this->val[MIN] = theValue;
        }
        /** Sets the max point.  */
        void setMax(const Point2<Number> & theValue){
            this->val[MAX] = theValue;
        }
        /** Sets the min point from a vector.  */
        void setMinVector(const Vector2<Number> & theValue){
            this->val[MIN] = asPoint(theValue);
        }
        /** Sets the max point from a vector.  */
        void setMaxVector(const Vector2<Number> & theValue){
            this->val[MAX] = asPoint(theValue);
        }
        /** Returns the center of a box. */
        Point2<Number> getCenter() const {
            return asPoint((getMinVector() + getMaxVector()) / 2);
        }

        /** Extends Box2 if necessary to touch given 2D point */
        void extendBy(const Point2<Number> & pt) {
            if (pt[0] < this->val[MIN][0]) {
                this->val[MIN][0] = pt[0];
            }
            if (pt[1] < this->val[MIN][1]) {
                this->val[MIN][1] = pt[1];
            }
            if (pt[0] > this->val[MAX][0]) {
                this->val[MAX][0] = pt[0];
            }
            if (pt[1] > this->val[MAX][1]) {
                this->val[MAX][1] = pt[1];
            }
        }
        /** Extends Box2 (if necessary) to touch given Box2 */
        void extendBy(const Box2 & r) {
            if (r.hasPosition()) {
                extendBy(r.getMin());
                extendBy(r.getMax());
            }
        }

        /** Returns TRUE if given point is inside or on boundary of box */
        bool contains(const Point2<Number> & pt) const {
            return (pt[0]>=this->val[MIN][0]) && (pt[0]<=this->val[MAX][0])
                && (pt[1]>=this->val[MIN][1]) && (pt[1]<=this->val[MAX][1]);
        }

        /** TRUE, if given box does not extend box */
        bool contains(const Box2 & b) const {
            return contains(b.val[MIN]) && contains(b.val[MAX]);
        }
        /** Returns TRUE if given point is inside the box, and FALSE if the point is on the edge or outside of the box.
         * @see contains(const Point2<Number> & pt) const
         */
        bool envelopes(const Point2<Number> & pt) const {
            return (pt[0]>this->val[MIN][0]) && (pt[0]<this->val[MAX][0])
                && (pt[1]>this->val[MIN][1]) && (pt[1]<this->val[MAX][1]);
        }
        /** Returns TRUE if given box is completely inside the box.  */
        bool envelopes(const Box2 & bb) const {
            return bb.hasPosition() && hasPosition() &&
                  (envelopes(bb.getMin()) && envelopes(bb.getMax()));
        }
        /** Returns TRUE if intersection of given boxes is a non-empty box 
           Note that in many cases you will prefer to use "touches" for  calculation
           of collision and visibility determination in order to fail on the safe side
           regarding numerical inaccuracies
         * @see touches(const Box2<Number> & pt) const
        */
        bool intersects(const Box2 & theOtherBox) const {
            return 
                (asl::maximum(theOtherBox.val[MIN][0], this->val[MIN][0]) < 
                 asl::minimum(theOtherBox.val[MAX][0], this->val[MAX][0])) &&
                (asl::maximum(theOtherBox.val[MIN][1], this->val[MIN][1]) < 
                 asl::minimum(theOtherBox.val[MAX][1], this->val[MAX][1]));
        }
        /** Returns TRUE if at least one point exists that is contained by both boxes;
         the difference to intersects() is that touches() returns true if the two boxes
         share only one edge or corner and the intersection is empty
         * @see intersects(const Box2<Number> & pt) const
        */
        bool touches(const Box2 & theOtherBox) const {
            return 
                (asl::maximum(theOtherBox.val[MIN][0], this->val[MIN][0]) <= 
                 asl::minimum(theOtherBox.val[MAX][0], this->val[MAX][0])) &&
                (asl::maximum(theOtherBox.val[MIN][1], this->val[MIN][1]) <= 
                 asl::minimum(theOtherBox.val[MAX][1], this->val[MAX][1]));
        }
       /** Set the bounding values from a box.  */
        void setBounds(const Box2 & theBox) {
            *this = theBox;
        }

        /** Set the bounding values from two points.  */
        void setBounds(const Point2<Number> & theMinCorner, const Point2<Number> & theMaxCorner){
            this->val[MIN] = theMinCorner;
            this->val[MAX] = theMaxCorner;
        }
        /** Set the bounding values from scalar values.  */
        void setBounds(Number xmin, Number ymin, Number xmax, Number ymax) {
            this->val[MIN] = Point2<Number>(xmin, ymin);
            this->val[MAX] = Point2<Number>(xmax, ymax);
        }

        /** Get the bounding values as scalar values.  */
        void getBounds(Number & xmin, Number & ymin, Number & xmax, Number & ymax) const {
            xmin = this->val[MIN][0];
            ymin = this->val[MIN][1];
            xmax = this->val[MAX][0];
            ymax = this->val[MAX][1];
        }

        /** Get the bounding values as two points.  */
        void getBounds(Point2<Number> & theMinCorner, Point2<Number> & theMaxCorner) const {
            theMinCorner = this->val[MIN];
            theMaxCorner = this->val[MAX];
        }

        /** Get the corners of the box.
         * @param theLeftTop
         * @param theRightBottom
         * @param theRightTop
         * @param theLeftBottom
         */
        void getCorners(Point2<Number> & theLeftTop, Point2<Number> & theRightBottom,
                        Point2<Number> & theRightTop, Point2<Number> & theLeftBottom) const
        {
            Number t, l, b, r;
            getBounds(l, t, r, b);
            theLeftTop = Point2<Number>(l, t);
            theRightBottom = Point2<Number>(r, b);
            theRightTop = Point2<Number>(r, t);
            theLeftBottom = Point2<Number>(l, b);
        }

        /** Get the origin (minimum point) of the box as scalars.
         * @param originX
         * @param originY
         */
        void getOrigin(Number & originX, Number & originY) const {
            originX = this->val[MIN][0]; originY = this->val[MIN][1];
        }
        /** Gets the size of the box as scalars 
         * @param sizeX
         * @param sizeY
         */
        void getSize(Number & sizeX, Number & sizeY) const {
            if (hasSize()) {
                sizeX = this->val[MAX][0] - this->val[MIN][0];
                sizeY = this->val[MAX][1] - this->val[MIN][1];
            } else {
                sizeX = 0;
                sizeY = 0;
            }
        }
        /** Returns the size of the box as a vector. */
        Vector2<Number> getSize() const {
            if (hasSize()) {
                return (Point2<Number>(this->val[MAX][0] - this->val[MIN][0], this->val[MAX][1] - this->val[MIN][1]));
            } else {
                return Vector2<Number>(Number(0),Number(0));
            }
        }
        /** Sets the size of the box while keeping its center.
         * @param theSize
         */
        void setSize(const Vector2<Number> & theSize) {
            Point2<Number> myCenter = getCenter();
            Vector2<Number> myHalfSize = theSize/2;
            this->val[MIN] = myCenter - myHalfSize;
            this->val[MAX] = myCenter + myHalfSize;
        }

        /** Sets the center of the box while keeping its size.
         * @param theCenter
         */
        void setCenter(const Point2<Number> & theCenter) {
            Vector2<Number> myHalfSize = getSize()/2;
            this->val[MIN] = theCenter - myHalfSize;
            this->val[MAX] = theCenter + myHalfSize;
        }
        /** Returns aspect ratio (ratio of width to height) of box */
        Number getAspectRatio() const {
            return (this->val[MAX][0] - this->val[MIN][0]) / (this->val[MAX][1] - this->val[MIN][1]);
        }

        /** Sets rect to contain nothing */
        void makeEmpty() {
            setBounds(
                NumericTraits<Number>::max(),
                NumericTraits<Number>::max(),
                NumericTraits<Number>::min(),
                NumericTraits<Number>::min()
                );
        }

        /** Sets rect to contain everything except Full */
        void makeFull() {
            setBounds(
                NumericTraits<Number>::min(),
                NumericTraits<Number>::min(),
                NumericTraits<Number>::max(),
                NumericTraits<Number>::max()
                );
        }

        /** Sets rect to correct min/max values */
        void makeCorrect() {
            if (this->val[MIN][0] > this->val[MAX][0]) {
                Number h = this->val[MIN][0];
                this->val[MIN][0] = this->val[MAX][0];
                this->val[MAX][0] = h;
            }
            if (this->val[MIN][1] > this->val[MAX][1]) {
                Number h = this->val[MIN][1];
                this->val[MIN][1] = this->val[MAX][1];
                this->val[MAX][1] = h;
            }
        }

        /** intersects box with another box */
        void intersect(const Box2 & theOtherBox) {
            if (theOtherBox.val[MIN][0] > this->val[MIN][0]) {
                this->val[MIN][0] = theOtherBox.val[MIN][0];
            }
            if (theOtherBox.val[MIN][1] > this->val[MIN][1]) {
                this->val[MIN][1] = theOtherBox.val[MIN][1];
            }
            if (theOtherBox.val[MAX][0] < this->val[MAX][0]) {
                this->val[MAX][0] = theOtherBox.val[MAX][0];
            }
            if (theOtherBox.val[MAX][1] < this->val[MAX][1]) {
                this->val[MAX][1] = theOtherBox.val[MAX][1];
            }
        }

        /** Return whether the box is empty, meaning it has no area */
        bool isEmpty() const {
            return !hasArea();
        }

        /** Checks if the box is not degenerated, meaning that one minimum coordinaate is larger than one max coordinaate */
        bool hasPosition() const {
            return ((this->val[MAX][0] >= this->val[MIN][0]) && (this->val[MAX][1] >= this->val[MIN][1]));
        }

        /** Checks if the box is extended on at least one dimensions */
        bool hasSize() const {
            return (((this->val[MAX][0] >  this->val[MIN][0]) && (this->val[MAX][1] >= this->val[MIN][1]))  ||
                    ((this->val[MAX][0] >= this->val[MIN][0]) && (this->val[MAX][1] >  this->val[MIN][1])));
        }

        /** Checks if the box has area; i.e., both dimensions have positive size */
        bool hasArea() const {
            return ((this->val[MAX][0] > this->val[MIN][0]) && (this->val[MAX][1] > this->val[MIN][1]));
        }
        /** a 2D box never has volume */
        bool hasVolume() const {
            return false;
        }
        /** Returns the area contained by the box */
        Number getArea() const {
            if (hasArea()) {
                return (this->val[MAX][0] - this->val[MIN][0]) * (this->val[MAX][1] - this->val[MIN][1]);
            } else {
                return 0;
            }
        }
        /** Returns the volume contained by the box. A 2D box does not have a volume. */
        Number getVolume() const {
            return 0;
        }

        /** @name Arithmetic Operations */
        /* @{ */
        template <class otherReal>
        void mult(const otherReal& y) {
            this->val[MIN].mult(Number(y));
            this->val[MAX].mult(Number(y));
        }
        template <class otherReal>
        void div(const otherReal& y) {
            this->val[MIN].div(Number(y));
            this->val[MAX].div(Number(y));
        }

        void add(const Vector2<Number> & y) {
            this->val[MIN].add(y);
            this->val[MAX].add(y);
        }
        void sub(const Vector2<Number> & y) {
            this->val[MIN].sub(y);
            this->val[MAX].sub(y);
        }
        void mult(const Vector2<Number> & y) {
            this->val[MIN].mult(y);
            this->val[MAX].mult(y);
        }
        void div(const Vector2<Number> & y) {
            this->val[MIN].div(y);
            this->val[MAX].div(y);
        }

        /** Multiplicative assignment operator for scalars*/
        template <class otherReal>
        void operator*=(const otherReal& y) {
            this->val[MIN].mult(Number(y));
            this->val[MAX].mult(Number(y));
        }
        /** Dividing assignment operator for scalars*/
        template <class otherReal>
        void operator/=(const otherReal& y) {
            this->val[MIN].div(Number(y));
            this->val[MAX].div(Number(y));
        }
        /** Additive assignment operator for Vector2 */
        void operator+=(const Vector2<Number> & y) {
            this->val[MIN].add(y);
            this->val[MAX].add(y);
        }
        /** Subtractive assignment operator for Vector2 */
        void operator-=(const Vector2<Number> & y) {
            this->val[MIN].sub(y);
            this->val[MAX].sub(y);
        }
        /** Multiplicative assignment operator for Vector2 */
        void operator*=(const Vector2<Number> & y) {
            this->val[MIN].mult(y);
            this->val[MAX].mult(y);
        }
        /** Dividing assignment operator for Vector2 */
        void operator/=(const Vector2<Number> & y) {
            this->val[MIN].div(y);
            this->val[MAX].div(y);
        }
        /** Addition operator for Vector2 */
        Box2 operator+(const Vector2<Number> & y) const {
            Box2 result(*this);
            result[0].add(y);
            result[1].add(y);
            return result;
        }
        /** Subtraction operator for Vector2 */
        Box2<Number> operator-(const Vector2<Number> & y) const {
            Box2 result(*this);
            result[0].sub(y);
            result[1].sub(y);
            return result;
        }
        /** Multiplication operator for floats */ 
        Box2 operator*(const float & y) const {
            Box2 result(*this);
            result[0].mult(Number(y));
            result[1].mult(Number(y));
            return result;
        }
        /** Multiplication operator for double */ 
        Box2 operator*(const double & y) const {
            Box2 result(*this);
            result[0].mult(Number(y));
            result[1].mult(Number(y));
            return result;
        }
        /** Multiplication operator for int */ 
        Box2 operator*(const int & y) const {
            Box2 result(*this);
            result[0].mult(Number(y));
            result[1].mult(Number(y));
            return result;
        }

        /** Division operator for float */ 
        Box2 operator/(const float & y) const {
            Box2 result(*this);
            result[0].div(Number(y));
            result[1].div(Number(y));
            return result;
        }
        /** Division operator for double */ 
        Box2 operator/(const double & y) const {
            Box2 result(*this);
            result[0].div(Number(y));
            result[1].div(Number(y));
            return result;
        }
        /** Division operator for int */ 
        Box2 operator/(const int & y) const {
            Box2 result(*this);
            result[0].div(Number(y));
            result[1].div(Number(y));
            return result;
        }
        /* @} */

    };

    /** Multiplication operator for float
     * @relates Box2
     */ 
    template<class Number>
    Box2<Number> operator*(const float & y, const Box2<Number> & v) {
        Box2<Number> result(v);
        result.mult(Point2<Number>::value_type(y));
        return result;
    }
    /** Multiplication operator for double
     * @relates Box2
     */ 
    template<class Number>
    Box2<Number> operator*(const double & y, const Box2<Number> & v) {
        Box2<Number> result(v);
        result.mult(Point2<Number>::value_type(y));
        return result;
    }
    /** Multiplication operator for int
     * @relates Box2
     */ 
    template<class Number>
    Box2<Number> operator*(const int & y, const Box2<Number> & v) {
        Box2<Number> result(v);
        result.mult(Box2<Number>::value_type(y));
        return result;
    }

    typedef Box2<int>    Box2i;
    typedef Box2<float>  Box2f;
    typedef Box2<double> Box2d;

    typedef Box2<int>    Recti;
    typedef Box2<float>  Rectf;
    typedef Box2<double> Rectd;

    /** An axis aligned three dimensional box.
     * A Box3 is defined by a min and a max point. Because the box is axis
     * aligned that is all it needs to define its position and size.
     */
    template<class Number>
    class Box3 : public PairOf<Point3<Number> > {
    public:
        typedef Number value_type;
        typedef PairOf<Point3<Number> > Base;
        typedef Point3<Number> PointType;
        typedef Vector3<Number> VectorType;

        enum { MIN = 0, MAX = 1, DIMENSIONS = 3 };

        /** Default constructor */
        Box3() {};
        Box3(const Box3 & theOtherBox)
            : Base(theOtherBox)
        {}

        /** Constructor given bounds */
        Box3(Number xmin, Number ymin, Number zmin, Number xmax, Number ymax, Number zmax)
            : Base(Point3<Number>(xmin, ymin, zmin), Point3<Number>(xmax, ymax, zmax))
        {}

        /** Constructor given minimum and maximum points. */
        Box3(const Point3<Number> & theMinCorner, const Point3<Number> & theMaxCorner )
            : Base(theMinCorner, theMaxCorner)
        {}

        /** Destructor */
        ~Box3() {}

        /** Returns the min point */
        const Point3<Number> & getMin() const  {
            return this->val[MIN];
        }
        /** Returns the max point */
        const Point3<Number> & getMax() const  {
            return this->val[MAX];
        }
        /** Returns the min point as a vector */
        const Vector3<Number> & getMinVector() const  {
            return reinterpret_cast<const Vector3<Number> &>(this->val[MIN]);
        }
        /** Returns the max point as a vector */
        const Vector3<Number> & getMaxVector() const  {
            return reinterpret_cast<const Vector3<Number> &>(this->val[MAX]);
        }
        /** Sets the min point.  */
        void setMin(const Point3<Number> & theValue){
            this->val[MIN] = theValue;
        }
        /** Sets the max point.  */
        void setMax(const Point3<Number> & theValue){
            this->val[MAX] = theValue;
        }
        /** Sets the min point from a vector.  */
        void setMinVector(const Vector3<Number> & theValue){
            this->val[MIN] = asPoint(theValue);
        }
        /** Sets the max point from a vector.  */
        void setMaxVector(const Vector3<Number> & theValue){
            this->val[MAX] = asPoint(theValue);
        }
        /** Returns the center of a box. */
        Point3<Number> getCenter() const {
            return asPoint((getMinVector() + getMaxVector()) / 2);
        }

        /** Extends Box if necessary to touch given 3D point. */
        void extendBy(const Point3<Number> & pt) {
            if (pt[0] < this->val[MIN][0]) {
                this->val[MIN][0] = pt[0];
            }
            if (pt[1] < this->val[MIN][1]) {
                this->val[MIN][1] = pt[1];
            }
            if (pt[2] < this->val[MIN][2]) {
                this->val[MIN][2] = pt[2];
            }
            if (pt[0] > this->val[MAX][0]) {
                this->val[MAX][0] = pt[0];
            }
            if (pt[1] > this->val[MAX][1]) {
                this->val[MAX][1] = pt[1];
            }
            if (pt[2] > this->val[MAX][2]) {
                this->val[MAX][2] = pt[2];
            }
        }
        /** Extends Box3f (if necessary) to touch given Box3f. */
        void extendBy(const Box3 & r) {
            // Check if box is valid (i.e. maxvalues >= minvalues)
            if (r.hasPosition()) {
                extendBy(r.getMin());
                extendBy(r.getMax());
            }
        }
       /** intersects box with another box */
        void intersect(const Box3 & theOtherBox) {
            if (theOtherBox.val[MIN][0] > this->val[MIN][0]) {
                this->val[MIN][0] = theOtherBox.val[MIN][0];
            }
            if (theOtherBox.val[MIN][1] > this->val[MIN][1]) {
                this->val[MIN][1] = theOtherBox.val[MIN][1];
            }
            if (theOtherBox.val[MIN][2] > this->val[MIN][2]) {
                this->val[MIN][2] = theOtherBox.val[MIN][2];
            }
             if (theOtherBox.val[MAX][0] < this->val[MAX][0]) {
                this->val[MAX][0] = theOtherBox.val[MAX][0];
            }
            if (theOtherBox.val[MAX][1] < this->val[MAX][1]) {
                this->val[MAX][1] = theOtherBox.val[MAX][1];
            }
            if (theOtherBox.val[MAX][2] < this->val[MAX][2]) {
                this->val[MAX][2] = theOtherBox.val[MAX][2];
            }
         }


        /** Returns TRUE if given point is inside or on boundary of box */
        bool contains(const Point3<Number> & pt) const {
            return (pt[0] >= this->val[MIN][0]) && (pt[0] <= this->val[MAX][0])
                && (pt[1] >= this->val[MIN][1]) && (pt[1] <= this->val[MAX][1])
                && (pt[2] >= this->val[MIN][2]) && (pt[2] <= this->val[MAX][2]);
        }

        /** TRUE, if given box does not extend box. */
        bool contains(const Box3 &b) const {
            return contains(b.val[MIN]) && contains(b.val[MAX]);
        }
        /** Returns TRUE if given point is inside the box, and FALSE if the point is on the edge or outside of the box.
         * @see contains(const Point3<Number> & pt) const
         */
        bool envelopes(const Point3<Number> & pt) const {
            return (pt[0]>this->val[MIN][0]) && (pt[0]<this->val[MAX][0])
                && (pt[1]>this->val[MIN][1]) && (pt[1]<this->val[MAX][1])
                && (pt[2]>this->val[MIN][2]) && (pt[2]<this->val[MAX][2]);
        }
         /** Returns TRUE if given box is completely inside the box.  */
        bool envelopes(const Box3 &bb) const {
            return bb.hasPosition() && hasPosition() &&
                   (envelopes(bb.getMin()) && envelopes(bb.getMax()));
        }
        /** Returns TRUE if intersection of given boxes is a non-empty box 
           Note that in many cases you will prefer to use "touches" for  calculation
           of collision and visibility determination in order to fail on the safe side
           regarding numerical inaccuracies
         * @see touches(const Box3<Number> & pt) const
        */
        bool intersects(const Box3 & theOtherBox) const {
            return
                (asl::maximum(theOtherBox.val[MIN][0], this->val[MIN][0]) < 
                 asl::minimum(theOtherBox.val[MAX][0], this->val[MAX][0])) &&
                (asl::maximum(theOtherBox.val[MIN][1], this->val[MIN][1]) < 
                 asl::minimum(theOtherBox.val[MAX][1], this->val[MAX][1])) &&
                (asl::maximum(theOtherBox.val[MIN][2], this->val[MIN][2]) < 
                 asl::minimum(theOtherBox.val[MAX][2], this->val[MAX][2]));
        }

       /** Returns TRUE if at least one point exists that is contained by both boxes;
         the difference to intersects() is that touches() returns true if the two boxes
         share only one edge or corner and the intersection is empty
         * @see intersects(const Box3<Number> & pt) const
        */
       bool touches(const Box3 & theOtherBox) const {
#ifdef OLD_IMPL
            // TODO: make an implementation with fabs
            const asl::Vector3<Number> myCenterOffset_twice = (getMin()+getMax()) - (bb.getMin()+bb.getMax());
            const asl::Vector3<Number> myDoubleSize = getSize()+bb.getSize();
            return fabs(myCenterOffset_twice[0]) <= (myDoubleSize[0])
                && fabs(myCenterOffset_twice[1]) <= (myDoubleSize[1])
                && fabs(myCenterOffset_twice[2]) <= (myDoubleSize[2]);
#else
           return
                (asl::maximum(theOtherBox.val[MIN][0], this->val[MIN][0]) <= 
                 asl::minimum(theOtherBox.val[MAX][0], this->val[MAX][0])) &&
                (asl::maximum(theOtherBox.val[MIN][1], this->val[MIN][1]) <= 
                 asl::minimum(theOtherBox.val[MAX][1], this->val[MAX][1])) &&
                (asl::maximum(theOtherBox.val[MIN][2], this->val[MIN][2]) <= 
                 asl::minimum(theOtherBox.val[MAX][2], this->val[MAX][2]));
#endif
        }
       // Common get and set functions
        void setBounds(Number xmin, Number ymin, Number zmin,
                       Number xmax, Number ymax, Number zmax)
        {
            this->val[MIN] = Point3<Number>(xmin, ymin, zmin);
            this->val[MAX] = Point3<Number>(xmax, ymax, zmax);
        }

        void setBounds(const Point3<Number> & theMinCorner, const Point3<Number> & theMaxCorner) {
            this->val[MIN] = theMinCorner;
            this->val[MAX] = theMaxCorner;
        }
        void setBounds(const Box3 & theBox) {
            *this = theBox;
        }

        void getBounds(Number & xmin, Number & ymin, Number & zmin,
            Number & xmax, Number & ymax, Number & zmax) const
        {
            xmin = this->val[MIN][0];
            ymin = this->val[MIN][1];
            zmin = this->val[MIN][2];
            xmax = this->val[MAX][0];
            ymax = this->val[MAX][1];
            zmax = this->val[MAX][2];
        }

        void getBounds(Point3<Number> & theMinCorner, Point3<Number> & theMaxCorner) const {
            theMinCorner = this->val[MIN];
            theMaxCorner = this->val[MAX];
        }

        // return the eight corner points of the box
        void getCorners(
            Point3<Number> & _ltf,  Point3<Number> &_rbf, Point3<Number> &_rtf,  Point3<Number> &_lbf,
            Point3<Number> & _ltbk, Point3<Number> &_rbbk,Point3<Number> &_rtbk, Point3<Number> &_lbbk) const
        {
            Number t, l, b, r, f, bk;
            getBounds(l, t, f, r, b, bk);
            _ltf  = Point3<Number>(l, t, f);
            _rbf  = Point3<Number>(r, b, f);
            _rtf  = Point3<Number>(r, t, f);
            _lbf  = Point3<Number>(l, b, f);
            _ltbk = Point3<Number>(l, t, bk);
            _rbbk = Point3<Number>(r, b, bk);
            _rtbk = Point3<Number>(r, t, bk);
            _lbbk = Point3<Number>(l, b, bk );
        }

        // Returns origin (minimum point) of box
        void getOrigin(Number & originX, Number & originY, Number originZ) const {
            originX = this->val[MIN][0];
            originY = this->val[MIN][1];
            originZ = this->val[MIN][2];
        }
        // Returns size of box
        void getSize(Number & sizeX, Number & sizeY, Number & sizeZ) const {
            if (hasSize()) {
                sizeX = this->val[MAX][0] - this->val[MIN][0];
                sizeY = this->val[MAX][1] - this->val[MIN][1];
                sizeZ = this->val[MAX][2] - this->val[MIN][2];
            } else {
                sizeX = 0;
                sizeY = 0;
                sizeZ = 0;
            }
        }

        Vector3<Number> getSize() const {
            if (hasSize()) {
                return this->val[MAX] - this->val[MIN];
            } else {
                return Vector3<Number>(0,0,0);
            }
        }

        void setSize(const Vector3<Number> & theSize) {
            Point3<Number> myCenter = getCenter();
            Vector3<Number> myHalfSize = theSize/2;
            this->val[MIN] = myCenter - myHalfSize;
            this->val[MAX] = myCenter + myHalfSize;
        }
        void setCenter(const Point3<Number> & theCenter) {
            Vector3<Number> myHalfSize = getSize()/2;
            this->val[MIN] = theCenter - myHalfSize;
            this->val[MAX] = theCenter + myHalfSize;
        }

        // Sets rect to contain nothing
        void makeEmpty() {
            setBounds(
                NumericTraits<Number>::max(),
                NumericTraits<Number>::max(),
                NumericTraits<Number>::max(),
                NumericTraits<Number>::min(),
                NumericTraits<Number>::min(),
                NumericTraits<Number>::min()
                );
        }

        // Sets rect to contain everything except Full
        void makeFull() {
            setBounds(
                NumericTraits<Number>::min(),
                NumericTraits<Number>::min(),
                NumericTraits<Number>::min(),
                NumericTraits<Number>::max(),
                NumericTraits<Number>::max(),
                NumericTraits<Number>::max()
                );
        }

        // Sets rect to correct min/max values
        void makeCorrect() {
            if (this->val[MIN][0] > this->val[MAX][0]) {
                Number h = this->val[MIN][0];
                this->val[MIN][0] = this->val[MAX][0];
                this->val[MAX][0] = h;
            }
            if (this->val[MIN][1] > this->val[MAX][1]) {
                Number h = this->val[MIN][1];
                this->val[MIN][1] = this->val[MAX][1];
                this->val[MAX][1] = h;
            }
            if (this->val[MIN][2] > this->val[MAX][2]) {
                Number h = this->val[MIN][2];
                this->val[MIN][2] = this->val[MAX][2];
                this->val[MAX][2] = h;
            }
        }

        bool isEmpty() const {
            return !hasVolume();
        }

        // Checks if the box is not degenerated
        bool hasPosition() const {
            return ((this->val[MAX][0] >= this->val[MIN][0]) && (this->val[MAX][1] >= this->val[MIN][1]) && (this->val[MAX][2] >= this->val[MIN][2]));
        }

        // Checks if the box is extended on one dimensions
        bool hasSize() const {
            return (((this->val[MAX][0] >  this->val[MIN][0]) && (this->val[MAX][1] >= this->val[MIN][1]) && (this->val[MAX][2] >= this->val[MIN][2])) ||
                    ((this->val[MAX][0] >= this->val[MIN][0]) && (this->val[MAX][1] >  this->val[MIN][1]) && (this->val[MAX][2] >= this->val[MIN][2])) ||
                    ((this->val[MAX][0] >= this->val[MIN][0]) && (this->val[MAX][1] >= this->val[MIN][1]) && (this->val[MAX][2] >  this->val[MIN][2])));
        }

        // Checks if the box is extended on two dimensions
        bool hasArea() const {
            return (((this->val[MAX][0] >  this->val[MIN][0]) && (this->val[MAX][1] >  this->val[MIN][1]) && (this->val[MAX][2] >= this->val[MIN][2])) ||
                    ((this->val[MAX][0] >  this->val[MIN][0]) && (this->val[MAX][1] >= this->val[MIN][1]) && (this->val[MAX][2] >  this->val[MIN][2])) ||
                    ((this->val[MAX][0] >= this->val[MIN][0]) && (this->val[MAX][1] >  this->val[MIN][1]) && (this->val[MAX][2] >  this->val[MIN][2])));
        }
        Number getArea() const {
            if (hasArea()) {
                Point3<Number> mySize = getSize();
                return 2*(mySize[0]*mySize[1]+mySize[1]*mySize[2]+mySize[0]*mySize[2]);
            } else {
                return 0;
            }
        }
        // Checks if the box has volueme; i.e. all dimensions have positive size
        bool hasVolume() const {
            return ((this->val[MAX][0] > this->val[MIN][0]) && (this->val[MAX][1] > this->val[MIN][1]) && (this->val[MAX][2] > this->val[MIN][2]));
        }

        Number getVolume() const {
            if (hasVolume()) {
                return (this->val[MAX][0] - this->val[MIN][0]) * (this->val[MAX][1] - this->val[MIN][1]) *
                       (this->val[MAX][2] - this->val[MIN][2]);
            } else {
                return 0;
            }
        }

        Box3 & operator=(const Box3 & t) {
            assign(t);
            return *this;
        }

        template <class otherReal>
        void mult(const otherReal& y) {
            this->val[MIN].mult(Number(y));
            this->val[MAX].mult(Number(y));
        }
        template <class otherReal>
        void div(const otherReal& y) {
            this->val[MIN].div(Number(y));
            this->val[MAX].div(Number(y));
        }

        void add(const Vector3<Number> & y) {
            this->val[MIN].add(y);
            this->val[MAX].add(y);
        }
        void sub(const Vector3<Number> & y) {
            this->val[MIN].sub(y);
            this->val[MAX].sub(y);
        }
        void mult(const Vector3<Number> & y) {
            this->val[MIN].mult(y);
            this->val[MAX].mult(y);
        }
        void div(const Vector3<Number> & y) {
            this->val[MIN].div(y);
            this->val[MAX].div(y);
        }

        void operator+=(const Vector3<Number> & y) {
            this->val[MIN].add(y);
            this->val[MAX].add(y);
        }
        void operator-=(const Vector3<Number> & y) {
            this->val[MIN].sub(y);
            this->val[MAX].sub(y);
        }
        void operator*=(const Vector3<Number> & y) {
            this->val[MIN].mult(y);
            this->val[MAX].mult(y);
        }
        void operator/=(const Vector3<Number> & y) {
            this->val[MIN].div(y);
            this->val[MAX].div(y);
        }

        Box3 operator*(const float & y) const {
            Box3 result(*this);
            result[0].mult(Number(y));
            result[1].mult(Number(y));
            return result;
        }
        Box3 operator*(const double & y) const {
            Box3 result(*this);
            result[0].mult(Number(y));
            result[1].mult(Number(y));
            return result;
        }
        Box3 operator*(const int & y) const {
            Box3 result(*this);
            result[0].mult(Number(y));
            result[1].mult(Number(y));
            return result;
        }

        Box3 operator/(const float & y) const {
            Box3 result(*this);
            result[0].div(Number(y));
            result[1].div(Number(y));
            return result;
        }
        Box3 operator/(const double & y) const {
            Box3 result(*this);
            result[0].div(Number(y));
            result[1].div(Number(y));
            return result;
        }
        Box3 operator/(const int & y) const {
            Box3 result(*this);
            result[0].div(Number(y));
            result[1].div(Number(y));
            return result;
        }

        Box3 operator+(const Vector3<Number> & y) const {
            Box3 result(*this);
            result[0].add(y);
            result[1].add(y);
            return result;
        }
        Box3<Number> operator-(const Vector3<Number> & y) const {
            Box3 result(*this);
            result[0].sub(y);
            result[1].sub(y);
            return result;
        }
    };

    template<class Number>
    Box3<Number> operator*(const float & y, const Box3<Number> & v) {
        Box3<Number> result(v);
        result.mult(Point3<Number>::value_type(y));
        return result;
    }
    template<class Number>
    Box3<Number> operator*(const double & y, const Box3<Number> & v) {
        Box3<Number> result(v);
        result.mult(Point3<Number>::value_type(y));
        return result;
    }
    template<class Number>
    Box3<Number> operator*(const int & y, const Box3<Number> & v) {
        Box3<Number> result(v);
        result.mult(Box3<Number>::value_type(y));
        return result;
    }

    typedef Box3<int>    Box3i;
    typedef Box3<float>  Box3f;
    typedef Box3<double> Box3d;

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


    template <class T>
    std::ostream & printBox (std::ostream & os, const T & b,
            bool oneElementPerLine = false,
            const char theStartToken = '[',
            const char theEndToken = ']',
            const char theDelimiter = ',')

    {
        if (!b.hasPosition()) {
            return os << theStartToken << theEndToken;
        } else {
            return asl::printVector(os, b, oneElementPerLine, theStartToken, theEndToken, theDelimiter);
        }
    }

    template <class T>
    std::ostream & operator << (std::ostream & os, const asl::Box2<T> & theBox) {
        if (os.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return printBox(os, theBox,
                    0 != os.iword(FixedVectorStreamFormatter::ourOneElementPerLineFlagIndex),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourDelimiterIndex)) );
        } else {
            return asl::printBox(os, theBox);
        }
    }

    template <class T>
    std::ostream & operator << (std::ostream & os, const asl::Box3<T> & theBox) {
        if (os.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return printBox(os, theBox,
                    0 != os.iword(FixedVectorStreamFormatter::ourOneElementPerLineFlagIndex),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                    static_cast<char>(os.iword(FixedVectorStreamFormatter::ourDelimiterIndex)) );
        } else {
            return asl::printBox(os, theBox);
        }
    }

    template <class T>
    std::istream & parseBox(std::istream & is, T & b,
                           const char theStartToken = '[',
                           const char theEndToken = ']',
                           const char theDelimiter = ',')
    {
        char myChar1;
        char myChar2;
        is >> myChar1;
        is >> myChar2;
        if (myChar1 == theStartToken && myChar2 == theEndToken) {
            b.makeEmpty();
            return is;
        } else {
            is.putback(myChar2);
            is.putback(myChar1);
            return asl::parseVector(is, b, theStartToken, theEndToken, theDelimiter);
        }
    }

    template <class T>
    std::istream & operator>>(std::istream & is, asl::Box2<T> & theBox) {
        if (is.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return parseBox(is, theBox, static_cast<char>(is.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                                        static_cast<char>(is.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                                        static_cast<char>(is.iword(FixedVectorStreamFormatter::ourDelimiterIndex)));
        } else {
            return asl::parseBox(is, theBox);
        }
    }

    template <class T>
    std::istream & operator>>(std::istream & is, asl::Box3<T> & theBox) {
        if (is.iword(FixedVectorStreamFormatter::ourIsFormattedFlag)) {
            return parseBox(is, theBox, static_cast<char>(is.iword(FixedVectorStreamFormatter::ourStartTokenIndex)),
                                        static_cast<char>(is.iword(FixedVectorStreamFormatter::ourEndTokenIndex)),
                                        static_cast<char>(is.iword(FixedVectorStreamFormatter::ourDelimiterIndex)));
        } else {
            return asl::parseBox(is, theBox);
        }
    }

    /* @} */

} // namespace asl;
#endif
