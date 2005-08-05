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
//   $Id: JointBuilder.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: JointBuilder.h,v $
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

#ifndef AC_SOM_JOINT_BUILDER_INCLUDED
#define AC_SOM_JOINT_BUILDER_INCLUDED

#include "TransformBuilderBase.h"

namespace y60 {

    class JointBuilder : public TransformBuilderBase {
        public:
            JointBuilder(const std::string & theName);
            virtual ~JointBuilder() {};

            void setJointOrientation(const asl::Vector3f & theOrientation);
            void setJointOrientation(const asl::Quaternionf & theJointOrientation);
        private:
    };

    typedef asl::Ptr<JointBuilder> JointBuilderPtr;

}

#endif //  AC_SOM_TRANSFORM_BUILDER_INCLUDED
