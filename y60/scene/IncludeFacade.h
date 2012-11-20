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
//
//   $RCSfile: $
//
//   $Author: $
//
//   $Revision: $
//
//=============================================================================

#ifndef _ac_scene_IncludeFacade_h_
#define _ac_scene_IncludeFacade_h_

#include "y60_scene_settings.h"

#include <asl/base/Ptr.h>
#include "TransformHierarchyFacade.h"
#include <y60/base/NodeValueNames.h>

namespace y60 {
    //                  theTagName     theType      theAttributeName         theDefault
    DEFINE_ATTRIBUTE_TAG(IncludeSrcTag, std::string, INCLUDE_SRC_ATTRIB, "", Y60_SCENE_DECL);

/**
 * @ingroup y60scene
 * Facade for the include node. Include nodes are used to include an
 * external x60 file into the current dom.
 *
 */
class IncludeFacade :
    public TransformHierarchyFacade,
    public IncludeSrcTag::Plug
{
    public:
        IncludeFacade(dom::Node & theNode) : TransformHierarchyFacade(theNode),
            IncludeSrcTag::Plug(theNode)
        {
            if (theNode.childNodesLength()) {
                _myPreviousSource = theNode.getAttributeString(INCLUDE_SRC_ATTRIB);
            } else {
                _myPreviousSource = "";
            }
        }

        IMPLEMENT_FACADE(IncludeFacade);
        void load();

        /**
         * @retval true the file given in src is included as child of this node and all
         * its materials, shapes, ... are merged into the scene
         * @retval false the file given in src is not yet included as child of this node.
         * Dangling nodes should be collected from the scene and an import should be called
         * for the src of this include node.
         */
        bool isUpToDate() const {
            return (_myPreviousSource == getNode().getAttributeString(INCLUDE_SRC_ATTRIB));
        }
        /**
         * sets the node to be up to date. This means that the include file given in the src
         * attribute will not be included in future passes since it is assumed to be up to date.
         */
        void setUpToDate() {
            _myPreviousSource = getNode().getAttributeString(INCLUDE_SRC_ATTRIB);
        }
    private:
        std::string _myPreviousSource;

};
typedef asl::Ptr<IncludeFacade, dom::ThreadingModel> IncludeFacadePtr;
}

#endif // _ac_scene_IncludeFacade_h_
