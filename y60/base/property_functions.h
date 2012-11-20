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
*/
//
//   $Id: property_functions.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: property_functions.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: XML-File-Export Plugin
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_property_functions_h_
#define _ac_property_functions_h_

#include "y60_base_settings.h"

#include "NodeNames.h"
#include <asl/dom/Nodes.h>
#include <asl/base/string_functions.h>
#include <asl/base/Exception.h>

namespace y60 {

    DEFINE_EXCEPTION(SOM_PROPERTY_MISSING, asl::Exception);
    DEFINE_EXCEPTION(SOM_PROPERTYLIST_MISSING, asl::Exception);

    template <class T>
    void
    setPropertyValue(dom::NodePtr theParentNode, const char * theType, const char * thePropertyName, const T & theValue,
                     const char * theListName = PROPERTY_LIST_NAME) {
        dom::NodePtr myPropertyListNode = theParentNode->childNode(theListName);

        if (myPropertyListNode) {

            dom::NodePtr myPropertyNode = myPropertyListNode->childNodeByAttribute(theType, NAME_ATTRIB, thePropertyName);
            if (!myPropertyNode) {
                myPropertyNode = dom::NodePtr(new dom::Element(theType));
                (*myPropertyNode)("#text").nodeValue(asl::as_string(theValue));
                myPropertyListNode->appendChild(myPropertyNode);

                myPropertyNode->appendAttribute(NAME_ATTRIB, thePropertyName);
            } else {
                (*myPropertyNode)("#text").nodeValue(asl::as_string(theValue));
            }
        } else {
            throw SOM_PROPERTYLIST_MISSING(std::string("Propertylist in node ")+theParentNode->nodeName()+" not found",PLUS_FILE_LINE);
        }

    }

    template <class T>
    T
    getPropertyValue(dom::NodePtr theParentNode, const char * theType, const char * thePropertyName,
                     const char * theListName = PROPERTY_LIST_NAME) {
        return getPropertyValue<T>(*theParentNode, theType, thePropertyName, theListName);
    }

    template <class T>
    T
    getPropertyValue(const dom::Node & theParentNode, const char * theType, const char * thePropertyName,
                     const char * theListName = PROPERTY_LIST_NAME) {
        dom::NodePtr myPropertyListNode = theParentNode.childNode(theListName);
        if (!myPropertyListNode) {
            throw SOM_PROPERTYLIST_MISSING(std::string("Propertylist in node ")+theParentNode.nodeName()+" not found",PLUS_FILE_LINE);
        }
        dom::NodePtr myPropertyNode = myPropertyListNode->childNodeByAttribute(theType, NAME_ATTRIB, thePropertyName);
        if (!myPropertyNode) {
            throw SOM_PROPERTY_MISSING(std::string("Property ")+thePropertyName+" not found",PLUS_FILE_LINE);
        }
        return (*myPropertyNode)("#text").dom::Node::nodeValueAs<T>(); // help gcc a bit to find the method :-(
    }

    template <class T>
    bool
    hasPropertyValue(dom::NodePtr theParentNode, const char * theType, const char * thePropertyName,
                     const char * theListName = PROPERTY_LIST_NAME) {
        dom::NodePtr myPropertyListNode = theParentNode->childNode(theListName);
        if (!myPropertyListNode) {
            return false;
        }
        dom::NodePtr myPropertyNode = myPropertyListNode->childNodeByAttribute(theType, NAME_ATTRIB, thePropertyName);
        if (!myPropertyNode) {
            return false;
        }
        return true;
    }
    template <class T>
    T
    getPropertyValue(dom::NodePtr theParentNode, const char * theType, const char * thePropertyName, const T & theDefault,
                     const char * theListName = PROPERTY_LIST_NAME) {
        if (hasPropertyValue<T>(theParentNode, theType, thePropertyName,theListName) ) {
            try {
                return getPropertyValue<T>(theParentNode, theType, thePropertyName, theListName);
            } catch (SOM_PROPERTY_MISSING ex) {
                return theDefault;
            }
        } else {
            return theDefault;
        }
    }
}
#endif // _ac_property_functions_h_


