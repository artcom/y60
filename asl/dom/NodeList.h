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
//    $RCSfile: NodeList.h,v $
//
//   $Revision: 1.14 $
//
// Description: tiny fast XML-Parser and DOM
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _xml_NodeList_h_included_
#define _xml_NodeList_h_included_

#include "asl_dom_settings.h"

#include "typedefs.h"
#include "Dictionary.h"
#include <asl/base/Block.h>

#include <vector>
#include <map>

namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

    /* NodeList holds a list of Nodes; beware that you can not use NodeList freely because NodeList also
       manages parent pointers and IDRegistry entries, so putting a Node into a NodeList will unregister it
       with the original dom IDRRegistry.
    */

    class ASL_DOM_DECL NodeList {
    private:
        typedef std::vector<NodePtr> _myNodeListType;
    public:
        friend class Node;

        typedef _myNodeListType::size_type size_type;

        NodeList(Node * theShell);
        NodeList(const NodeList & theOtherList, Node * theShell);
        virtual ~NodeList();
        virtual Node & operator[](size_type i);
        virtual const Node & operator[](size_type i) const;
        NodePtr item(size_type i);
        const NodePtr item(size_type i) const;
        size_type length() const;
        size_type size() const;
        void binarize(asl::WriteableStream & theDest, Dictionaries & theDicts,unsigned long long theIncludeVersion) const;
        asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos, Dictionaries & theDicts, OpMode theLoadMode);
        size_type findIndex(const Node * theNode) const;
        NodePtr previousSibling(const Node * theNode);
        const NodePtr previousSibling(const Node * theNode) const;
        NodePtr nextSibling(const Node * theNode);
        const NodePtr nextSibling(const Node * theNode) const;
        bool findByOffset(asl::Unsigned64 myOffset, asl::AC_SIZE_TYPE & theIndex) const;

        virtual NodePtr removeItem(size_type i);
        virtual NodePtr append(NodePtr theNewNode);
        virtual void setItem(size_type theIndex, NodePtr theNewNode);
        virtual void insert(size_type theIndex, NodePtr theNewNode);
        virtual void clear();

        void setVersion(unsigned long long theVersion);
        // do not use except if you know exactly what you are doing
        virtual void setShell(Node * theShell) {
            _myShell = theShell;
        }
        virtual void freeCaches() const;
    protected:
        NodePtr appendWithoutReparenting(NodePtr theNewNode);
        virtual void reparent(Node * theNewParent, Node * theTopNewParent);
        virtual void resize(asl::AC_SIZE_TYPE newSize);
        virtual void flush(); // is a clear() with version bumping
        Node * _myShell;
    private:
        _myNodeListType _myNodes;
    };

    class ASL_DOM_DECL NamedNodeMap : public NodeList {
    public:
        friend class Node;

        NamedNodeMap(Node * theShell);
        NamedNodeMap(const NamedNodeMap & other, Node * theShell);
        virtual Node & operator[](size_type i);
        virtual const Node & operator[](size_type i) const;
        virtual Node & operator[](const DOMString & name);
        virtual const Node & operator[](const DOMString & name) const;
        virtual NodePtr getNamedItem(const DOMString & name);
        virtual const NodePtr getNamedItem(const DOMString & name) const;
        virtual NodePtr setNamedItem(NodePtr node);

        virtual NodePtr append(NodePtr theNewNode);
        virtual void setItem(size_type theIndex, NodePtr theNewItem);
        virtual void insert(size_type theIndex, NodePtr theNewNode);

        static size_type countNodesNamed(const DOMString & name, const NodeList & nodes);
        static size_type findNthNodeNamed(const DOMString & name, size_type n, const NodeList & nodes);
    protected:
        NodePtr appendWithoutReparenting(NodePtr theNewNode);
    };

    class ASL_DOM_DECL TypedNamedNodeMap : public NamedNodeMap {
    public:
        friend class Node;

        TypedNamedNodeMap(unsigned short theAllowedType, Node * theShell) : NamedNodeMap(theShell), allowedType(theAllowedType) {}
        TypedNamedNodeMap(Node * theShell) : NamedNodeMap(theShell), allowedType(0) {}
        TypedNamedNodeMap(const NamedNodeMap & other, unsigned short theAllowedType, Node * theShell)
            : NamedNodeMap(other, theShell), allowedType(theAllowedType)  {}
        TypedNamedNodeMap(const TypedNamedNodeMap & other, Node * theShell)
            : NamedNodeMap(other, theShell), allowedType(other.allowedType) {}
        virtual NodePtr setNamedItem(NodePtr node);
        virtual NodePtr append(NodePtr theNewNode);
        virtual void setItem(size_type theIndex, NodePtr theNewItem);
        virtual void insert(size_type theIndex, NodePtr theNewNode);
    protected:
        NodePtr appendWithoutReparenting(NodePtr theNewNode);
    private:
        void checkType(NodePtr theNewNode);
        unsigned short allowedType;
    };
    /* @} */
} //Namespace dom



#endif
