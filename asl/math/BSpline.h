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
#include <asl/base/Ptr.h>

namespace asl {

    /*! @addtogroup aslmath */
    /* @{ */

    /**
     * BSpline class.
     * Based on BSpline.js code. [Rev 5374]
     * Note: this is not a BSpline but a Bezier curve...
     */
    template<class T>
    class BSpline {
    public:
        class Position {
            public:
                Position(const BSpline & theSpline, const float & theStrokeSize) :
                    _mySpline(theSpline),
                    _myStrokeSize(theStrokeSize)
                {}
    
                Vector3<T> operator()(unsigned x, unsigned y) const {
                    const std::vector<Vector3<T> > & myPoints = _mySpline.getResult();
                    if (x >= myPoints.size()) {
                        throw asl::Exception(std::string("Invalid x value for bspline-position: ") + 
                            as_string(x) + ". Must be less than " +
                            as_string(myPoints.size()), PLUS_FILE_LINE);
                    }
                    if (x == myPoints.size() - 1) {
                        x--;
                    }
                    Vector3<T> mySideVector;
                    Vector3<T> myUpVector;
                    _mySpline.getSideAndUpVector(myPoints[x + 1] - myPoints[x], mySideVector, myUpVector);
                    mySideVector *= _myStrokeSize / 2;
                    if (y == 0) {
                        return Vector3<T>(myPoints[x] - mySideVector);
                    } else {
                        return Vector3<T>(myPoints[x] + mySideVector);
                    }
                }
    
            private:
                const BSpline & _mySpline;
                const float & _myStrokeSize;
        };

        class Normal {
            public:
                Normal(const BSpline & theSpline, Vector3<T> & theUpVector = Vector3<T>(0,0,1)) :
                    _mySpline(theSpline),
                    _myUpVector(theUpVector)
                {}

                Vector3<T> operator()(unsigned x, unsigned y) const {
                    const std::vector<Vector3<T> > & myPoints = _mySpline.getResult();
                    if (x >= myPoints.size()) {
                        throw asl::Exception(std::string("Invalid x value for bspline-normal: ") + 
                            as_string(x) + ". Must be less than " +
                            as_string(myPoints.size()), PLUS_FILE_LINE);
                    }
                    if (x == myPoints.size() - 1) {
                        x--;
                    }
                    Vector3<T> mySideVector;
                    Vector3<T> myUpVector;
                    _mySpline.getSideAndUpVector(myPoints[x + 1] - myPoints[x], mySideVector, myUpVector);
                    
                    return myUpVector;
                }

            private:
                const BSpline    & _mySpline;
                const Vector3<T> & _myUpVector;
        };

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
         * @param theStartHandle Start point anchor.
         * @param theEnd End point.
         * @param theEndHandle End point anchor.
         */
        void setControlPoints(const Vector3<T> & theStart,
                              const Vector3<T> & theStartHandle,
                              const Vector3<T> & theEnd,
                              const Vector3<T> & theEndHandle)
        {
            // setup polynom coefficients
            for (unsigned i = 0; i < 3; ++i) {
                _myCoeff[0][i] = theStart[i];
                _myCoeff[1][i] = 3 * (theStartHandle[i] - theStart[i]);
                _myCoeff[2][i] = 3 * (theEndHandle[i] - theStartHandle[i]) - _myCoeff[1][i];
                _myCoeff[3][i] = theEnd[i] - theStart[i] - _myCoeff[1][i] - _myCoeff[2][i];
            }
            
            clearResultCache();

            _myStart       = theStart;
            _myEnd         = theEnd;
            _myStartHandle = theStartHandle;
            _myEndHandle   = theEndHandle;
        }

        /**
         * Setup spline.
         * @param thePoints List of start, start-anchor, end, end-anchor points.
         */
        void setControlPoints(const std::vector< Vector3<T> > & thePoints) {
            setControlPoints(thePoints[0], thePoints[1],
                             thePoints[2], thePoints[3]);
        }

        /**
         * Setup spline from a set of points.
         * The points are named 'before', 'start', 'end', and 'past'.
         */
        void setupFromPoints(const std::vector< Vector3<T> > & thePoints, float theSize) {
            Vector3<T> myDir1 = normalized(thePoints[2] - thePoints[0]) * theSize;
            Vector3<T> myStartHandle = thePoints[1] + myDir1;

            Vector3<T> myDir2 = normalized(thePoints[1] - thePoints[3]) * theSize;
            Vector3<T> myEndHandle = thePoints[2] + myDir2;

            setControlPoints(thePoints[1], myStartHandle, thePoints[2], myEndHandle);
        }

        const Vector3<T> & getStart() const {
            return _myStart;
        }
        
        const Vector3<T> & getEnd() const {
            return _myEnd;
        }
        
        const Vector3<T> & getStartHandle() const {
            return _myStartHandle;
        }
        
        const Vector3<T> & getEndHandle() const {
            return _myEndHandle;
        }

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
         * Calculate first derivate at the given curve position.
         * @param theCurveParameter Position along the curve.
         * @param theEaseIn Ease-in. Range [0..1].
         * @param theEaseOut Ease-out. Range [0..1].
         * @return Normal at the given curve position.
         */
        Vector3<T> evaluateNormal(T theCurveParameter,
                                  T theEaseIn = 0, T theEaseOut = 0,
                                  const Vector3<T> & theUpVector = Vector3<T>(0,0,1)) const
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
            return getNormal(theCurveParameter, theUpVector);
        }

        /**
         * Calculates the arc length of the spline and caches the result.
         * @return The arc length.
         */
        T getArcLength() {
            if (_myResult.empty()) {
                throw asl::Exception("Could not get arc length, because spline is not calculated, yet.", PLUS_FILE_LINE);
            }                        

            // Distances are calculated lazily
            if (_myDistances.empty()) {
                _myDistances.resize(_myResult.size());
                
                _myDistances[0] = 0;
                for (unsigned i = 1; i < _myResult.size(); ++i) {
                    _myDistances[i] = _myDistances[i-1] + length(_myResult[i] - _myResult[i-1]);   
                }
            }
            
            return _myDistances.back();
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
            clearResultCache();
            _myResult.reserve(theResolution + 1);
            for (unsigned i = 0; i < theResolution + 1; ++i) {
                _myResult.push_back(evaluate(i / T(theResolution), theEaseIn, theEaseOut));
            }            

            return _myResult;
        }

        /// Get sampled points.
        const std::vector< Vector3<T> > & getResult() const {
            return _myResult;
        }

        void getSideAndUpVector(const Vector3<T> & theForwardVector,
                                Vector3<T> & theSideVector,
                                Vector3<T> & theUpVector) const
        {
            theSideVector = cross(theUpVector, theForwardVector);
            theSideVector = normalized(theSideVector);
            theUpVector = cross(theForwardVector, theSideVector);
            theUpVector = normalized(theUpVector);
        }
        
        Vector3<T> getPosition(T theDistance) {
            if (theDistance >= getArcLength()) {
                return _myResult.back();
            }
            
            // Find the last point that is within the given distance
            unsigned myIndex = 1;
            while (myIndex < _myDistances.size() && _myDistances[myIndex] < theDistance) {
                ++myIndex;
            }
            --myIndex;
                                
            T myRemainingDistance = theDistance - _myDistances[myIndex];            
            Vector3<T> myDirection = normalized(_myResult[myIndex+1] - _myResult[myIndex]);
            
            return _myResult[myIndex] + myDirection * myRemainingDistance;
        }
        
        bool isLineSegment() const {
            return almostEqual( _myStart, _myStartHandle) && almostEqual( _myEnd, _myEndHandle );
        }

    private:
        void clearResultCache() {
            _myResult.clear();
            _myDistances.clear();
        }
    
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

        // evaluate normal at 't'
        Vector3<T> getNormal(T t, const Vector3<T> & theUpVector) const {

            Vector3<T> myPos0, myPos1;
            if (t >= 1) {
                myPos0 = getValue(t - (T)0.01);
                myPos1 = getValue(t);
            } else {
                myPos0 = getValue(t);
                myPos1 = getValue(t + (T)0.01);
            }

            Vector3<T> myNormal = normalized(cross(theUpVector, myPos1 - myPos0));

            return myNormal;
        }
        
        Vector3<T> _myCoeff[4];
        
        Vector3<T> _myStart;
        Vector3<T> _myEnd;
        Vector3<T> _myStartHandle;
        Vector3<T> _myEndHandle;

        
        // The sampled points
        std::vector< Vector3<T> > _myResult;
        
        // The distance to each sampled point in _myResult from the spline start
        std::vector<T> _myDistances;         
    };

    typedef BSpline<float> BSplinef;
    typedef BSpline<double> BSplined;
    typedef Ptr<BSplinef> BSplinePtr; // a little bit dirty ... should be BSplinefPtr

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
