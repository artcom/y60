//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.

#ifndef _ac_dom_PropertyPlug_h_
#define _ac_dom_PropertyPlug_h_

#include <dom/Nodes.h>
#include "PlugHelper.h"
//#include "AttributePlug.h"

namespace dom {


#define DEFINE_PROPERTY_TAG(theTagName, theFacade, theType, theNodeName, thePropertyName, \
        thePropertyListName, theDefault) \
    class theFacade; \
    struct theTagName { \
        typedef theType TYPE; \
        typedef dom::PropertyPlug<theTagName, theFacade> Plug; \
        static const char * getNodeName() { return theNodeName; } \
        static const char * getName() { return thePropertyName; } \
        static const char * getListName() { return thePropertyListName; } \
		static const TYPE getDefault() { return theDefault; } \
    };


    template<class TAG, class FACADE>
    class PropertyPlug {
        public:
            typedef typename TAG::TYPE VALUE;
            typedef typename ValueWrapper<VALUE>::Type WRAPPER;

            PropertyPlug(FACADE * theFacade) : _myTextChild(0), _myPropertyNode(0), _myFacade(theFacade)
            {
                ensureTextChild(theFacade->getNode());
            }

            ValuePtr getValuePtr() {
                return _myTextChild->nodeValueWrapperPtr();
            }
            
            const VALUE & getValue(const Node & theNode) const {
                ensureTextChild(theNode);
                return ValueHelper<VALUE, WRAPPER>::getValue(_myTextChild->nodeValueWrapperPtr());
            }

            const VALUE & setValue(Node & theNode, const VALUE & theValue) {
                ensureTextChild(theNode, theValue);
                return ValueHelper<VALUE, WRAPPER>::setValue(_myTextChild->nodeValueWrapperPtr(), theValue);
            }

            unsigned long long getVersion(const Node & theNode) const {
                ensureTextChild(theNode);
                return _myTextChild->nodeVersion();
            }
            bool hasOutdatedDependencies() const {
                if (_myTextChild) {
                    return _myTextChild->nodeValueWrapperPtr()->hasOutdatedDependencies();
                }
                return true;
            }
            void ensureDependencies() const {
                if (_myTextChild && _myTextChild->nodeValueWrapperPtr()) {
                    _myTextChild->nodeValueWrapperPtr()->ensureDependencies();
                }
            }
        protected:
            ValuePtr getValuePtr(const Node & theNode, const VALUE & theValue) {
                ensureTextChild(theNode, theValue);
                return _myTextChild->nodeValueWrapperPtr();
            }
            const ValuePtr getValuePtr(const Node & theNode, const VALUE & theValue) const {
                ensureTextChild(theNode, theValue);
                return _myTextChild->nodeValueWrapperPtr();
            }
        private:
            PropertyPlug() {};

            void ensureTextChild(const Node & theNode) const {
                // to allow factory nodes
                if (theNode) {
                    ensureTextChild(theNode, TAG::getDefault());
                }
            }

            void ensureTextChild(const Node & theNode, const VALUE & theValue) const {
				if (_myTextChild && _myPropertyNode &&
					_myTextChild->parentNode() && 
                    _myTextChild->parentNode()->parentNode() == _myPropertyNode->parentNode()) 
                {
					// alles super && alles beim alten
					return;
				}
                dom::Node & myPropertyListNode = const_cast<Node&>(theNode);
                _myPropertyNode = myPropertyListNode.childNodeByAttribute(TAG::getNodeName(), "name", TAG::getName());
                if (!_myPropertyNode) {
                    _myPropertyNode = myPropertyListNode.appendChild(NodePtr(new Element(TAG::getNodeName())));
                    _myPropertyNode->appendAttribute("name", TAG::getName());
                }

                _myTextChild = _myPropertyNode->childNode("#text");
                if (!_myTextChild) {
                    _myTextChild = _myPropertyNode->appendChild(dom::Text());
                    ValueHelper<VALUE, WRAPPER>::setValue(_myTextChild->nodeValueWrapperPtr(), theValue);
                }
				if (!_myFacade->getProperty(TAG::getName())) {
                    NodePtr myName = _myPropertyNode->getAttribute("name");
					if (myName) {
                        _myFacade->getProperties()[myName->nodeValue()] = _myPropertyNode->childNode("#text");
					}
				}
			}
            mutable dom::NodePtr _myTextChild;
            mutable dom::NodePtr _myPropertyNode;
			FACADE * _myFacade;

    };
}

#endif
