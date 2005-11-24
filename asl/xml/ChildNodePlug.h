/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2003, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//   $RCSfile: ChildNodePlug.h,v $
//
//   $Revision: 1.15 $
//
//   Description:
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _xml_ChildNodePlug_h_
#define _xml_ChildNodePlug_h_

#include "Nodes.h"
#include <asl/Exception.h>

namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

#define DEFINE_CHILDNODE_TAG(theTagName, theFacade, theChildFacade, theName) \
    class theFacade; \
    struct theTagName { \
		typedef theFacade FACADE; \
		typedef theChildFacade CHILDFACADE; \
		typedef dom::Node TYPE; \
        typedef dom::ChildNodePlug<theTagName, theFacade> Plug; \
        static const char * getName() { return theName; } \
    };

	template<class TAG, class FACADE>
    class ChildNodePlug {
        public:
			typedef typename TAG::TYPE VALUE;

            ChildNodePlug(FACADE * theFacade) : _myChild(0) 
			{
                ensureChild(theFacade->getNode());
                if (_myChild) {
                    theFacade->appendChild(_myChild);
                } else {
                    theFacade->appendChild(NodePtr(new Element(TAG::getName())));
                }
			}

			const VALUE & getValue(const Node & theNode) const {
                if (!_myChild) {
                    ensureChild(theNode);
                }
				return *_myChild;
			}
			void ensureDependencies() const {}

			const NodePtr getChildNode(const Node & theNode) const { 
                if (!_myChild) {
                    ensureChild(theNode);
                }
				return _myChild; 
			}
			
		private:
            mutable NodePtr _myChild;
            ChildNodePlug() {};

            void ensureChild(const Node & theNode) const {
                if (!theNode) {
                    return; // to allow factory nodes
                }
				_myChild = theNode.childNode(TAG::getName());
                if (!_myChild) {
                    try {
                        _myChild = const_cast<Node&>(theNode).appendChild(NodePtr(new Element(TAG::getName())) );
					} catch (asl::Exception) {
                        throw Facade::Exception(std::string("Could not add child '") + TAG::getName() +
                            "' to node:\n" + asl::as_string(theNode), PLUS_FILE_LINE);
                    }
                }
            }
	
    };
}

#endif
