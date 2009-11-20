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

#ifndef _ac_dom_PropertyPlug_h_
#define _ac_dom_PropertyPlug_h_

#include "asl_dom_settings.h"

#include "asl/dom/Nodes.h"
#include "PlugHelper.h"

namespace dom {

#define PROPERTY_TAG_DEFAULT_VALUE(theType) \
    static const dom::ValuePtr getDefaultValue() { \
        static dom::ValuePtr myValue(new dom::ValueWrapper<theType>::Type(getDefault(), 0)); \
        return myValue; \
    }

#define PROPERTY_TAG_NO_DEFAULT_VALUE() \
    static const dom::ValuePtr getDefaultValue() { \
        return dom::ValuePtr(); \
    }

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
    PROPERTY_TAG_DEFAULT_VALUE(theType) \
    };

    template<class TAG, class FACADE>
    class PropertyPlug : PlugBase {
    public:
        typedef typename TAG::TYPE VALUE;
        typedef typename ValueWrapper<VALUE>::Type WRAPPER;

        PropertyPlug(FACADE * theFacade) : _myTextChild(), _myPropertyNode(), _myFacade(theFacade)
        {
            Node& myNode = theFacade->getNode();
            ensureTextChild(myNode);
        }

        const ValuePtr getDefaultValue(const Node & theNode) const {
            if(!_myTextChild) {
                ensureTextChild(theNode);
            }
            return TAG::getDefaultValue();
        }

        ValuePtr getValuePtr() {
            return _myTextChild->nodeValueWrapperPtr();
        }

        const ValuePtr getValuePtr() const {
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
            _myPropertyNode->setPlug(this);

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
