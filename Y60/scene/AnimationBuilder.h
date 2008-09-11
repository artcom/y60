//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: AnimationBuilder.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: AnimationBuilder.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: XML-File-Export Plugin
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_y60_AnimationBuilder_h_
#define _ac_y60_AnimationBuilder_h_

#include "BuilderBase.h"
#include <y60/base/DataTypes.h>
#include <y60/base/NodeValueNames.h>
#include <y60/base/TypeTraits.h>
#include <asl/dom/Nodes.h>

namespace y60 {
    class AnimationBuilder : public BuilderBase {
        public:
            AnimationBuilder();
            virtual ~AnimationBuilder();

            void setName(const std::string & theName);
            void setNodeRef(const std::string & theNodeRef);
            void setAttribute(const std::string & theAttribute);
            void setProperty(const std::string & theProperty);
            void setDuration(float theDuration);
            void setEnable(bool theEnable);
            void setBegin(float theBegin);
            void setPause(float thePause);
            void setCount(unsigned theCount);
            void setDirection(AnimationDirection theDirection);

            void appendValues(const VectorOfString & theValues);

            template <class T>
            void appendValues(const std::vector<T> & theValues);
        private:
    };

    template <class T>
    void AnimationBuilder::appendValues(const std::vector<T> & theValues) {
        (*getNode())(VectorTypeNameTrait<T>::name()).appendChild(
            dom::Text(dom::ValuePtr(new dom::VectorValue<std::vector<T>, dom::MakeResizeableVector>(theValues,0))));
    }

    typedef asl::Ptr<AnimationBuilder> AnimationBuilderPtr;
}

#endif
