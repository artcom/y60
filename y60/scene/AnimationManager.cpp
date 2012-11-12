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

// own header
#include "AnimationManager.h"

#include "Animation.h"
#include <y60/base/typedefs.h>
#include <y60/base/NodeNames.h>
#include <asl/math/Frustum.h>
#include <asl/xpath/xpath_api.h>

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
            char myLastChar = static_cast<char>(tolower(myName[myName.length() - 1]));
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
            if (theComponent != AnimationBase::FULL_VALUE) {
                myName = myName.substr(0, myName.length() - 2);
            }
        } else if (myName == "frustum.hfov") {
            theComponent = AnimationBase::HFOV;
            myName = "frustum";
        } else {
            size_t myPos = myName.find('[');
            if (myPos != std::string::npos) {
                size_t myCPos = myName.find(']');
                if (myCPos != std::string::npos) {
                    std::string myNumberString = myName.substr(myPos+1, myCPos-myPos-1);
                    size_t myIndex;
                    if (asl::fromString(myNumberString, myIndex)) {
                        theComponent = AnimationBase::AttributeComponent(myIndex);
                    } else {
                        throw AnimationManagerException(string("Malformed animation target name '") + theName +
                            "' , could not convert '"+ myNumberString+ " to unsigned integer", PLUS_FILE_LINE);
                    }
                } else {
                    throw AnimationManagerException(string("Malformed animation target name '") + theName +
                        "' , missing ]", PLUS_FILE_LINE);
                }
                myName = myName.substr(0,myPos);
            }
        }
        return myName;
    }

    dom::NodePtr
    AnimationManager::findAnimatedValue(dom::NodePtr theNode,
                                        const string & theAttributeRef,
                                        AnimationBase::AttributeComponent & theAttributeComponent)
    {
        string myAttributeName = theAttributeRef;

        myAttributeName = splitNameAndComponent( myAttributeName, theAttributeComponent );

        //XXX: somehow theNode does not get the full facade if not called once or something like that..
        // Serialization Problem???
        theNode->hasFacade();
        ///////////////////
        
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
        AnimationBase::AttributeComponent myAttributeComponent = AnimationBase::FULL_VALUE;
        dom::NodePtr    myAttribute = theNode->getAttribute(ANIM_ATTRIBUTE_ATTRIB);

        if (myAttribute && (myAttribute->nodeValue() != "")) {
            myAttributeRef = myAttribute->nodeValue();
            myAnimatedAttribute = findAnimatedValue(myAnimatedNode, myAttributeRef, myAttributeComponent);
        } else {
            dom::NodePtr myProperty = theNode->getAttribute(ANIM_PROPERTY_ATTRIB);
            if (myProperty) {
                myAttributeRef = myProperty->nodeValue();

                myAttributeRef = splitNameAndComponent( myAttributeRef, myAttributeComponent );

#if 0
                dom::NodePtr myPropPtr = myAnimatedNode->childNode(PROPERTY_LIST_NAME);
                dom::NodePtr myNode;
                for (dom::NodeList::size_type i=0; i<myPropPtr->childNodes().length(); ++i) {
                    myNode = myPropPtr->childNode(i);
                    if (myNode->getAttribute("name")->nodeValue() == myAttributeRef) {
                        myAnimatedAttribute = myNode->firstChild();
                    }
                }
#else
#if 0
                std::string myExpression = std::string(".//*[@name = '") + myAttributeRef + "']";
                dom::NodePtr myPropPtr = xpath::find(myExpression, myAnimatedNode);
                if (myPropPtr) {
                    myAnimatedAttribute = myPropPtr->firstChild();
                }
#else
                dom::NodePtr myPropPtr = myAnimatedNode->getElementByAttribute("","name",myAttributeRef);
                if (myPropPtr) {
                    myAnimatedAttribute = myPropPtr->firstChild();
                }
#endif
#endif
                if (!myAnimatedAttribute) {
                    throw AnimationManagerException(string("Property with name '") + myAttributeRef + "' not defined in \n" +
                                                    as_string(*myPropPtr), PLUS_FILE_LINE);
                }

            } else {
                throw AnimationManagerException(string("Attribute 'attribute' or 'property' not defined in \n") +
                                                as_string(*theNode), PLUS_FILE_LINE);
            }
        }

        // Get type of animated values
        dom::NodePtr myValueList;
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
            case AnimationBase::X:
            case AnimationBase::Y:
            case AnimationBase::Z:
            case AnimationBase::W:
                if (myTypeName == SOM_VECTOR_FLOAT_NAME) {
                    const asl::Vector3f * myVector3fValue = myAnimatedAttribute->nodeValuePtr<asl::Vector3f>();
                    if (myVector3fValue) {
                        return AnimationPtr(new Animation<float, asl::Vector3f>(theNode,
                                                                                myValueList, myAnimatedAttribute, myAttributeComponent - AnimationBase::X,
                                                                                myAngleAttribute));
                    }
                    const asl::Vector4f * myVector4fValue =  myAnimatedAttribute->nodeValuePtr<asl::Vector4f>();
                    if (myVector4fValue) {
                        return AnimationPtr(new Animation<float, asl::Vector4f>(theNode,
                                                                                myValueList, myAnimatedAttribute, myAttributeComponent - AnimationBase::X,
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
            case AnimationBase::FULL_VALUE:
                if (myTypeName == SOM_VECTOR_BOOL_NAME) {
                    const bool * myValue = myAnimatedAttribute->nodeValuePtr<bool>();
                    if (myValue) {
                        return AnimationPtr(new Animation<AcBool>(theNode, myValueList,
                                    myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_FLOAT_NAME) {
                    const float * myValue = myAnimatedAttribute->nodeValuePtr<float>();
                    if (myValue) {
                        return AnimationPtr(new Animation<float>(theNode, myValueList,
                                    myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_UNSIGNED_NAME) {
                    const unsigned * myValue = myAnimatedAttribute->nodeValuePtr<unsigned>();
                    if (myValue) {
                        return AnimationPtr(new Animation<unsigned>(theNode, myValueList,
                                    myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_VECTOR2F_NAME) {
                    const asl::Vector2f * myValue = myAnimatedAttribute->nodeValuePtr<asl::Vector2f>();
                    if (myValue) {
                        return AnimationPtr(new Animation<asl::Vector2f>(theNode, myValueList,
                                                                    myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_VECTOR3F_NAME) {
                    const asl::Vector3f * myValue = myAnimatedAttribute->nodeValuePtr<asl::Vector3f>();
                    if (myValue) {
                        return AnimationPtr(new Animation<asl::Vector3f>(theNode, myValueList,
                                                                         myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_VECTOR4F_NAME) {
                    const asl::Vector4f * myValue = myAnimatedAttribute->nodeValuePtr<asl::Vector4f>();
                    if (myValue) {
                        return AnimationPtr(new Animation<asl::Vector4f>(theNode, myValueList,
                                                                         myAnimatedAttribute,myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_STRING_NAME) {
                    const string * myValue = myAnimatedAttribute->nodeValuePtr<string>();
                    if (myValue) {
                        return AnimationPtr(new Animation<string>(theNode, myValueList,
                                    myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_QUATERNIONF_NAME) {
                    const asl::Quaternionf * myValue =
                            myAnimatedAttribute->nodeValuePtr<asl::Quaternionf>();
                    if (myValue) {
                        return AnimationPtr(new Animation<asl::Quaternionf>(theNode,
                                myValueList, myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                }
                break;

            default:
                if (myTypeName == SOM_VECTOR_BOOL_NAME) {
                    const VectorOfBool * myValue = myAnimatedAttribute->nodeValuePtr<VectorOfBool>();
                    if (myValue) {
                        return AnimationPtr(new Animation<AcBool,VectorOfBool>(theNode, myValueList,
                                             myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_FLOAT_NAME) {
                    const VectorOfFloat * myValue = myAnimatedAttribute->nodeValuePtr<VectorOfFloat>();
                    if (myValue) {
                        return AnimationPtr(new Animation<float, VectorOfFloat>(theNode, myValueList,
                                             myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_UNSIGNED_NAME) {
                    const VectorOfUnsignedInt * myValue = myAnimatedAttribute->nodeValuePtr<VectorOfUnsignedInt>();
                    if (myValue) {
                        return AnimationPtr(new Animation<unsigned,VectorOfUnsignedInt>(theNode, myValueList,
                                              myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_VECTOR2F_NAME) {
                    const VectorOfVector2f * myValue = myAnimatedAttribute->nodeValuePtr<VectorOfVector2f>();
                    if (myValue) {
                        return AnimationPtr(new Animation<asl::Vector2f,VectorOfVector2f>(theNode, myValueList,
                                             myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_VECTOR3F_NAME) {
                    const VectorOfVector3f * myValue = myAnimatedAttribute->nodeValuePtr<VectorOfVector3f>();
                    if (myValue) {
                        return AnimationPtr(new Animation<asl::Vector3f,VectorOfVector3f>(theNode, myValueList,
                                             myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_VECTOR4F_NAME) {
                    const VectorOfVector4f * myValue = myAnimatedAttribute->nodeValuePtr<VectorOfVector4f>();
                    if (myValue) {
                        return AnimationPtr(new Animation<asl::Vector4f,VectorOfVector4f>(theNode, myValueList,
                                             myAnimatedAttribute,myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_STRING_NAME) {
                    const VectorOfString * myValue = myAnimatedAttribute->nodeValuePtr<VectorOfString>();
                    if (myValue) {
                        return AnimationPtr(new Animation<string,VectorOfString>(theNode, myValueList,
                                             myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                } else if (myTypeName == SOM_VECTOR_QUATERNIONF_NAME) {
                    const VectorOfQuaternionf * myValue =  myAnimatedAttribute->nodeValuePtr<VectorOfQuaternionf>();
                    if (myValue) {
                        return AnimationPtr(new Animation<asl::Quaternionf, VectorOfQuaternionf>(theNode,
                                            myValueList, myAnimatedAttribute, myAttributeComponent, myAngleAttribute));
                    }
                    myTypeStringIsValid = true;
                }
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
        if (!_myGlobalAnimations) {
            _myGlobalAnimations = AnimationClipPtr(new AnimationClip(theNode, *this, theWorld));
        } else {
            _myGlobalAnimations->reload(theNode, *this, theWorld);
        }
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
