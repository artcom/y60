//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _asl_BSpline_
#define _asl_BSpline_

#include "Vector234.h"
#include <math.h>

namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    /**
     * BSpline class.
     * Based on BSpline.js code.
     */
    template<class T>
    class BSpline {
    public:
        BSpline() {}

        /**
         * Create BSpline.
         * @param theStart Start point.
         * @param theStartAnchor Start point anchor.
         * @param theEnd End point.
         * @param theEndAnchor End point anchor.
         */
        BSpline(const Vector3<T> & theStart,
                const Vector3<T> & theStartAnchor,
                const Vector3<T> & theEnd,
                const Vector3<T> & theEndAnchor)
        {
            setControlPoints(theStart, theStartAnchor, theEnd, theEndAnchor);
        }

        /**
         * Create BSpline.
         * @param thePoints List of start, start-anchor, end, end-anchor points.
         */
        BSpline(const std::vector< Vector3<T> > & thePoints) {
            setControlPoints(thePoints);
        }

        /**
         * Setup spline.
         * @param theStart Start point.
         * @param theStartAnchor Start point anchor.
         * @param theEnd End point.
         * @param theEndAnchor End point anchor.
         */
        void setControlPoints(const Vector3<T> & theStart,
                              const Vector3<T> & theStartAnchor,
                              const Vector3<T> & theEnd,
                              const Vector3<T> & theEndAnchor)
        {

            // setup polynom coefficients
            for (unsigned i = 0; i < 3; ++i) {
                _myCoeff[0][i] = theStart[i];
                _myCoeff[1][i] = 3 * (theStartAnchor[i] - theStart[i]);
                _myCoeff[2][i] = 3 * (theEndAnchor[i] - theStartAnchor[i]) - _myCoeff[1][i];
                _myCoeff[3][i] = theEnd[i] - theStart[i] - _myCoeff[1][i] - _myCoeff[2][i];
            }
            _myArcLength = -1;
        }

        /**
         * Setup spline.
         * @param thePoints List of start, start-anchor, end, end-anchor points.
         */
        void setControlPoints(const std::vector< Vector3<T> > & thePoints) {
            setControlPoints(thePoints[0], thePoints[1],
                             thePoints[2], thePoints[3]);
        }

#if 0
        // UH: disabled since the only place this is used seems to be testBSpline.tst.js
        /**
         * Setup from points. WTF? In need of documentation.
         * Hint: it's not a convex hull thingy.
         */
        void setupFromPoints(const std::vector< Vector3<T> > & thePoints,
                             T theSize)
        {
            Vector3<T> myDir1 = product(normalized(difference(thePoints[2], thePoints[0])), theSize);
            Vector3<T> myStartAnchor = sum(thePoints[1], myDir1);

            Vector3<T> myDir2 = product(normalized(difference(thePoints[1], thePoints[3])), theSize);
            Vector3<T> myEndAnchor = sum(thePoints[2], myDir2);

            setControlPoints(thePoints[1], myStartAnchor, thePoints[2], myEndAnchor);
        }
#endif

        /**
         * Calculate a point on the spline.
         * @param theCurveParameter Position along the curve.
         * @param theEaseIn Ease-in. Range [0..1].
         * @param theEaseOut Ease-out. Range [0..1].
         * @return Point on the spline.
         */
        Vector3<T> evaluate(T theCurveParameter,
                            T theEaseIn = 0, T theEaseOut = 0) const
        {
            if (theEaseIn > 0 && theEaseOut > 0) {
                theEaseOut = 1 - theEaseOut;
                theCurveParameter = smoothStep(theCurveParameter, theEaseIn, theEaseOut);
            }
            if (theCurveParameter < 0) {
                theCurveParameter = 0;
            } else if (theCurveParameter > 1) {
                theCurveParameter = 1;
            }
            return getValue(theCurveParameter);
        }

        /**
         * Calculates the arc length of the spline and caches the result.
         * @return The arc length.
         */
        T getArcLength() {

            if (_myArcLength < 0) {

                Vector3<T> myStart = getValue(0);
                Vector3<T> myEnd = getValue(1);
                _myArcLength = getSegmentLength(0,1, myStart, myEnd, distance(myStart, myEnd));
            }

            return _myArcLength;
        }

        /**
         * Sample the spline at given intervals and caches the result.
         * Generates 'theResolution+1' points.
         * @param theResolution Number of intervals to subdivide the spline into.
         * @param theEaseIn Ease-in. Range [0..1].
         * @param theEaseOut Ease-out. Range [0..1].
         * @return List of points.
         */
        const std::vector< Vector3<T> > & calculate(unsigned theResolution,
                                                    T theEaseIn = 0, T theEaseOut = 0)
        {
            _myResult.clear();
            for (unsigned i = 0; i <= theResolution; ++i) {
                _myResult.push_back(evaluate(i / (double) theResolution, theEaseIn, theEaseOut));
            }

            return _myResult;
        }

        /// Get sampled points.
        const std::vector< Vector3<T> > & getResult() const {
            return _myResult;
        }

    private:

        Vector3<T> _myCoeff[4];
        T _myArcLength;
        std::vector< Vector3<T> > _myResult;

        // evaluate at 't'
        Vector3<T> getValue(T t) const {

            Vector3<T> myPoint;
            for (unsigned i = 0; i < 3; ++i) {
                myPoint[i] = _myCoeff[3][i] * t*t*t +
                    _myCoeff[2][i] * t*t +
                    _myCoeff[1][i] * t +
                    _myCoeff[0][i];
            }

            return myPoint;
        }

        // get segment length
        double getSegmentLength(double theLeft, double theRight,
                                Vector3<T> theLeftPoint, Vector3<T> theRightPoint,
                                T theChordLength)
        {
            const double MAX_ERROR = 0.01;

            double myCenter = 0.5 * (theLeft + theRight);
            Vector3<T> myCenterPoint = getValue(myCenter);

            double myLeftLen = distance(theLeftPoint, myCenterPoint);
            double myRightLen = distance(theRightPoint, myCenterPoint);

            double myError = ((myLeftLen + myRightLen) - theChordLength) / theChordLength;
            if (myError > MAX_ERROR) {
                myLeftLen = getSegmentLength(theLeft, myCenter, theLeftPoint, myCenterPoint, myLeftLen);
                myRightLen = getSegmentLength(myCenter, theRight, myCenterPoint, theRightPoint, myRightLen);
            }

            return myLeftLen + myRightLen;
        }
    };

    typedef BSpline<float> BSplinef;
    typedef BSpline<double> BSplined;

    template <class T>
    std::ostream & operator<<(std::ostream & os, const asl::BSpline<T> & theSpline) {
        return os << "BSpline";
    }

    template <class T>
    std::istream & operator>>(std::istream & is, asl::BSpline<T> & theSpline) {
        return is;
    }

    /* @} */

} // namespace asl
#endif
