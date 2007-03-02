/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//   $RCSfile: CommonTags.h,v $
//
//   $Revision: 1.1 $
//
//   Description:
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _xml_CommonTags_h_
#define _xml_CommonTags_h_

#include "NodeNames.h"
#include <dom/AttributePlug.h>
#include <asl/Box.h>
#include <asl/Singleton.h>
#include <asl/string_functions.h>

namespace y60 {

    //                  theTagName       theType        theAttributeName      theDefault
    DEFINE_ATTRIBUT_TAG(NameTag,         std::string,   NAME_ATTRIB,     "JohnDoe");
    DEFINE_ATTRIBUT_TAG(VisibleTag,      bool,          VISIBLE_ATTRIB,  true);
    DEFINE_ATTRIBUT_TAG(Position2DTag,   asl::Vector2f, POSITION_ATTRIB, asl::Vector2f(0.f,0.f));

    struct IdTag : public asl::Singleton<IdTag> {
        typedef std::string TYPE;
        typedef dom::AttributePlug<IdTag> Plug;
        IdTag();
        static const char * getName();
        static const TYPE getDefault();
        unsigned counter;
        unsigned myStartTime;
    };

    struct BoundingBoxTag {
        typedef asl::Box3f TYPE;
        typedef dom::AttributePlug<BoundingBoxTag> Plug;
        static const char * getName() { return BOUNDING_BOX_ATTRIB; }
        static const TYPE getDefault() {
            asl::Box3f myEmptyBox;
            myEmptyBox.makeEmpty();
            return myEmptyBox;
        }
    };
}

#endif
