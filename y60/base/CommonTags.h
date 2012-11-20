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
*/

#ifndef _xml_CommonTags_h_
#define _xml_CommonTags_h_

#include "y60_base_settings.h"

#include "DataTypes.h"
#include "NodeNames.h"
#include <asl/dom/AttributePlug.h>
#include <asl/math/Frustum.h>
#include <asl/math/Box.h>
#include <asl/base/Singleton.h>
#include <asl/base/string_functions.h>

namespace y60 {

    //                  theTagName       theType        theAttributeName      theDefault
    DEFINE_ATTRIBUTE_TAG(NameTag,         std::string,   NAME_ATTRIB,     "JohnDoe",              Y60_BASE_DECL);
    //struct NameTag {
    //    typedef std::string TYPE;
    //    static const char * getName() { return NAME_ATTRIB; }
    //    static const TYPE getDefault() { return "JohnDoe"; }
    //    template class Y60_BASE_DECL dom::AttributePlug<NameTag>;
    //    typedef dom::AttributePlug<NameTag> Plug;
    //};

    DEFINE_ATTRIBUTE_TAG(VisibleTag,      bool,          VISIBLE_ATTRIB,  true,                   Y60_BASE_DECL);
    DEFINE_ATTRIBUTE_TAG(Position2DTag,   asl::Vector2f, POSITION_ATTRIB, asl::Vector2f(0.f,0.f), Y60_BASE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(LastActiveFrameTag, asl::Unsigned64, "lastActiveFrame", 0, Y60_BASE_DECL);

    // used by textures and viewports
    DEFINE_ATTRIBUTE_TAG(ResizePolicyTag, asl::ResizePolicy, RESIZE_POLICY_ATTRIB, asl::ADAPT_VERTICAL, Y60_BASE_DECL);

    struct Y60_BASE_DECL IdTag : public asl::Singleton<IdTag> {
        typedef std::string TYPE;
        ASL_DOM_EXPORT_STATICS(dom::AttributePlug,IdTag,Y60_BASE_DECL) \
        typedef dom::AttributePlug<IdTag> Plug;
        static const char * getName();
        static const TYPE getDefault();
        unsigned counter;
        unsigned myStartTime;
        ATTRIBUTE_TAG_NO_DEFAULT_VALUE();
        IdTag();
    };

    struct BoundingBoxTag {
        typedef asl::Box3f TYPE;
        ASL_DOM_EXPORT_STATICS(dom::FacadeAttributePlug,BoundingBoxTag,Y60_BASE_DECL) \
        typedef dom::FacadeAttributePlug<BoundingBoxTag> Plug;
        static const char * getName() { return BOUNDING_BOX_ATTRIB; }
        static const TYPE getDefault() {
            asl::Box3f myEmptyBox;
            myEmptyBox.makeEmpty();
            return myEmptyBox;
        }
        ATTRIBUTE_TAG_NO_DEFAULT_VALUE();
    };
}

#endif
