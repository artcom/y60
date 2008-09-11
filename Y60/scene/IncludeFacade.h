//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
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

#include <asl/base/Ptr.h>
#include "TransformHierarchyFacade.h"
#include <y60/base/NodeValueNames.h>

namespace y60 {
    //                  theTagName     theType      theAttributeName         theDefault
    DEFINE_ATTRIBUT_TAG(IncludeSrcTag, std::string, INCLUDE_SRC_ATTRIB, "");

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
