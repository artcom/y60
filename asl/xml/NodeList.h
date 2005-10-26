/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// Parts of the code (C) 2000 Pavel Mayer, written 22.-31.12.2000
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

#include "typedefs.h"
#include "Dictionary.h"
#include <asl/Block.h>

#include <vector>
#include <map>

namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

    class NodeList {
    public:
        friend class Node;

        NodeList(Node * theShell);
        NodeList(const NodeList & theOtherList, Node * theShell);
        virtual ~NodeList();
        virtual Node & operator[](int i);
        virtual const Node & operator[](int i) const;
        NodePtr item(int i);
        const NodePtr item(int i) const;
        int length() const;
        int size() const;
        void binarize(asl::WriteableStream & theDest, Dictionaries & theDicts,unsigned long long theIncludeVersion) const;
        asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos, Dictionaries & theDicts, bool thePatchFlag);
        int findIndex(const Node * theNode) const;
        NodePtr previousSibling(const Node * theNode);
        const NodePtr previousSibling(const Node * theNode) const;
        NodePtr nextSibling(const Node * theNode);
        const NodePtr nextSibling(const Node * theNode) const;
        
        virtual NodePtr removeItem(int i);
        virtual NodePtr append(NodePtr theNewNode);
        virtual void setItem(int theIndex, NodePtr theNewNode);
        virtual void insert(int theIndex, NodePtr theNewNode);
        virtual void clear();

        void setVersion(unsigned long long theVersion);
        // do not use except if you know exactly what you are doing
        virtual void setShell(Node * theShell) {
            _myShell = theShell;
        }
    protected:
        NodePtr appendWhileParsing(NodePtr theNewNode);
        virtual void reparent(Node * theNewParent, Node * theTopNewParent);
        virtual void resize(asl::AC_SIZE_TYPE newSize);
        Node * _myShell;
    private:
        std::vector<NodePtr> _myNodes;
    };

	class NamedNodeMap : public NodeList {
	public:
		friend class Node;

        NamedNodeMap(Node * theShell);
		NamedNodeMap(const NamedNodeMap & other, Node * theShell);
		virtual Node & operator[](int i);
		virtual const Node & operator[](int i) const;
		virtual Node & operator[](const DOMString & name);
		virtual const Node & operator[](const DOMString & name) const;
		virtual NodePtr getNamedItem(const DOMString & name);
		virtual const NodePtr getNamedItem(const DOMString & name) const;
		virtual NodePtr setNamedItem(NodePtr node);

        virtual NodePtr append(NodePtr theNewNode);
        virtual void setItem(int theIndex, NodePtr theNewItem);
        virtual void insert(int theIndex, NodePtr theNewNode);

        static int countNodesNamed(const DOMString & name, const NodeList & nodes);
        static int findNthNodeNamed(const DOMString & name, int n, const NodeList & nodes);
    protected:
        NodePtr appendWhileParsing(NodePtr theNewNode);
	};

	class TypedNamedNodeMap : public NamedNodeMap {
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
        virtual void setItem(int theIndex, NodePtr theNewItem);
        virtual void insert(int theIndex, NodePtr theNewNode);
    protected:
        NodePtr appendWhileParsing(NodePtr theNewNode);
	private:
        void checkType(NodePtr theNewNode);
		unsigned short allowedType;
	};
     /* @} */
} //Namespace dom



#endif
