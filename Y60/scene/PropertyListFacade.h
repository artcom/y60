//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

#include <y60/PropertyNames.h>
#include <dom/Facade.h>
#include <string>
#include <vector>

namespace y60 {
	class PropertyListFacade : public dom::Facade {
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
