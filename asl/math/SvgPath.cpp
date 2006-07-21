//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "SvgPath.h"
#include "intersection.h"
#include "linearAlgebra.h"

#include <asl/Logger.h>
#include <algorithm>

namespace asl {

    SvgPath::SvgPath() {
        AC_DEBUG << "SvgPath::SvgPath";
        setup();
    }

    SvgPath::SvgPath(const std::string & thePathDefinition, float theSegmentLength)
    {
        AC_DEBUG << "SvgPath::SvgPath d='" << thePathDefinition << "' len=" << theSegmentLength;
        setup();
        if (thePathDefinition.size()) {
            parsePathDefinition(thePathDefinition);
        }
        _mySegmentLength = theSegmentLength;
    }

    SvgPath::SvgPath(const SvgPath & thePath) {
        AC_DEBUG << "SvgPath::SvgPath";
        assign(thePath);
    }

    SvgPath::~SvgPath()
    {
        AC_DEBUG << "SvgPath::~SvgPath";
        for (unsigned i = 0; i < _myElements.size(); ++i) {
            //delete _myElements[i];
        }
    }

    /// Move to position.
    void SvgPath::move(const Vector3f & thePos, bool theRelativeFlag)
    {
        Vector3f myEnd = (theRelativeFlag ? sum(_myLastPos, thePos) : thePos);;
        _myOrigin = myEnd;
        _myLastPos = myEnd;
        AC_TRACE << "move " << myEnd;
    }

    /// Line to position.
    void SvgPath::line(const Vector3f & thePos, bool theRelativeFlag)
    {
        Vector3f myEnd = (theRelativeFlag ? sum(_myLastPos, thePos) : thePos);
        push(new LineSegment<float>(_myLastPos, myEnd));
        _myLastPos = myEnd;
        AC_TRACE << "line " << myEnd;
    }

    /// Horizontal line to position.
    void SvgPath::hline(float theX, bool theRelativeFlag)
    {
        Vector3f myEnd = _myLastPos;
        if (theRelativeFlag) {
            myEnd[0] += theX;
        } else {
            myEnd[0] = theX;
        }
        push(new LineSegment<float>(_myLastPos, myEnd));
        _myLastPos = myEnd;
        AC_TRACE << "hline " << myEnd;
    }

    /// Vertical line to position.
    void SvgPath::vline(float theY, bool theRelativeFlag)
    {
        Vector3f myEnd = _myLastPos;
        if (theRelativeFlag) {
            myEnd[1] += theY;
        } else {
            myEnd[1] = theY;
        }
        push(new LineSegment<float>(_myLastPos, myEnd));
        _myLastPos = myEnd;
        AC_TRACE << "vline " << myEnd;
    }

    /// Cubic bezier spline.
    void SvgPath::cbezier(const Vector3f & theStartAnchor,
                          const Vector3f & theEndAnchor,
                          const Vector3f & theEnd,
                          bool theRelativeFlag)
    {
        Vector3f myStartAnchor = (theRelativeFlag ? sum(_myLastPos, theStartAnchor) : theStartAnchor);
        Vector3f myEndAnchor = (theRelativeFlag ? sum(_myLastPos, theEndAnchor) : theEndAnchor);
        Vector3f myEnd = (theRelativeFlag ? sum(_myLastPos, theEnd) : theEnd);
        push(new BSpline<float>(_myLastPos, myStartAnchor, myEnd, myEndAnchor));
        _myLastPos = myEnd;
    }

    /// Cubic bezier spline from points. 
    void SvgPath::cbezierFromPoints(const Vector3f & theBeforeStartPoint,
                                    const Vector3f & theStartPoint,
                                    const Vector3f & theEndPoint,
                                    const Vector3f & theAfterEndPoint,
                                    float theHandleSize,
                                    bool theRelativeFlag)
    {

        // calculate handles
        Vector3f myDir1 = normalized(theEndPoint - theBeforeStartPoint) * theHandleSize;
        Vector3f myDir2 = normalized(theStartPoint - theAfterEndPoint) * theHandleSize;
        Vector3f myStartAnchor = theStartPoint + myDir1;
        Vector3f myEndAnchor   = theEndPoint + myDir2;

        //setControlPoints(thePoints[1], myStartHandle, thePoints[2], myEndHandle);

        cbezier( myStartAnchor, myEndAnchor, theEndPoint, theRelativeFlag );
    }

    /// Close path to origin or position of last move.
    void SvgPath::close()
    {
        push(new LineSegment<float>(_myLastPos, _myOrigin));
        _myLastPos = _myOrigin;
        AC_TRACE << "close " << _myOrigin;
    }

    /// Revert path.
    void SvgPath::revert()
    {
        reverse(_myElements.begin(), _myElements.end());
        for (unsigned i = 0; i < _myElements.size(); ++i) {
            Vector3f myP = _myElements[i]->origin;
            _myElements[i]->origin = _myElements[i]->end;
            _myElements[i]->end = myP;
        }
    }

    /// Find point on path closest to position.
    SvgPath::PathPoint3f SvgPath::nearest(const Vector3f & thePos) const
    {
        PathPoint3f myPoint;
        float myNearestDistance = -1.0f;

        for (unsigned i = 0; i < _myElements.size(); ++i) {

            LineSegmentPtr myElement = _myElements[i];
            Vector3f myNearest = asl::nearest(*myElement, asPoint(thePos));
            float myDistance = distance(myNearest, thePos);
            if (myNearestDistance < 0.0f || myDistance < myNearestDistance) {
                myPoint.assign(myNearest);
                myPoint.path = this; // marks it as valid
                myPoint.element = i;
                myNearestDistance = myDistance;

                // abort if nearest distance is almost zero
                if (almostEqual(myNearestDistance, 0)) {
                    break;
                }
            }
        }

        return myPoint;
    }

    SvgPath::PathNormal SvgPath::normal(const Vector3f & thePos,
                                        const Vector3f & theUpVector) const
    {
        PathNormal myNormal;
        myNormal.nearest = nearest(thePos);
        if (myNormal.nearest.path) {
            LineSegmentPtr myElement = _myElements[myNormal.nearest.element];
            Vector3f myForward = myElement->end - myElement->origin;
            myNormal.normal = normalized(cross(theUpVector, myForward));
        }

        return myNormal;
    }

    SvgPath::PathPoint3f SvgPath::pointOnPath(float thePercentage) const
    {
        PathPoint3f myPoint;

        if (thePercentage <= 0.0f) {
            myPoint.assign(_myElements[0]->origin);
            myPoint.element = 0;
            myPoint.path = this;
        } else if (thePercentage >= 1.0f) {
            myPoint.assign(_myElements[_myElements.size()-1]->end);
            myPoint.element = _myElements.size()-1;
            myPoint.path = this;
        } else {

            float myPathLength = thePercentage * _myLength;
            float myAccumLength = 0.0f;

            for (unsigned i = 0; i < _myElements.size(); ++i) {

                LineSegmentPtr myElement = _myElements[i];
                Vector3f myElementVector = myElement->end - myElement->origin;
                float myElementLength = magnitude(myElementVector);

                if ((myAccumLength + myElementLength) >= myPathLength) {

                    float myRemainLength = myPathLength - myAccumLength;
                    myPoint.assign(sum(myElement->origin, product(normalized(myElementVector), myRemainLength)));
                    myPoint.element = i;
                    myPoint.path = this;
                    break;
                }
                myAccumLength += myElementLength;
            }
        }

        return myPoint;
    }

    std::vector<SvgPath::PathPoint3f> SvgPath::intersect(const LineSegment<float> & theElement) const
    {
        std::vector<SvgPath::PathPoint3f> myIsect;

        for (unsigned i = 0; i < _myElements.size(); ++i) {

            Point3f myResult;
            if (intersection(theElement, *(_myElements[i]), myResult)) {

                PathPoint3f myIsectPoint(this);
                myIsectPoint.assign(myResult);
                myIsectPoint.element = i;
                myIsect.push_back(myIsectPoint);
            }
        }

        return myIsect;
    }

    std::vector<SvgPath::PathPoint3f> SvgPath::intersect(const Line<float> & theElement) const
    {
        std::vector<SvgPath::PathPoint3f> myIsect;

        for (unsigned i = 0; i < _myElements.size(); ++i) {

            Point3f myResult;
            if (intersection(theElement, *(_myElements[i]), myResult)) {

                PathPoint3f myIsectPoint(this);
                myIsectPoint.assign(myResult);
                myIsectPoint.element = i;
                myIsect.push_back(myIsectPoint);
            }
        }

        return myIsect;
    }

    std::vector<SvgPath::PathPoint3f> SvgPath::intersect(const Sphere<float> & theElement) const
    {
        std::vector<SvgPath::PathPoint3f> myIsect;

        for (unsigned i = 0; i < _myElements.size(); ++i) {

            std::vector<Point3f> myResult;
            if (intersection(theElement, *(_myElements[i]), myResult)) {

                for (unsigned j = 0; j < myResult.size(); ++j) {
                    PathPoint3f myIsectPoint(this);
                    myIsectPoint.assign(myResult[j]);
                    myIsectPoint.element = i;
                    myIsect.push_back(myIsectPoint);
                }
            }
        }

        return myIsect;
    }

    SvgPath * SvgPath::createPerpendicularPath(const Vector3f & thePos,
                                               float theLength,
                                               const Vector3f & theUpVector) const
    {
        Vector3f myNearest = thePos;
        PathNormal myNormal = normal(myNearest, theUpVector);
        Vector3f myLeftVector = product(myNormal.normal, theLength);

        // setup path
        SvgPath * myPath = new SvgPath();
        myPath->move(myNormal.nearest);
        myPath->line(myLeftVector, true);

        return myPath;
    }

    /**
     * Create a new path along this path from start to end.
     * @param theStart Start position for sub-path.
     * @param theEnd End position for sub-path.
     * @param theShortestPathFlag If true then find the shortest path, otherwise the
     * sub-path is along the original path direction.
     * @return Path.
     */
    SvgPath * SvgPath::createSubPath(const Vector3f & theStart,
                                     const Vector3f & theEnd,
                                     bool theShortestPathFlag)
    {
        PathPoint3f myStartNearest = nearest(theStart);
        PathPoint3f myEndNearest = nearest(theEnd);
        if (!myStartNearest.valid() || !myEndNearest.valid()) {
            AC_ERROR << "Unable to find start/end point on path";
            return 0;
        }

        float myMaxLength = (theShortestPathFlag ? _myLength * 0.5f : -1.0f);
        SvgPath * myPath = createSubPath(myStartNearest, myEndNearest, myMaxLength);
        if (theShortestPathFlag && myPath == 0) {
            myPath = createSubPath(myEndNearest, myStartNearest, myMaxLength);
            if (!myPath) {
                AC_ERROR << "SvgPath::createSubPath unable to find shortest path";
            } else {
                AC_DEBUG << "Reverting path";
                myPath->revert();
            }
        }

        return myPath;
    }

    /**********************************************************************
     *
     * Private
     *
     **********************************************************************/

    void SvgPath::setup() {
        _myLength = 0.0f;
        _myLastPos = Vector3f(0.0f, 0.0f, 0.0f);
        _myOrigin = Vector3f(0.0f, 0.0f, 0.0f);
        _mySegmentLength = 2.0f;
        _myNumSegments = 2;
    }

    // deep copy
    void SvgPath::assign(const SvgPath & thePath) {
        AC_DEBUG << "SvgPath::assign";
        _myLength = thePath._myLength;
        _myLastPos = thePath._myLastPos;
        _myOrigin = thePath._myOrigin;
        _myCp0 = thePath._myCp0;
        _myCp1 = thePath._myCp1;
        _mySegmentLength = thePath._mySegmentLength;
        _myNumSegments = thePath._myNumSegments;
        for (unsigned i = 0; i < thePath._myElements.size(); ++i) {
            _myElements.push_back(thePath._myElements[i]);
        }
    }

    // push element
    void SvgPath::push(LineSegment<float> * theElement) {
        _myElements.push_back(LineSegmentPtr(theElement));
        _myLength += magnitude(difference(theElement->end, theElement->origin));
    }

    void SvgPath::push(BSpline<float> * theSpline) {
/*
        unsigned myNumSegments;
        if (_mySegmentLength <= 0.0f) {
            myNumSegments = _myNumSegments;
        } else {
            myNumSegments = (unsigned)(theSpline->getArcLength() / _mySegmentLength);
            if (myNumSegments < MIN_SPLINE_SEGMENTS) {
                myNumSegments = MIN_SPLINE_SEGMENTS;
            } else if (myNumSegments > MAX_SPLINE_SEGMENTS) {
                myNumSegments = MAX_SPLINE_SEGMENTS;
            }
        }
*/
        // [CH]: There is no good way to find out the curvature of the spline,
        //       so we just go for the best resolution.
        Point3f myP = theSpline->evaluate(0.0f);
        for (unsigned i = 1; i <= MAX_SPLINE_SEGMENTS; ++i) {
            Point3f myP1 = theSpline->evaluate(i / (float) MAX_SPLINE_SEGMENTS);
            push(new LineSegment<float>(myP, myP1));
            myP = myP1;
        }

        // no longer needed
        delete theSpline;
    }

    bool SvgPath::isCommand(char theChar) const {

        switch (tolower(theChar)) {
        case 'm':
        case 'l':
        case 'h':
        case 'v':
        case 't':
        case 'q':
        case 's':
        case 'c':
        case 'a':
        case 'z':
            return true;
        }
        return false;
    }

    bool SvgPath::isPrefix(char theChar) const {
        switch (theChar) {
        case '+':
        case '-':
            return true;
        }
        return false;
    }

    bool SvgPath::isWhiteSpace(char theChar) const {
        switch (theChar) {
        case ' ':
        case ',':
        case '\r':
        case '\n':
        case '\t':
            return true;
        }
        return false;
    }

    bool SvgPath::isSeparator(char theChar) const {
        return isPrefix(theChar) || isWhiteSpace(theChar) || isCommand(theChar);
    }

    void SvgPath::parsePathDefinition(const std::string & thePathDefinition) {

        std::vector<std::string> myParts = splitPathDefinition(thePathDefinition);
        renderPathParts(myParts);
    }

    std::vector<std::string> SvgPath::splitPathDefinition(const std::string & thePathDefinition) const {
        std::vector<std::string> myParts;
        std::string myPart;

        for (unsigned i = 0; i < thePathDefinition.size(); ++i) {

            char myChar = thePathDefinition[i];
            if (isSeparator(myChar)) {

                if (myPart.size()) {
                    myParts.push_back(myPart);
                    myPart.clear();
                }
                if (isCommand(myChar)) {
                    myParts.push_back(std::string(1, myChar));
                    continue;
                } else if (isWhiteSpace(myChar)) {
                    continue;
                }
            }

            myPart += myChar;
        }

        // push last part
        if (myPart.size()) {
            myParts.push_back(myPart);
        }

        return myParts;
    }

    SvgPath::SvgPathCommand SvgPath::_ourPathCommands[] = {
        { MOVE_TO, 'm', 2 },                    // xe,ye
        { LINE_TO, 'l', 2 },                    // xe,ye
        { VERTICAL_LINE, 'v', 1 },              // ye
        { HORIZONTAL_LINE, 'h', 1 },            // xe
        { SHORTHAND_QUADRATIC_BEZIER, 't', 2 }, // xe,ye
        { QUADRATIC_BEZIER, 'q', 4 },           // x_cp,y_cp, xe,ye
        { SHORTHAND_CUBIC_BEZIER, 's', 4 },     // x_cp1,y_cp1, xe,ye
        { CUBIC_BEZIER, 'c', 6 },               // x_cp0,y_cp0, x_cp1,y_cp1, xe,ye
        { ARC, 'a', 7 },
        { RETURN_TO_ORIGIN, 'z', 0 }
    };

    const SvgPath::SvgPathCommand * SvgPath::getSvgPathCommand(char theCommandChar) const {

        theCommandChar = tolower(theCommandChar);
        for (unsigned i = 0; i < (sizeof(_ourPathCommands) / sizeof(_ourPathCommands[0])); ++i) {
            if (_ourPathCommands[i].command == theCommandChar) {
                return &_ourPathCommands[i];
            }
        }
        return 0;
    }

    void SvgPath::renderPathParts(const std::vector<std::string> & theParts) {

        const SvgPathCommand * myLastCommand = 0;
        bool myRelativeFlag = false;

        for (unsigned i = 0; i < theParts.size(); ++i) {

            // current part must be a command else it repeats the last command
            char myChar = theParts[i][0];
            const SvgPathCommand * myCommand = getSvgPathCommand(myChar);
            if (myCommand == 0) {
                if (myLastCommand == 0) {
                    AC_ERROR << "Part '" << myChar << "' is not a command and can't repeat";
                    continue;
                }

                // repeat command
                myCommand = myLastCommand;
                AC_DEBUG << "Repeat command '" << myCommand->command << "'";
                --i; // must see this part
            } else {
                myRelativeFlag = (myChar >= 'a' && myChar <= 'z');
            }

            // fetch arguments
            std::vector<float> myArgs;
            for (unsigned j = 0; j < myCommand->numArgs; ++j) {
                myArgs.push_back(as<float>(theParts[++i]));
            }

            // render as elements
            renderPathCommand(myCommand, myArgs, myRelativeFlag);
            myLastCommand = myCommand;
        }
    }

    void SvgPath::renderPathCommand(const SvgPathCommand * theCommand,
            const std::vector<float> & theArgs,
            bool theRelativeFlag)
    {
        switch (theCommand->token) {
        case MOVE_TO:
            move(Vector3f(theArgs[0], theArgs[1], 0.0f), theRelativeFlag);
            break;
        case LINE_TO:
            line(Vector3f(theArgs[0], theArgs[1], 0.0f), theRelativeFlag);
            break;
        case VERTICAL_LINE:
            vline(theArgs[0], theRelativeFlag);
            break;
        case HORIZONTAL_LINE:
            hline(theArgs[0], theRelativeFlag);
            break;
        case SHORTHAND_QUADRATIC_BEZIER:
            // XXX this is wrong. Our BSpline is cubic.
            // mirror last Cp1
            _myCp1 = sum(_myLastPos, product(difference(_myCp1, _myLastPos), -1.0f));
            cbezier(_myCp1, _myCp1, Vector3f(theArgs[0], theArgs[1], 0.0f), theRelativeFlag);
            break;
        case QUADRATIC_BEZIER:
            // XXX this is wrong. Our BSpline is cubic.
            _myCp1 = Vector3f(theArgs[0], theArgs[1], 0.0f);
            cbezier(_myCp1, _myCp1, Vector3f(theArgs[2], theArgs[3], 0.0f), theRelativeFlag);
            break;
        case SHORTHAND_CUBIC_BEZIER:
            // mirror last cp1
            _myCp0 = sum(_myLastPos, product(difference(_myCp1, _myLastPos), -1.0f));
            _myCp1 = Vector3f(theArgs[0], theArgs[1], 0.0f);
            cbezier(_myCp0, _myCp1, Vector3f(theArgs[2], theArgs[3], 0.0f), theRelativeFlag);
            break;
        case CUBIC_BEZIER:
            _myCp0 = Vector3f(theArgs[0], theArgs[1], 0.0f);
            _myCp1 = Vector3f(theArgs[2], theArgs[3], 0.0f);
            cbezier(_myCp0, _myCp1, Vector3f(theArgs[4], theArgs[5], 0.0f), theRelativeFlag);
            break;
        case ARC:
            // XXX just so we're ending up in the right position
            line(Vector3f(theArgs[5], theArgs[6], 0.0f), theRelativeFlag);
            break;
        case RETURN_TO_ORIGIN:
            close();
            break;
        }
    }

    SvgPath * SvgPath::createSubPath(const PathPoint3f & theStart,
                                     const PathPoint3f & theEnd,
                                     float theMaxLength)
    {

        Vector3f myLastPos = theStart;
        unsigned myElementIndex = theStart.element;
        bool myFirstFlag = true;
        bool myLooped = false;

        SvgPath * myPath = new SvgPath;
        myPath->move(myLastPos);

        while (1) {

            LineSegmentPtr myElement = _myElements[myElementIndex];
            if (!myFirstFlag && !almostEqual(myElement->origin, myLastPos)) {
                // discontinuous
                myPath->move(myElement->origin);
            }
            myFirstFlag = false;

            if (myElementIndex == theEnd.element) {
                if (myElementIndex == theStart.element) {
                    // start/end on same element
                    if (myLooped == true) {
                        // been there, done that
                        break;
                    } else {
                        /*
                         * if start/end are on the same segment, determine if
                         * end point is 'behind' start point.
                         */
                        float myStartLen = magnitude(difference(static_cast<Vector3f>(theStart), myElement->origin));
                        float myEndLen = magnitude(difference(static_cast<Vector3f>(theEnd), myElement->origin));
                        if (myEndLen >= myStartLen) {
                            break;
                        } else {
                            myLooped = true;
                        }
                    }
                } else {
                    break;
                }
            }

            // draw to end of element
            myLastPos = myElement->end;
            myPath->line(myLastPos);
            myElementIndex = (myElementIndex + 1) % _myElements.size();

            // path exceeds length?
            if (theMaxLength > 0.0 && myPath->getLength() > theMaxLength) {
                delete myPath;
                return 0;
            }
        }

        myPath->line(theEnd);

        return myPath;
    }

    std::ostream & operator<<(std::ostream & os, const asl::SvgPath & thePath) {
        os << "[";
        for (unsigned i = 0; i < thePath.getNumElements(); ++i) {
            if (i > 0) {
                os << ",";
            }
            SvgPath::LineSegmentPtr myLineSegment = thePath.getElement(i);
            if (myLineSegment) {
                os << *myLineSegment;
            }
        }
        os << "]";
        return os;
    }
} // namespace
