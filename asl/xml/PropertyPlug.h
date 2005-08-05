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
//   $RCSfile: LightSource.h,v $
//   $Author: christian $
//   $Revision: 1.9 $
//   $Date: 2004/11/15 10:05:49 $
//
//  Description: This class is the base of all lights.
//
//=============================================================================

#ifndef _ac_dom_PropertyPlug_h_
#define _ac_dom_PropertyPlug_h_

#include <dom/Nodes.h>

namespace dom {

    static const char * PROPERTY_LIST_NAME = "properties";
    static const char * NAME_ATTRIB        = "name";

#define DEFINE_PROPERTY_TAG(theTagName, theType, theNodeName, thePropertyName, theDefault) \
    struct theTagName { \
        typedef theType TYPE; \
        typedef dom::PropertyPlug<theTagName> Plug; \
        static const char * getNodeName() { return theNodeName; } \
        static const char * getName() { return thePropertyName; } \
		static const TYPE getDefault() { return theDefault; } \
    };

    template<class TAG>
    class PropertyPlug {
        public:
            typedef typename TAG::TYPE VALUE;
            typedef typename ValueWrapper<VALUE>::Type WRAPPER;

            PropertyPlug(const Node & theNode) : _myTextChild(0) {
            }

            const VALUE & getValue(const Node & theNode) const {
                if (!_myTextChild) {
                    ensureTextChild(theNode);
                }
                return ValueHelper<VALUE, WRAPPER>::getValue(_myTextChild->nodeValueWrapperPtr());
            }

            const VALUE & setValue(Node & theNode, const VALUE & theValue) {
                if (!_myTextChild) {
                    return ensureTextChild(theNode, theValue);
                } else {
                    return ValueHelper<VALUE, WRAPPER>::setValue(_myTextChild->nodeValueWrapperPtr(), theValue);
                }
            }

            unsigned long long getVersion(const Node & theNode) const {
                if (!_myTextChild) {
                    ensureTextChild(theNode);
                }
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
                if (!_myTextChild) {
                    ensureTextChild(theNode, theValue);
                }
                return _myTextChild->nodeValueWrapperPtr();
            }
            const ValuePtr getValuePtr(const Node & theNode, const VALUE & theValue) const {
                if (!_myTextChild) {
                    ensureTextChild(theNode, theValue);
                }
                return _myTextChild->nodeValueWrapperPtr();
            }
        private:
            PropertyPlug() {};

            const VALUE & ensureTextChild(const Node & theNode) const {
                return ensureTextChild(theNode, TAG::getDefault());
            }

            const VALUE & ensureTextChild(const Node & theNode, const VALUE & theValue) const {
                dom::NodePtr myPropertyListNode = theNode.childNode(dom::PROPERTY_LIST_NAME);
                if (!myPropertyListNode) {
                    myPropertyListNode = const_cast<Node&>(theNode).appendChild(NodePtr(new Element(dom::PROPERTY_LIST_NAME)));
                }

                NodePtr myPropertyNode = myPropertyListNode->childNodeByAttribute(TAG::getNodeName(), dom::NAME_ATTRIB, TAG::getName());
                if (!myPropertyNode) {
                    myPropertyNode = myPropertyListNode->appendChild(NodePtr(new Element(TAG::getNodeName())));
                    myPropertyNode->appendAttribute(dom::NAME_ATTRIB, TAG::getName());
                }

                _myTextChild = myPropertyNode->childNode("#text");
                if (!_myTextChild) {
                    _myTextChild = myPropertyNode->appendChild(dom::Text());
                    return ValueHelper<VALUE, WRAPPER>::setValue(_myTextChild->nodeValueWrapperPtr(), theValue);
                } else {
                    return ValueHelper<VALUE, WRAPPER>::getValue(_myTextChild->nodeValueWrapperPtr());
                }
            }
 
            mutable dom::NodePtr _myTextChild;
    };
}

#endif
