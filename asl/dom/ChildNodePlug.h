/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
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
#include <asl/base/Exception.h>

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

            ChildNodePlug(FACADE * theFacade) 
			{
                AC_TRACE << "ChildNodePlug::ChildNodePlug()" ;
                theFacade->registerChildName(TAG::getName());
			}

			const VALUE & getValue(const Node & theNode) const {
                if (!theNode) {
                    throw Facade::NoParentNode(std::string(TAG::getName()),PLUS_FILE_LINE);
                }
                return *Facade::ensureChild(theNode, TAG::getName());
			}
			void ensureDependencies() const {}

			const NodePtr getChildNode(const Node & theNode) const { 
                return Facade::ensureChild(theNode, TAG::getName());
			}
			NodePtr getChildNode(const Node & theNode) { 
                return Facade::ensureChild(theNode, TAG::getName());
			}
				
		private:
            ChildNodePlug() {};

            NodePtr ensureChild(const Node & theNode) const {
 				NodePtr myChild = theNode.childNode(TAG::getName());
                if (!myChild) {
                    myChild = createChild(theNode);
                }
                return myChild;
            }
            NodePtr createChild(const Node & theNode) const {
                try {
                    return const_cast<Node&>(theNode).appendChild(NodePtr(new Element(TAG::getName())) );
                } catch (asl::Exception & ex) {
                    throw Facade::Exception(std::string("Could not add child '") + TAG::getName() +
                            "' to node:\n" + asl::as_string(theNode) + asl::as_string(ex), PLUS_FILE_LINE);
                }
            }
    };
}

#endif
