//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _y60_SvgPath_h_
#define _y60_SvgPath_h_

#include "Vector234.h"
#include "BSpline.h"
#include "Line.h"
#include "Sphere.h"

#include <asl/Ptr.h>
#include <vector>

namespace asl {

    /**
     * SvgPath class.
     * Based on SvgPath.js code. [Rev 5376]
     */
    class SvgPath {
    public:
        /**
         * Point-on-path.
         * IS-A Vector3f that keeps tab on which path it is on.
         */
        class PathPoint3f : public Vector3f {
        public:
            PathPoint3f(const SvgPath * thePath = 0) : Vector3f(), path(thePath), element(0) {}
            const SvgPath * path;
            unsigned element;

            bool valid() const {
                return (path != 0);
            }
        };

        /**
         * Path normal.
         */
        struct PathNormal {
            Vector3f normal;
            PathPoint3f nearest;
        };

        /// LineSegment.
        typedef Ptr<LineSegment<float> > LineSegmentPtr;

        /// Constructor.
        SvgPath();
        SvgPath(const std::string & thePathDefinition, float theSegmentLength = 16.0f);
        SvgPath(const SvgPath & thePath);

        /// Destructor.
        ~SvgPath();

        /// Assignment operator.
        SvgPath & operator=(const SvgPath & thePath) {
            assign(thePath);
            return *this;
        }

        /// Move to position.
        void move(const Vector3f & thePos, bool theRelativeFlag = false);

        /// Line to position.
        void line(const Vector3f & thePos, bool theRelativeFlag = false);

        /// Horizontal line to position.
        void hline(float theX, bool theRelativeFlag = false);

        /// Vertical line to position.
        void vline(float theY, bool theRelativeFlag = false);

        /// Cubic bezier spline from anchors and end point.
        void cbezier(const Vector3f & theStartAnchor,
                     const Vector3f & theEndAnchor,
                     const Vector3f & theEnd,
                     bool theRelativeFlag = false);

        /// Cubic bezier spline from points.
        void cbezierFromPoints(const Vector3f & theBeforeStartPoint,
                               const Vector3f & theStartPoint,
                               const Vector3f & theEndPoint,
                               const Vector3f & theAfterEndPoint,
                               float theHandleSize,
                               bool theRelativeFlag = false);

        /// Close path to origin or position of last move.
        void close();

        /// Revert path.
        void revert();

        /// Set segment length for BSpline segmentation.
        void setSegmentLength(float theLength) {
            _mySegmentLength = theLength;
        }

        /// Get segment length.
        float getSegmentLength() const {
            return _mySegmentLength;
        }

        /// Set number of segments for BSpline segmentation.
        void setNumSegments(unsigned theNum) {
            _myNumSegments = theNum;
            _mySegmentLength = -1.0f;
        }

        /// Get number of segments.
        unsigned getNumSegments() const {
            return _myNumSegments;
        }

        /// Return length of path.
        float getLength() const {
            return _myLength;
        }

        /// Get number of path elements.
        unsigned getNumElements() const {
            return _myElements.size();
        }

        /// Get element.
        LineSegmentPtr getElement(unsigned theIndex) const {
            if (theIndex >= 0 && theIndex < _myElements.size()) {
                return _myElements[theIndex];
            }
            return LineSegmentPtr(0);
        }

        /// Find point on path closest to position.
        PathPoint3f nearest(const Vector3f & thePos) const;

        /**
         * Return left-vector of path at given position.
         * @param thePos Position.
         * @param theUpVector Up-vector at intersection.
         * @return Normal and nearest point.
         */
        PathNormal normal(const Vector3f & thePos,
                          const Vector3f & theUpVector = Vector3f(0,0,1)) const;

        /// Return point on path by percentage of path length.
        PathPoint3f pointOnPath(float thePercentage) const;

        /**
         * Intersect path with given element.
         * @param theElement Element to intersect with.
         * @return List of intersection points.
         */
        std::vector<PathPoint3f> intersect(const LineSegment<float> & theElement) const;
        std::vector<PathPoint3f> intersect(const Line<float> & theElement) const;
        std::vector<PathPoint3f> intersect(const Sphere<float> & theElement) const;

        /**
         * Create a new path that is perpendicular to this path at the given position.
         * @param thePos Position to originate the new path.
         * @param theLength Length of new path.
         * @param theUpVector Up-vector at intersection.
         * @return Path.
         */
        SvgPath * createPerpendicularPath(const Vector3f & thePos,
                                          float theLength,
                                          const Vector3f & theUpVector = Vector3f(0,0,1)) const;

        /**
         * Create a new path along this path from start to end.
         * @param theStart Start position for sub-path.
         * @param theEnd End position for sub-path.
         * @param theShortestPathFlag If true then find the shortest path, otherwise the
         * sub-path is along the original path direction.
         * @return Path.
         */
        SvgPath * createSubPath(const Vector3f & theStart,
                                const Vector3f & theEnd,
                                bool theShortestPathFlag = false);

    private:
        // path
        float _myLength;
        std::vector<LineSegmentPtr> _myElements;

        // state
        Vector3f _myLastPos;
        Vector3f _myOrigin;
        Vector3f _myCp0;
        Vector3f _myCp1;

        // BSpline segmentation
        float _mySegmentLength;
        unsigned _myNumSegments;
        enum {
            MIN_SPLINE_SEGMENTS = 4,
            MAX_SPLINE_SEGMENTS = 32
        };

        // SVG commands
        enum CommandToken {
            MOVE_TO = 0,
            LINE_TO,
            VERTICAL_LINE,
            HORIZONTAL_LINE,
            SHORTHAND_QUADRATIC_BEZIER,
            QUADRATIC_BEZIER,
            SHORTHAND_CUBIC_BEZIER,
            CUBIC_BEZIER,
            ARC,
            RETURN_TO_ORIGIN
        };

        struct SvgPathCommand {
            CommandToken token;
            char command;
            unsigned numArgs;
        };

        static SvgPathCommand _ourPathCommands[];

        // object setup
        void setup();

        // assignment
        void assign(const SvgPath & thePath);

        // push element
        void push(LineSegment<float> * theElement);
        void push(BSpline<float> * theSpline);

        // 'true' if theChar is a supported command
        bool isCommand(char theChar) const;

        // 'true' if theChar is a number prefix. (Prefixes act as separators.)
        bool isPrefix(char theChar) const;

        // 'true' if theChar is a whitespace
        bool isWhiteSpace(char theChar) const;

        // 'true' if theChar is any kind of separator
        bool isSeparator(char theChar) const;

        // parse path definition
        void parsePathDefinition(const std::string & thePathDefinition);

        // split path definition into parts
        std::vector<std::string> splitPathDefinition(const std::string & thePathDefinition) const;

        // fetch SVG path command definition
        const SvgPathCommand * getSvgPathCommand(char theCommand) const;

        // render path parts into elements
        void renderPathParts(const std::vector<std::string> & theParts);

        // render path command into elements
        void renderPathCommand(const SvgPathCommand * theCommand,
                               const std::vector<float> & theArgs,
                               bool theRelativeFlag);

        /**
         * Create path from 'theStart' to 'theEnd' and
         * abort when the created path length exceeds 'theMaxLength'.
         */
        SvgPath * createSubPath(const PathPoint3f & theStart,
                                const PathPoint3f & theEnd,
                                float theMaxLength = -1.0f);
    };

    std::ostream & operator<<(std::ostream & os, const asl::SvgPath & thePath);

} // namespace

#endif
