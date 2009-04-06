/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below. 
//    
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
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
//   $RCSfile: Animation.h,v $
//   $Author: martin $
//   $Revision: 1.25 $
//   $Date: 2005/02/22 16:40:01 $
//
//  Description: A simple animation class.
//
//=============================================================================

#ifndef _ac_scene_Animation_h_
#define _ac_scene_Animation_h_

#include "y60_scene_settings.h"

#include "AnimationBase.h"
#include <y60/base/NodeNames.h>
#include <y60/base/DataTypes.h>

#include <asl/base/Exception.h>
#include <asl/base/string_functions.h>
#include <asl/math/Frustum.h>

#include <asl/dom/Nodes.h>
#include <asl/base/begin_end.h>

#include <string>
#include <math.h>

namespace asl {
    inline
    const std::string & interpolate(const std::string & a, const std::string & b, const double & theAlpha) {
        if (theAlpha <= 0.5) {
            return a;
        } else {
            return b;
        }
    }

    inline
    asl::Quaternionf interpolate(const asl::Quaternionf & a, const asl::Quaternionf & b, const double & theAlpha) {
        return slerp(a , b, float(theAlpha)); 
    }

}

namespace y60 {

    template <class T, class ANIMATED_ATTRIBUTE_VALUE>
    class AnimationValueSetter {
        public:
        AnimationValueSetter(dom::NodePtr theAnimatedValue, unsigned theAnimatedIndex) 
            : _myAnimatedAttribute(theAnimatedValue),
              _myAnimatedIndex(theAnimatedIndex)
        {
            const ANIMATED_ATTRIBUTE_VALUE & myAnimatedAttributeValue = 
                _myAnimatedAttribute->dom::Node::nodeValueRef<ANIMATED_ATTRIBUTE_VALUE>();
            _myPreviousValue  = myAnimatedAttributeValue[_myAnimatedIndex];
        }
        void setValue(const T & theNewValue) {
                if (!(theNewValue == _myPreviousValue)) {
                    dom::Node::WritableValue<ANIMATED_ATTRIBUTE_VALUE> myAnimatedAttributeLock(_myAnimatedAttribute);
                    ANIMATED_ATTRIBUTE_VALUE & myAnimatedAttributeValue = myAnimatedAttributeLock.get();
                    myAnimatedAttributeValue[_myAnimatedIndex] = theNewValue;
                    _myPreviousValue  = theNewValue;
                }
            }

         private:
            dom::NodePtr     _myAnimatedAttribute;
            unsigned         _myAnimatedIndex;
            T                _myPreviousValue;
    };

    template <>
    class AnimationValueSetter<float, asl::Frustum> {
        public:
        AnimationValueSetter(dom::NodePtr theAnimatedValue, unsigned) 
            : _myAnimatedAttribute(theAnimatedValue)
        {
            const asl::Frustum & myAnimatedAttributeValue = 
                    _myAnimatedAttribute->dom::Node::nodeValueRef<asl::Frustum>();
            _myPreviousValue  = myAnimatedAttributeValue.getHFov();
        }
        void setValue(const float & theNewValue) {
            if (!(theNewValue == _myPreviousValue)) {
                dom::Node::WritableValue<asl::Frustum> myAnimatedAttributeLock(_myAnimatedAttribute);
                asl::Frustum & myAnimatedAttributeValue = myAnimatedAttributeLock.get();
                myAnimatedAttributeValue.setHFov( theNewValue );
                _myPreviousValue  = theNewValue;
            }
        }

         private:
            dom::NodePtr     _myAnimatedAttribute;
            float            _myPreviousValue;
    };

    template<>
    class AnimationValueSetter<AcBool, AcBool> {
        public:
        AnimationValueSetter(dom::NodePtr theAnimatedValue, unsigned) 
            : _myAnimatedAttribute(theAnimatedValue)
        {
            const bool & myAnimatedAttributeValue = 
                _myAnimatedAttribute->dom::Node::nodeValueRef<bool>();
            _myPreviousValue  = myAnimatedAttributeValue;
        }
        void setValue(const AcBool & theNewValue) {
            if (!(theNewValue == _myPreviousValue)) {
                dom::Node::WritableValue<bool> myAnimatedAttributeLock(_myAnimatedAttribute);
                bool & myAnimatedAttributeValue = myAnimatedAttributeLock.get();
                myAnimatedAttributeValue = theNewValue;
                _myPreviousValue  = theNewValue;
            }
        }

         private:
            dom::NodePtr  _myAnimatedAttribute;
            bool          _myPreviousValue;
    };

    template <class ANIMATED_ATTRIBUTE_VALUE>
    class AnimationValueSetter<ANIMATED_ATTRIBUTE_VALUE,ANIMATED_ATTRIBUTE_VALUE> {
        public:
        AnimationValueSetter(dom::NodePtr theAnimatedValue, unsigned) 
            : _myAnimatedAttribute(theAnimatedValue)
        {
            const ANIMATED_ATTRIBUTE_VALUE & myAnimatedAttributeValue = 
                _myAnimatedAttribute->dom::Node::nodeValueRef<ANIMATED_ATTRIBUTE_VALUE>();
            _myPreviousValue  = myAnimatedAttributeValue;
        }
        void setValue(const ANIMATED_ATTRIBUTE_VALUE & theNewValue) {
                if (!(theNewValue == _myPreviousValue)) {
                    dom::Node::WritableValue<ANIMATED_ATTRIBUTE_VALUE> myAnimatedAttributeLock(_myAnimatedAttribute);
                    ANIMATED_ATTRIBUTE_VALUE & myAnimatedAttributeValue = myAnimatedAttributeLock.get();
                    myAnimatedAttributeValue = theNewValue;
                    _myPreviousValue  = theNewValue;
                }
            }

         private:
            dom::NodePtr             _myAnimatedAttribute;
            ANIMATED_ATTRIBUTE_VALUE _myPreviousValue;
     };


    template <class T, class ANIMATED_ATTRIBUTE_VALUE = T>
    class Animation : public AnimationBase {
        public:
            Animation(dom::NodePtr theNode, dom::NodePtr theValueList, dom::NodePtr theAnimatedAttribute, unsigned theAnimatedIndex, bool theAngleAnimation)
                : AnimationBase(theNode, theAngleAnimation),
                _myValuesNode(theValueList->childNode(0)),
                _myValueSetter(theAnimatedAttribute, theAnimatedIndex)
            {}
            
            Animation(dom::NodePtr theNode, dom::NodePtr theValueList, dom::NodePtr theAnimatedAttribute, bool theAngleAnimation)
                : AnimationBase(theNode, theAngleAnimation),
                _myValuesNode(theValueList->childNode(0)),
                _myValueSetter(theAnimatedAttribute, 0)
            {}

            virtual ~Animation() {}

            bool run(double theTime) {
                int     myLowerIndex = 0;
                int     myUpperIndex = 0;
                double  myAlpha = 0;

                const std::vector<T> & myValues = _myValuesNode->dom::Node::nodeValueRef<std::vector<T> >();
                unsigned int myResult = calcIndicies(myLowerIndex, myUpperIndex, myAlpha,
                                                     theTime, myValues.size());
                if (myResult == 0) {
                    return true;
                } else if (myResult == 1) {
                    setToEndValue();
                    return false;
                }
                _myValueSetter.setValue(asl::interpolate(myValues[myLowerIndex],
                            myValues[myUpperIndex], myAlpha));
                return true;
            }

        private:
            Animation();

            dom::NodePtr          _myValuesNode;
            AnimationValueSetter<T, ANIMATED_ATTRIBUTE_VALUE> _myValueSetter;

            void setToEndValue() {
                // TODO: Account for clipping.
                const std::vector<T> & myValues = _myValuesNode->dom::Node::nodeValueRef<std::vector<T> >();
                if (getCurrentDirection() == FORWARD) {
                    _myValueSetter.setValue(myValues[myValues.size() - 1]);
                } else {
                    _myValueSetter.setValue(myValues[0]);
                }
            }
    };

    template <class T>
    bool
    axisFlip(const T & theParam1, const T & theParam2) {
        return false;
    }

    inline bool
    axisFlip(const float & theParam1, const float & theParam2) {
        return (fabs(theParam1 - theParam2) >= 1.5);
    }

    inline bool
    axisFlip(const double & theParam1, const double & theParam2) {
        return (fabs(theParam1 - theParam2) >= 1.5);
    }

    inline bool
    axisFlip(const long double & theParam1, const long double & theParam2) {
        return (fabs(theParam1 - theParam2) >= 1.5);
    }
}

#endif

