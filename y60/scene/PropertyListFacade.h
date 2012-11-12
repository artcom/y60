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
//   $RCSfile: MaterialBase.h,v $
//   $Author: christian $
//   $Revision: 1.32 $
//   $Date: 2005/03/24 23:36:03 $
//
//   Description:
//
//=============================================================================

#ifndef _ac_scene_PropertyListFacade_h_
#define _ac_scene_PropertyListFacade_h_

#include "y60_scene_settings.h"

#include <y60/base/PropertyNames.h>
#include <asl/dom/Facade.h>
#include <string>
#include <vector>

namespace y60 {
    class Y60_SCENE_DECL PropertyListFacade : public dom::Facade {
        public:
            PropertyListFacade(dom::Node & theNode) : dom::Facade(theNode), _myNodeVersion(0) {}
            void ensureProperties() const {
                if (getNode().nodeVersion() != _myNodeVersion) {
                    _myPropertyNodes.clear();
                    unsigned mySize = getNode().childNodesLength();
                    for (unsigned i = 0; i < mySize; ++i) {
                        dom::NodePtr myNode = getNode().childNode(i);
                        if (myNode->nodeType() == dom::Node::ELEMENT_NODE &&
                            myNode->nodeName() != "#comment")
                        {
                            std::string myName = myNode->getAttributeString(NAME_ATTRIB);
                            if (_myPropertyNodes.find(myName) == _myPropertyNodes.end()) {
                                _myPropertyNodes[myName] = myNode->childNode("#text");
                            }
                        }
                    }
                    _myNodeVersion = getNode().nodeVersion();
                }
            }

        private:
            mutable unsigned long long _myNodeVersion;
    };
}

#endif
