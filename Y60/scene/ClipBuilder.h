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
//   $Id: ClipBuilder.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: ClipBuilder.h,v $
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

#ifndef _ac_y60_ClipBuilder_h_
#define _ac_y60_ClipBuilder_h_

#include "BuilderBase.h"

#include <map>

namespace y60 {
    class AnimationBuilder;
    class ClipBuilder : public BuilderBase {
        public:
            ClipBuilder(const std::string & theName);
            virtual ~ClipBuilder();

            const std::string & appendAnimation(AnimationBuilder & theAnimation);
        private:
    };

    typedef asl::Ptr<ClipBuilder> ClipBuilderPtr;
    typedef std::map<std::string, ClipBuilderPtr> ClipBuilderMap;
}

#endif

