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
//   $Id: CharacterBuilder.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: CharacterBuilder.h,v $
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

#ifndef _ac_y60_CharacterBuilder_h_
#define _ac_y60_CharacterBuilder_h_

#include "BuilderBase.h"

namespace y60 {
    class ClipBuilder;
    class CharacterBuilder : public BuilderBase {
        public:
            CharacterBuilder(const std::string & theName);
            virtual ~CharacterBuilder();

            const std::string & appendClip(ClipBuilder & theClip);
        private:
    };
    typedef asl::Ptr<CharacterBuilder> CharacterBuilderPtr;
}

#endif // _ac_y60_CharacterBuilder_h_
