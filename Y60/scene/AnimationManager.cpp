//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "AnimationManager.h"
#include "Animation.h"
#include <y60/typedefs.h>
#include <y60/NodeNames.h>
#include <asl/Frustum.h>

using namespace std;
using namespace asl;

namespace y60 {

    AnimationManager::AnimationManager(){
    }

    AnimationManager::~AnimationManager() {
    }

    std::string
    AnimationManager::splitNameAndComponent( const std::string & theName,
        AnimationBase::AttributeComponent & theComponent ) 
    {
        std::string myName( theName );

        if (myName.length() >= 2 && myName[myName.length() - 2] == '.') {
            char myLastChar = tolower(myName[myName.length() - 1]);
            switch (myLastChar) {
                case 'x':
                case 'r':
                    theComponent = AnimationBase::X;
                    break;
                case 'y':
                case 'g':
                    theComponent = AnimationBase::Y;
                    break;
                case 'z':
                case 'b':
                    theComponent = AnimationBase::Z;
                    break;
                case 'w':
                case 'a':
                    theComponent = AnimationBase::W;
                    break;
            }
            if (theComponent != AnimationBase::SCALAR) {
                myName = myName.substr(0, myName.length() - 2);
            }
        } else if (myName == "frustum.hfov") {
            theComponent = AnimationBase::HFOV;
            myName = "frustum";
        }
        return myName;
    }

    //dom::ValuePtr
    dom::NodePtr
    AnimationManager::findAnimatedValue(dom::NodePtr theNode,
                                        const string & theAttributeRef,
                                        AnimationBase::AttributeComponent & theAttributeComponent)
    {
        string myAttributeName = theAttributeRef;

        myAttributeName = splitNameAndComponent( myAttributeName, theAttributeComponent );
            
        dom::NodePtr myAnimatedAttribute = theNode->getAttribute(myAttributeName);
        
        if (!myAnimatedAttribute) {
            throw AnimationManagerException(string("Animated attribute '") + myAttributeName +
                "' not found in \n" + as_string(*theNode), PLUS_FILE_LINE);
        }

        return myAnimatedAttribute;
    }

    AnimationPtr
    AnimationManager::createAnimation(const dom::NodePtr theNode, dom::NodePtr theWorld) {
        // Find node to be animated
        dom::NodePtr myAnimatedNode;        
		dom::NodePtr myNodeRefAttribute = theNode->getAttribute(ANIM_NODEREF_ATTRIB);
        string myNodeRef;
        if (myNodeRefAttribute) {
			myNodeRef = myNodeRefAttribute->nodeValue();
            myAnimatedNode = theWorld->getElementById(myNodeRef);
            if (!myAnimatedNode) {
                throw AnimationManagerException(string("Animation points to a node that does not exist.\n") +
                    as_string(*theNode), PLUS_FILE_LINE);
            }
        } else {
            throw AnimationManagerException(string("Attribute 'node' not defined in \n") +
                as_string(*theNode), PLUS_FILE_LINE);
        }

        // Find attribute to be animated
        dom::NodePtr    myAnimatedAttribute;
        string          myAttributeRef;
        bool            myAngleAttribute = false;
        AnimationBase::AttributeComponent myAttributeComponent = AnimationBase::SCALAR;
		dom::NodePtr    myAttribute = theNode->getAttribute(ANIM_ATTRIBUTE_ATTRIB);

        if (myAttribute && (myAttribute->nodeValue() != "")) {
            myAttributeRef = myAttribute->nodeValue();
            myAnimatedAttribute = findAnimatedValue(myAnimatedNode, myAttributeRef, myAttributeComponent);
        } else {
            dom::NodePtr myProperty = theNode->getAttribute(ANIM_PROPERTY_ATTRIB);
            if (myProperty) {
                myAttributeRef = myProperty->nodeValue();

                myAttributeRef = splitNameAndComponent( myAttributeRef, myAttributeComponent );

                dom::NodePtr myPropPtr = myAnimatedNode->childNode(PROPERTY_LIST_NAME);

                dom::NodePtr myNode;
                for (int i=0; i<myPropPtr->childNodes().length(); ++i) {
                    myNode = myPropPtr->childNode(i);
                    if (myNode->getAttribute("name")->nodeValue() == myAttributeRef) {
                        myAnimatedAttribute = myNode->firstChild();
                    }
                }

                if (!myAnimatedAttribute) {
                    throw AnimationManagerException(string("Property ") + myAttributeRef + " not defined in \n" +
                                                    as_string(*myPropPtr), PLUS_FILE_LINE);
                }
                
            } else {
                throw AnimationManagerException(string("Attribute 'attribute' or 'property' not defined in \n") +
                                                as_string(*theNode), PLUS_FILE_LINE);
            }
        }
        
        // Get type of animated values
        dom::NodePtr myValueList(0);
        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            dom::NodePtr myChild = theNode->childNode(i);
            if (myChild->nodeType() == dom::Node::ELEMENT_NODE) {
                myValueList = myChild;
                break;
            }
        }

        if (!myValueList) {
            throw AnimationManagerException(
                string("Animation node does not contain value list: \n") +
                asl::as_string(*theNode), PLUS_FILE_LINE);
        }

        string myTypeName = myValueList->nodeName();

        // Setup animation template class
        bool myTypeStringIsValid = false;
        switch (myAttributeComponent) {
            case AnimationBase::SCALAR:
                if (myTypeName == SOM_VECTOR_BOOL_NAME) {
                    const bool * myValue = myAnimatedAttribute->nodeValuePtr<bool>();
                    if (myValue) {
                        return AnimationPtr(new Animation<AcBool>(theNode, myValueList, 
                                    myAnimatedAttribute, 0, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_FLOAT_NAME) {
                    const float * myValue = myAnimatedAttribute->nodeValuePtr<float>();
                    if (myValue) {
                        return AnimationPtr(new Animation<float>(theNode, myValueList, 
                                    myAnimatedAttribute, 0, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_UNSIGNED_NAME) {
                    const unsigned * myValue = myAnimatedAttribute->nodeValuePtr<unsigned>();
                    if (myValue) {
                        return AnimationPtr(new Animation<unsigned>(theNode, myValueList, 
                                    myAnimatedAttribute, 0, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_STRING_NAME) {
                    const string * myValue = myAnimatedAttribute->nodeValuePtr<string>();
                    if (myValue) {
                        return AnimationPtr(new Animation<string>(theNode, myValueList, 
                                    myAnimatedAttribute, 0, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_QUATERNIONF_NAME) {
                    const asl::Quaternionf * myValue = 
                            myAnimatedAttribute->nodeValuePtr<asl::Quaternionf>();
                    if (myValue) {
                        return AnimationPtr(new Animation<asl::Quaternionf>(theNode, 
                                myValueList, myAnimatedAttribute, 0, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                }
                break;
            case AnimationBase::X:
            case AnimationBase::Y:
            case AnimationBase::Z:
            case AnimationBase::W:
                if (myTypeName == SOM_VECTOR_FLOAT_NAME) {
                    const asl::Vector3f * myVector3fValue = 
                        myAnimatedAttribute->nodeValuePtr<asl::Vector3f>();
                    if (myVector3fValue) {
                        return AnimationPtr(new Animation<float, asl::Vector3f>(theNode, 
                                    myValueList, myAnimatedAttribute, myAttributeComponent, 
                                    myAngleAttribute));
                    }
                    const asl::Vector4f * myVector4fValue = 
                            myAnimatedAttribute->nodeValuePtr<asl::Vector4f>();
                    if (myVector4fValue) {
                        return AnimationPtr(new Animation<float, asl::Vector4f>(theNode, 
                                    myValueList, myAnimatedAttribute, myAttributeComponent, 
                                    myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                }
                break;
            case AnimationBase::HFOV:
                if (myTypeName == SOM_VECTOR_FLOAT_NAME) {
                    const Frustum * myValue = myAnimatedAttribute->nodeValuePtr<Frustum>();
                    if (myValue) {
                        return AnimationPtr(new Animation<float, Frustum>(theNode, 
                                    myValueList, myAnimatedAttribute, myAttributeComponent, 
                                    myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                }
                break;
            default:
                break;
        }

        // Error handling
        if (myTypeStringIsValid) {
            throw AnimationManagerException(string("Attribute '") +
                myAttributeRef + "'with type '" +
                myAnimatedAttribute->nodeValueWrapperPtr()->name() + "' does not match type of animation values: '" +
                myTypeName + "'. Animated node id: " +
                myNodeRef, PLUS_FILE_LINE);
        } else {
            throw AnimationManagerException(string("Animation of attribute '") +
                myAttributeRef + "'with type '" +
                myAnimatedAttribute->nodeValueWrapperPtr()->name() + "' not supported. Animated nodeid: " +
                myNodeRef, PLUS_FILE_LINE);
        }
    }

    void
    AnimationManager::loadGlobals(const dom::NodePtr theNode, dom::NodePtr theWorld) {
        _myGlobalAnimations = AnimationClipPtr(new AnimationClip(theNode, *this, theWorld));
        _myGlobalAnimations->setActive();
    }

    void
    AnimationManager::loadCharacters(const dom::NodePtr theNode, dom::NodePtr theWorld) {
        _myCharacter.clear();
        unsigned myCharacterCount = theNode->childNodesLength();
        for (unsigned i = 0; i < myCharacterCount; ++i) {
            const dom::NodePtr myNode = theNode->childNode(i);
            if (myNode->nodeType() == dom::Node::ELEMENT_NODE &&
                myNode->nodeName() == CHARACTER_NODE_NAME)
            {
                string myCharacterName = myNode->dom::Node::getAttributeString(NAME_ATTRIB);
                if (_myCharacter.find(myCharacterName) == _myCharacter.end() ) {
                    _myCharacter[myCharacterName] = AnimationCharacterPtr(new AnimationCharacter(myNode,
                                                                              *this, theWorld));
                } else {
                    throw AnimationManagerException(string("Duplicate character: ") + myCharacterName, PLUS_FILE_LINE);
                }
            }
        }
    }

    void
    AnimationManager::update() {
        _myGlobalAnimations->update();
        _myGlobalAnimations->setActive();
    }

    void
    AnimationManager::run(double theTime) {

        _myGlobalAnimations->run(theTime);

        AnimationCharacterMap::iterator myCharacterBegin = _myCharacter.begin();
        AnimationCharacterMap::iterator myCharacterEnd   = _myCharacter.end();
        for (; myCharacterBegin != myCharacterEnd; ++myCharacterBegin) {
            myCharacterBegin->second->run(theTime);
        }
    }

    void
    AnimationManager::playClip(double theTime, const string & theCharacterName, const string & theClipName) {
        getCharacter(theCharacterName, PLUS_FILE_LINE)->playClip(theTime, theClipName);
    }

    bool
    AnimationManager::isClipActive(const string & theCharacterName, const string & theClipName) {
        return getCharacter(theCharacterName, PLUS_FILE_LINE)->isClipActive(theClipName);
    }

    bool
    AnimationManager::isCharacterActive(const std::string & theCharacterName) {
        return getCharacter(theCharacterName, PLUS_FILE_LINE)->isActive();
    }

    void
    AnimationManager::stop(const std::string & theCharacterName) {
        getCharacter(theCharacterName, PLUS_FILE_LINE)->stop();
    }

    void
    AnimationManager::setClipLoops(const std::string & theCharacterName,
                                   const std::string & theClipName, unsigned int theLoops) {
        getCharacter(theCharacterName, PLUS_FILE_LINE)->setClipLoops(theClipName, theLoops);
    }
    void
    AnimationManager::setClipForwardDirection(const std::string & theCharacterName,
                                   const std::string & theClipName, bool theFlag) {
        getCharacter(theCharacterName, PLUS_FILE_LINE)->setClipForwardDirection(theClipName, theFlag);
    }

    unsigned int
    AnimationManager::getLoops(const std::string & theCharacterName,
                                   const std::string & theClipName) {
        return getCharacter(theCharacterName, PLUS_FILE_LINE)->getLoops(theClipName);
    }

    AnimationCharacterPtr
    AnimationManager::getCharacter(const std::string & theCharacterName, const std::string & theContext=PLUS_FILE_LINE) {
         if (_myCharacter.find(theCharacterName) != _myCharacter.end() ) {
            return _myCharacter[theCharacterName];
        } else {
            throw AnimationManagerException(string("Unknown character: ") + theCharacterName, theContext);
        }
    }

}
