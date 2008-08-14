/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: Nodes.h,v $
//
//   $Revision: 1.53 $
//
// Description: tiny fast XML-Parser and DOM
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _xml_Nodes_h_included_
#define _xml_Nodes_h_included_

#include <asl/settings.h>

#include "typedefs.h"
#include "Exceptions.h"
#include "Value.h"
#include "NodeList.h"
#include "Dictionary.h"
#include "Facade.h"
#include "Events.h"

#include <map>
#include <set>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4355 )
#endif



namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */
   /// loads dictionaries and catalog from a stream starting at thePos
    inline
    asl::AC_SIZE_TYPE
    loadDictionariesAndCatalog(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos,
            Dictionaries & theDictionaries,
            NodeOffsetCatalog & theCatalog)
    {
        thePos = theDictionaries.debinarize(theSource, thePos);
        thePos = theCatalog.debinarize(theSource, thePos);
        asl::Unsigned64 mySize;
        thePos = theSource.readUnsigned64(mySize, thePos);
        return thePos; 
    }

    /// loads dictionaries and catalog from a block determining the position
    /// by reading the last 8 bytes of the block first, expecting it to contain
    /// the size of dictionaries and catalog 
    inline
    asl::AC_SIZE_TYPE
    loadDictionariesAndCatalog(const asl::ReadableStream & theSource,
            Dictionaries & theDictionaries,
            NodeOffsetCatalog & theCatalog)
    {
        asl::Unsigned64 myDictCatSize = 0;
        theSource.readUnsigned64(myDictCatSize, theSource.size() - sizeof(asl::Unsigned64));
        asl::AC_SIZE_TYPE thePos =  theSource.size() - asl::AC_SIZE_TYPE(myDictCatSize);
        thePos = theDictionaries.debinarize(theSource, thePos);
        thePos = theCatalog.debinarize(theSource, thePos);
        asl::Unsigned64 mySize;
        thePos = theSource.readUnsigned64(mySize, thePos);
        return thePos; 
    }

    /**
    *
    * XML - Document Object Model - Node Content:
    * <pre>
    * node                  nodeName                      nodeValue                             attributes
    * --------------------------------------------------------------------------------------------------
    * Element               tagName                   null                                  NamedNodeMap
    * Attribute             name of attribute         value of attribute                    null
    * Text                  "#text"                   content of the text node              null
    * CDATASection          "#cdata-section"          content of the CDATA Section          null
    * EntityReference       name of entity referenced null                                  null
    * Entity                entity name               null                                  null
    * ProcessingInstruction target                    entire content excluding the target   null
    * Comment               "#comment"                content of the comment                null
    * Document              "#document"               null                                  null
    * DocumentType          document type name        null                                  null
    * DocumentFragment      "#document-fragment"      null                                  null
    * Notation notation     name                      null                                  null
    * </pre>
    *
    * Allowed children:
    *
    *  - Document
    *    Element (maximum of one), ProcessingInstruction, Comment, DocumentType
    *  - DocumentFragment
    *    Element, ProcessingInstruction, Comment, Text, CDATASection, EntityReference
    *  - DocumentType
    *    Notation, Entity
    *  - EntityReference
    *    Element, ProcessingInstruction, Comment, Text, CDATASection, EntityReference
    *  - Element
    *    Element, Text, Comment, ProcessingInstruction, CDATASection, EntityReference
    *  - Attribute
    *    Text, EntityReference
    *  - ProcessingInstruction
    *    no other nodes
    *  - Comment
    *    no other nodes
    *  - Text
    *    no other nodes
    *  - CDATASection
    *    no other nodes
    *  - Entity
    *    no other nodes
    *  - Notation
    *    no other nodes
    *
    *   See http://www.w3.org/TR/PR-DOM-Level-1/level-one-core.html
     */

    extern const char * NodeTypeName[16];


    /** Base Class for all XML-Node Types, contains almost everything you use.
     *
     * The Node class is the base class for all node types and contains almost
     * all the functionality and all the interfaces for the XML Document Object Model
     * manipulation. It can assume every node type. The derived classes are just
     * provided for convenience. It is not possible to determine the node type by a
     * dynamic_cast; you have to check use nodeType() instead.
     *
     * Currently implemented node types are:
     *
     *  - ELEMENT_NODE
     *  - ATTRIBUTE_NODE
     *  - TEXT_NODE
     *  - CDATA_SECTION_NODE,
     *  - PROCESSING_INSTRUCTION_NODE
     *  - COMMENT_NODE
     *  - DOCUMENT_NODE,
     *  - DOCUMENT_FRAGMENT_NODE
     *  - ENTITY_REFERENCE_NODE
     *  - ENTITY_NODE,
     *  - DOCUMENT_TYPE_NODE
     *
     * Not yet implemented node types are:
     *  - NOTATION_NODE
     */
    class Node : public asl::PoolAllocator<Node>, public EventTarget {
    protected:
         struct SchemaInfo : public asl::PoolAllocator<SchemaInfo> {
            SchemaInfo();

            // TODO: value and facade factory pointers should be moved to
            // schema; they are all the same for one schema;
            // instead, a pointer to an element-id-registry should be here
            asl::Ptr<ValueFactory> _myValueFactory;
            dom::FacadeFactoryPtr  _myFacadeFactory;

            NodePtr                _mySchema;
            NodePtr                _mySchemaDeclaration;
            NodePtr                _myType;
            //NodePtr                _myArrayValuePrototype;
            int                    _myDepthLevel;

            const DOMString & getTypeName() const;
            SchemaPtr getSchema();
            const SchemaPtr getSchema() const;
        };
   public:
        typedef asl::Ptr<SchemaInfo,ThreadingModel> SchemaInfoPtr;

        friend class NodeList;
        friend class NamedNodeMap;
        friend class IDValue;
        friend class IDRefValue;
        friend class NodeOffsetCatalog;

        /**
         * used to distinguish different node types;
         * @doc X_NO_NODE and X_END_NODE are no valid nodes
         */
        enum NodeType {
            X_NO_NODE = 0,
            ELEMENT_NODE = 1,
            ATTRIBUTE_NODE = 2,
            TEXT_NODE = 3,
            CDATA_SECTION_NODE = 4,
            ENTITY_REFERENCE_NODE = 5,
            ENTITY_NODE = 6,
            PROCESSING_INSTRUCTION_NODE = 7,
            COMMENT_NODE = 8,
            DOCUMENT_NODE = 9,
            DOCUMENT_TYPE_NODE = 10,
            DOCUMENT_FRAGMENT_NODE = 11,
            NOTATION_NODE = 12,
            X_END_NODE = 15
        };

        enum { BINARIZER_VERSION = 100};
        enum { B60_MAGIC = 0xbbbbfb60};
        enum { D60_MAGIC = 0xddddfd60};
        enum { P60_MAGIC = 0xaaaaab60}; // b60 patch

        DEFINE_NESTED_EXCEPTION(Node,Exception,asl::Exception);
        DEFINE_NESTED_EXCEPTION(Node,LoadFileFailed,Exception);
        DEFINE_NESTED_EXCEPTION(Node,SchemaNotParsed,Exception);
        DEFINE_NESTED_EXCEPTION(Node,TypeMismatch,Exception);
        DEFINE_NESTED_EXCEPTION(Node,DuplicateIDValue,Exception);
        DEFINE_NESTED_EXCEPTION(Node,DuplicateNode,Exception);
        DEFINE_NESTED_EXCEPTION(Node,NodeNotRegistered,Exception);
        DEFINE_NESTED_EXCEPTION(Node,IDValueNotRegistered,Exception);
        DEFINE_NESTED_EXCEPTION(Node,DuplicateIDRefValue,Exception);
        DEFINE_NESTED_EXCEPTION(Node,IDRefValueNotRegistered,Exception);
        DEFINE_NESTED_EXCEPTION(Node,BadMagicNumber,Exception);
        DEFINE_NESTED_EXCEPTION(Node,VersionMismatch,Exception);

        /**@name constructors
            many different way to construct a node type.
            @doc However, you should not need to use them directly;
            use the constructors of the Node-derived classes for more
            safety, readabilty and convenience.
        */
        //@{
        /// default constructor
        Node() : _myType(X_NO_NODE), _myParseCompletionPos(0), _myDocSize(0),
                _myChildrenList(this), _myAttributes(this), _myParent(0), _myFacade(0), _myVersion(0), _lazyChildren(false) {}

        /// create element node with attributes
        Node(const DOMString & name, const NamedNodeMap & attributes, Node * theParent)
            : _myType(ELEMENT_NODE), _myName(name), _myAttributes(attributes, ATTRIBUTE_NODE, this),
            _myParseCompletionPos(0), _myDocSize(0), _myParent(theParent), _myChildrenList(this),
            _myFacade(0), _myVersion(0), _lazyChildren(false) {}

        /** create Entity Reference, Entity, ProcessingInstruction or Notation node
            or any of the above nodes
            */
        Node(NodeType type,const DOMString & name, const DOMString & value, Node * theParent);

        /** create Entity Reference, Entity, ProcessingInstruction or Notation node
            or any of the above nodes by supplying a special Value Type
            */
        Node(NodeType type,const DOMString & name, const ValueBase & theValue, Node * theParent)
            : _myType(type), _myName(name), _myValue(theValue.clone(this)),
            _myParseCompletionPos(0), _myDocSize(0), _myParent(theParent),_myChildrenList(this),
            _myAttributes(this), _myFacade(0), _myVersion(0), _lazyChildren(false) {}

        /** create Entity Reference, Entity, ProcessingInstruction or Notation node
            or any of the above nodes by supplying a special Value Type Ptr;
            this constructor takes the ownership of ValuePtr.
            */
        Node(NodeType type,const DOMString & name, ValuePtr theValuePtr, Node * theParent)
            : _myType(type), _myName(name), _myValue(theValuePtr),
            _myParseCompletionPos(0), _myDocSize(0), _myParent(theParent), _myChildrenList(this),
            _myAttributes(this), _myFacade(0), _myVersion(0), _lazyChildren(false)
        {
            if (_myValue) {
                _myValue->setNodePtr(this);
            }
        }

        /** create document oder document_fragment from xml-source
        @param xml must contain valid xml-data
        @param pos denotes the starting position for parsing; parsing
        proceeds until end of string or an error condition occurs
        @param type is the desired node type; it should be document or
        document fragment; if it is left unspecified, the node type is set
        to document on success
        */
        Node(const DOMString & xml, int pos,
            Node * parent, const Node * doctype, NodeType type = X_NO_NODE)
            : _myType(type), _myParseCompletionPos(0), _myDocSize(0),
            _myParent(parent),_myChildrenList(this),_myAttributes(this),
            _myFacade(0), _myVersion(0), _lazyChildren(false)
        {
            parseNextNode(xml,pos,parent,doctype);
        }

        Node(const DOMString & xml)
            : _myType(X_NO_NODE), _myParseCompletionPos(0), _myDocSize(0),_myParent(0),
            _myChildrenList(this), _myAttributes(this), _myFacade(0), _myVersion(0), _lazyChildren(false)
        {
            parseAll(xml);
        }

        /// create node with arbitrary type
        explicit Node(NodeType type, Node * theParent);

        /// create text, comment, cdata  or element node w/o attributes
        Node(NodeType type, const DOMString& name_or_value, Node * theParent);

        virtual ~Node();
        //@}

        /**@name copying & assignment */
        //@{
        /// makes a deep copy
        Node(const Node & n, Node * theParent);
        Node(const Node & n);

        /// makes a deep copy
        Node& operator=(const Node & n);

        /// distinguish between SHALLOW and DEEP copy
        enum CloneDepth {SHALLOW = 0, DEEP = 1};

        /** returns a shallow copy (type, name, value, attributes) or
            a deep copy (type, name, value, attributes, children)
            Note that a shallow copy does a deep copy one the attributes
        */
        virtual NodePtr cloneNode(CloneDepth depth, Node * theParent) const;
        virtual NodePtr cloneNode(CloneDepth depth = DEEP) const {
            return cloneNode(depth, 0);
        }
        //@}

        /**@name member access */
        //@{
        /// returns node type
        /*virtual*/ NodeType nodeType() const {
            return _myType;
        }
        const char * nodeTypeName() const {
            return NodeTypeName[nodeType()];
        }
        /// returns node name
        const DOMString & nodeName() const {
            return _myName;
        }
        /// returns node value as string
        const DOMString & nodeValue() const {
            ensureValue();
            return _myValue->getString();
        }
        asl::Unsigned64 nodeVersion() const {
            return _myVersion;
        }
        asl::Unsigned64 documentVersion() const {
            const Node * topNode = this;
            while (topNode->parentNode()) {
                topNode = topNode->parentNode();
            }
            return topNode->nodeVersion();
        }

        asl::Unsigned64 nodeVersion(asl::Unsigned64 theVersion) {
            return _myVersion = theVersion;
        }

        asl::Unsigned64 bumpVersion();
        void setVersion(asl::Unsigned64 theVersion) {
            _myVersion = theVersion;
            _myAttributes.setVersion(theVersion);
            getChildren().setVersion(theVersion);
        }
        const UniqueId & getUniqueId() const {
            return _myUniqueId;
        }
        /// returns reference of the wrapped value
        /// throws if wrapped value does not have the requested type
        /// close immediately by calling nodeValueRefClose after use
        template <class T>
        T & nodeValueRefOpen() {
            T * myValuePtr = nodeValuePtrOpen<T>();
            if (myValuePtr) {
                return *myValuePtr;
            } else {
                throw TypeMismatch(std::string("Could not cast value of\n") + asl::as_string(*this) +
                                   "\n to type: " + typeid(T).name() + ". Must be type: " + nodeValueTypeName(),
                                   "Node::nodeValueRef()");
            }
        }
        template <class T>
        void nodeValueRefClose() {
            nodeValuePtrClose<T>();
        }

        /// returns const reference of the wrapped value
        /// throws if wrapped value does not have the requested type
        template <class T>
        const T & nodeValueRef() const {
            const T * myValuePtr = nodeValuePtr<T>();
            if (myValuePtr) {
                return *myValuePtr;
            } else {
                throw TypeMismatch(std::string("Could not cast value of\n") + asl::as_string(*this) +
                                   "\n to type: " + typeid(T).name() + ". Must be type: " + nodeValueTypeName(),
                                   "Node::nodeValueRef()");
            }
        }

        /// returns pointer to the wrapped value
        /// returns null if wrapped value does not have the requested type
        /// must not call when already open
        /// close immediately by calling nodeValuePtrClose after use
        template <class T>
        T * nodeValuePtrOpen() {
            ensureValue();
            return dynamic_cast_and_openWriteableValue<T>(&(*_myValue));
        }
        template <class T>
        void nodeValuePtrClose() {
            ensureValue();
            dynamic_cast_and_closeWriteableValue<T>(&(*_myValue));
        }

        template <class T>
        class WritableValue {
            public:
                WritableValue(dom::NodePtr theNode) :
                    _myValue(theNode->nodeValueRefOpen<T>()),
                    _myNode(theNode)
                {}
                ~WritableValue() {
                    _myNode->nodeValueRefClose<T>();
                }
                T & get() {
                    return _myValue;
                }
            private:
                WritableValue();
                WritableValue(const WritableValue &); // not copy-safe!
                WritableValue & operator=(const WritableValue &); // not copy-safe!
                T            & _myValue;
                dom::NodePtr _myNode;
        };


        /// returns const pointer to the wrapped value
        /// returns null if wrapped value does not have the requested type
        template <class T>
        const T * nodeValuePtr() const {
            ensureValue();
            return dynamic_cast_Value<T>(&(*_myValue));
        }

        template <class T>
        T nodeValueAs() const {
            const T * myNodeValuePtr = nodeValuePtr<T>();
            if (myNodeValuePtr) {
                return *myNodeValuePtr;
            }
            return asl::as<T>(nodeValue());
        }

        /// returns pointer to the value wrapper
        ValuePtr nodeValueWrapperPtr()  {
            ensureValue();
            return _myValue;
        }

        /// returns const pointer to the value wrapper
        const ValuePtr nodeValueWrapperPtr() const {
            ensureValue();
            return _myValue;
        }

        // takes ownership of theValue
        void nodeValueWrapperPtr(ValuePtr theValue);

        /// returns a reference to the value wrapper
        ValueBase & nodeValueWrapper()  {
            ensureValue();
            return *_myValue;
        }
        /// returns a const reference to the value wrapper
        const ValueBase & nodeValueWrapper() const {
            ensureValue();
            return *_myValue;
        }

        // copies theValue into our value
        void nodeValueWrapper(const ValueBase & theValue) {
            if (checkValueTypeFits(theValue)) {
                if (_myValue) {
                    _myValue->set(theValue.accessReadableBlock()); // fastest, just copy the data
                } else {
                    _myValue = theValue.clone(this); // make a binary clone
                    _myValue->setNodePtr(this);
                }
            }
            else {
                ensureValue();
                _myValue->setString(theValue.getString()); // perform string conversion
            }
        }

        /** assign a new node value
            @exception DomException(NO_DATA_ALLOWED_ERR)
            if the node type is not element, attribute cdata or comment
        */
        void nodeValue(const DOMString & value);

        template <class T>
        void nodeValueAssign(const T & theT) {
            T & myNodeValue = nodeValueRefOpen<T>();
            myNodeValue = theT;
            nodeValueRefClose<T>();
        }

        const DOMString & nodeValueTypeName() const {
            if (_mySchemaInfo) {
                return _mySchemaInfo->getTypeName();
            }
            static const DOMString ourDefaultType = "string";
            return ourDefaultType;
        }


        /** assign a new node value
            @exception DomException(NO_DATA_ALLOWED_ERR)
            if the node type is not element, attribute cdata or comment
            @return a reference to the new value
        */
        /*virtual*/ Node& operator=(const DOMString & value);
        //@}

        /**@name input and output */
        //@{

        /** print the node and all its children; returns os
        */
        virtual std::ostream& print(std::ostream& os, const DOMString & indent="") const;

        /** parse the xml-data contained in is starting from pos; returns the position
            of the last parsed character; does not propagate parse exceptions
            */
        int parseAll(const DOMString & is);

        void parseFile(const std::string & theFileName);

        /** parse the xml-data contained in is starting from pos; returns the number
            of characters in is or throws a ParseException
        */
        //virtual int Parse(const string& is, int pos, const Node* parent);
        //@}

        /**@name status information */
        //@{
        /** query parsing success returns true if everything was parsed,
            false if parsing was aborted due to malformed xml data
        */
        /*virtual*/ operator bool() const {
            return (nodeType()!=X_NO_NODE) && (_myParseCompletionPos == _myDocSize)
                && (nodeType()!=DOCUMENT_NODE || hasLazyChildren() || getChildren().size());
        }
        /// returns the position past the last character processed Parse()
        /*virtual*/ int parsedUntil() const {
            return _myParseCompletionPos;
        }
        /// returns the size of the last string passed to Parse()
        /*virtual*/ int docSize() const {
            return _myDocSize;
        }
        //@}

        /**@name children access */
        //@{
        /** create or access a child node
        @doc this is the easiest way to access node in the hierachy;
        @return a reference to the n'th child node with the given name.
        if no n'th attribute with the given name exists, a new child node
        with this name is created; by default, an element node is
        created; however, the following special names create nodes
        of other types:
        <ul>
        <li>"#text" -> creates text node
        <li>"#cdata-section" -> creates CDATA section
        <li>"#comment" -> creates comment
        </ul>
        @see appendChild(NodePtr) for possible exceptions
        */
        /*virtual*/ Node& operator()(const DOMString & name, int n = 0);


        /** create or access a child node
        @doc this is the easiest way to access node in the hierachy;
        @return a reference to the n'th child node with the given name.
        if no n'th attribute with the given name exists, an empty node
        with type X_NO_NODE is returned.
        */
        /*virtual*/ const Node& operator()(const DOMString & name, int n = 0) const;


        /** Directly adds a new child;  no copy of child is made;
            @exception DomException(HIERARCHY_REQUEST_ERR) if this node must not have a child of this type;
            @exception DomException(INVALID_NAME_ERR) if name is not properly formed (see checkName )
            @return the value of <em>child</em> passed
        */
        NodePtr appendChild(NodePtr child);

        /** adds a deep copy of child;
            @returns the pointer to the new copy
        */
        NodePtr appendChild(const Node & child) {
            return appendChild(NodePtr(child.cloneNode(DEEP)));
        }
        /// returns what the name promises
        int childNodesLength() const {
            return getChildren().length();
        }
        /// returns what the name promises
        /*virtual*/ bool hasChildNodes() const {
            return getChildren().length() != 0;
        }
        /// returns true if theNode is the same Node
        bool isSameNode(const Node & theNode) const {
            return &(theNode._myType) == &_myType;
        }
        /// returns true if theNode is the same Node
        bool isSameNode(const NodePtr theNode) const {
            return &(theNode->_myType) == &_myType;
        }
        /** @return a read only pointer to node's i'th child
            @exception DomException(INDEX_SIZE_ERR) if i is out of range [0..childNodesLength()-1]
        */
        const NodePtr childNode(int i) const {
            if (i < 0 || i >= getChildren().size()) {
                throw DomException(JUST_FILE_LINE,DomException::INDEX_SIZE_ERR);
            }
            return getChildren().item(i);
        }
        /** @return a pointer to node's i'th child
            @exception DomException(INDEX_SIZE_ERR) if i is out of range [0..childNodesLength()-1]
        */
        NodePtr childNode(int i)  {
            if (i < 0 || i >=getChildren().size()) {
                throw DomException(JUST_FILE_LINE,DomException::INDEX_SIZE_ERR);
            }
            return getChildren().item(i);
        }
        /// returns the number of children of this node with this name
        int childNodesLength(const DOMString & name) const {
            return NamedNodeMap::countNodesNamed(name,getChildren());
        }
        /** @return a read only pointer to node's i'th child
            of this node with this name or 0 if it does not exist
        */
        const NodePtr childNode(const DOMString & name, int n) const {
            int i = NamedNodeMap::findNthNodeNamed(name,n,getChildren());
            if (i<getChildren().size()) {
                return getChildren().item(i);
            }
            return NodePtr(0);
        }
        // using this wrapper instead of default argument to allow member function pointers
        const NodePtr childNode(const DOMString & name) const {
            return childNode(name,0);
        }
        /** @return a pointer to node's i'th child
            of this node with this name or 0 if it does not exist
        */
        NodePtr childNode(const DOMString & name, int n) {
            int i = NamedNodeMap::findNthNodeNamed(name,n,getChildren());
            if (i<getChildren().size()) {
                return getChildren().item(i);
            }
            return NodePtr(0);
        }
        // using this wrapper instead of default argument to allow member function pointers
        NodePtr childNode(const DOMString & name) {
            return childNode(name,0);
        }
        NodeList & childNodes() {
            return getChildren();
        }
        const NodeList & childNodes() const {
            return getChildren();
        }
        Node * parentNode() {
            return _myParent;
        }
        const Node * parentNode() const {
            return _myParent;
        }
        NodeWeakPtr self() {
            if (!_mySelf) {
                if (_myParent) {
                    NodePtr myNode;
                    if (_myType == ATTRIBUTE_NODE) {
                        int myIndex = _myParent->attributes().findIndex(this);
                        if (myIndex>=0) {
                            _mySelf = _myParent->attributes().item(myIndex);
                        }
                    } else {
                        int myIndex = _myParent->childNodes().findIndex(this);
                        if (myIndex>=0) {
                            _mySelf = _myParent->childNodes().item(myIndex);
                        }
                    }
                }
            }
            return _mySelf;
        }
        const NodeWeakPtr & self() const {
            if (!_mySelf) {
                const_cast<Node*>(this)->self();
            }
            return _mySelf;
        }

        void self(NodeWeakPtr theSelf) {
            _mySelf = theSelf;
        }

        NodePtr nextSibling();
        const NodePtr nextSibling() const;
        NodePtr previousSibling();
        const NodePtr previousSibling() const;

        NodePtr firstChild() {
            if (getChildren().size()) {
                return getChildren().item(0);
            }
            return NodePtr(0);
        }
        const NodePtr firstChild() const {
            if (getChildren().size()) {
                return getChildren().item(0);
            }
            return NodePtr(0);
        }
        const NodePtr lastChild() const {
            if (getChildren().size()) {
                return getChildren().item(getChildren().size()-1);
            }
            return NodePtr(0);
        }
        NodePtr lastChild() {
            if (getChildren().size()) {
                return getChildren().item(getChildren().size()-1);
            }
            return NodePtr(0);
        }
        NodePtr removeChild(dom::NodePtr theChild);
        NodePtr replaceChild(NodePtr theNewChild, NodePtr theOldChild);
        NodePtr insertBefore(NodePtr theNewChild, NodePtr theRefChild);

       //@}

        /**@name attribute access */
        //@{

        /** returns reference to attribute node with the given name
            if no attribute with the given name exists, a new attribute
            node with this name is created. Use operator=(string) to assign a value.
            @exception DomException(INVALID_NAME_ERR) if name is malformed
            @exception DomException(HIERARCHY_REQUEST_ERR) if this node is not an element node
        */
        Node& operator[](const DOMString & name);

        /** returns reference to attribute node with the given name
            if no attribute with the given name exists, a new attribute
            node with this name is created.
            @exception DomException(INVALID_NAME_ERR) if name is malformed
            @exception DomException(HIERARCHY_REQUEST_ERR) if this node is not an element node
        */
        Node& operator[](const char* name) {
            return (*this)[DOMString(name)];
        }
        /**
            returns read-only reference to attribute node with the given name
            if no attribute with the given name exists, a reference to an
            empty node of type X_NO_NODE is returned.
            @exception DomException(INVALID_NAME_ERR) if name is malformed
            @exception DomException(HIERARCHY_REQUEST_ERR) if this node is not an element node
        */
        const Node& operator[](const DOMString & name) const;

        /**
            returns read-only reference to attribute node with the given name
            if no attribute with the given name exists, a reference to an
            empty node of type X_NO_NODE is returned.
            @exception DomException(INVALID_NAME_ERR) if name is malformed
        */
        const Node& operator[](const char* name) const {
            return (*this)[DOMString(name)];
        }

        /**
            directly adds an attribute - no copy is made of attribute.
            @exception DomException(INUSE_ATTRIBUTE_ERR) if name already exists
            @exception DomException(INVALID_NAME_ERR) if name is malformed
            @exception DomException(HIERARCHY_REQUEST_ERR) if:
            <ul><li> NodePtr does not point to an attribute node
            <li> this node is not an element node
            </ul>
            @see checkName allowedToHaveChild
        */
        NodePtr appendAttribute(NodePtr attribute);

        /**
            adds a deep copy of attribute; returns the pointer to the new copy
            @exception DomException
            @see appendAttribute(NodePtr) for details on exceptions
        */
        NodePtr appendAttribute(const Node& attribute) {
            return appendAttribute(NodePtr(attribute.cloneNode(DEEP)));
        }
        /** add a new attribute with name and value; value is automatically
            converted to string if operator<<(ostream&) is defined for VALUE_TYPE
            returns a pointer to the new attribute node
            @exception DomException
            @see appendAttribute(NodePtr) for details on exceptions
        */
        template <class VALUE_TYPE>
        NodePtr appendAttribute(const DOMString & theName,const VALUE_TYPE& theValue) {
            NodePtr myNewAttribute = appendAttribute(theName);
            VALUE_TYPE * myNodeValue = myNewAttribute->nodeValuePtrOpen<VALUE_TYPE>();
            if (myNodeValue) {
                *myNodeValue = theValue;
                 myNewAttribute->nodeValuePtrClose<VALUE_TYPE>();
                return myNewAttribute;
            }
            myNewAttribute->nodeValue(asl::as_string(theValue));
            return myNewAttribute;
        }

        /** add a new attribute with name and value.
            @return a pointer to the new attribute node
            @exception DomException
            @see appendAttribute(NodePtr) for details on exceptions
        */
        NodePtr appendAttribute(const DOMString & name,const DOMString & value) {
            return appendAttribute(NodePtr(new Node(ATTRIBUTE_NODE,name,value,this)));
        }
        NodePtr appendAttribute(const char * name,const char * value) {
            return appendAttribute(DOMString(name),DOMString(value));
        }

        /** add a new attribute with name and no value.
            @return a pointer to the new attribute node
            @exception DomException
            @see appendAttribute(NodePtr) for details on exceptions
        */
        NodePtr appendAttribute(const DOMString & name) {
            return appendAttribute(NodePtr(new Node(ATTRIBUTE_NODE,name,dom::ValuePtr(0),this)));
        }

        /// returns the number of attribute nodes in this node
        int attributesLength() const {
            return _myAttributes.size();
        }

        /** @return a read only pointer to the i'th attribute node
            @exception DomException(INDEX_SIZE_ERR) if i is out of range [0..attributesLength()-1]
        */
        const NodePtr getAttribute(int i) const {
            if (i < 0 || i >=_myAttributes.size())
                throw DomException(JUST_FILE_LINE,DomException::INDEX_SIZE_ERR);
            return _myAttributes.item(i);
        }

        /** @return a pointer to the i'th attribute node
            @exception DomException(INDEX_SIZE_ERR) if i is out of range [0..attributesLength()-1]
        */
        NodePtr getAttribute(int i) {
            if (i < 0 || i >=_myAttributes.size())
                throw DomException(JUST_FILE_LINE,DomException::INDEX_SIZE_ERR);
            return _myAttributes.item(i);
        }
        /// returns the number of attribute nodes in this node with this name
        int attributesLength(const DOMString & name) const {
            return NamedNodeMap::countNodesNamed(name,_myAttributes);
        }

        /** returns a read only pointer to the attribute node with this name
            or 0 if no attribute with given name exists
        */
        const NodePtr getAttribute(const DOMString & name) const {
            int i = NamedNodeMap::findNthNodeNamed(name,0,_myAttributes);
            if (i<_myAttributes.size()) return _myAttributes.item(i);
            return NodePtr(0);
        }

        /** returns a pointer to the attribute node with this name
            or 0 if no attribute with given name exists
        */
        NodePtr getAttribute(const DOMString & name) {
            int i = NamedNodeMap::findNthNodeNamed(name,0,_myAttributes);
            if (i<_myAttributes.size()) return _myAttributes.item(i);
            return NodePtr(0);
        }

        /**
         * returns a typed value of the attribute node value with this name
         * throws an exception if the attribute is not present.
         */
        template <class T>
        T getAttributeValue(const DOMString & name) const {
            const NodePtr myAttribute = getAttribute(name);
            if (myAttribute) {
                return myAttribute->nodeValueAs<T>();
            } else {
                throw DomException(std::string("Attribute '") + name + "' not found.", PLUS_FILE_LINE, DomException::NOT_FOUND_ERR);
            }
        }

        /**
         * returns a typed value of the attribute node value with this name
         * returns the given default value if the attribute is not present.
         */
        template <class T>
        T getAttributeValue(const DOMString & name, const T & theDefault) const {
            const NodePtr myAttribute = getAttribute(name);
            if (myAttribute) {
                return myAttribute->nodeValueAs<T>();
            } else {
                return theDefault;
            }
        }

        /// returns a pointer typed value of the attribute node value with this name
        const ValuePtr getAttributeValueWrapperPtr(const DOMString & name) const {
            const NodePtr myAttribute = getAttribute(name);
            if (myAttribute) {
                return myAttribute->nodeValueWrapperPtr();
            } else {
                return ValuePtr(0);
            }
        }
        /// returns a pointer typed value of the attribute node value with this name
        ValuePtr getAttributeValueWrapperPtr(const DOMString & name) {
            NodePtr myAttribute = getAttribute(name);
            if (myAttribute) {
                return myAttribute->nodeValueWrapperPtr();
            } else {
                return ValuePtr(0);
            }
        }

        /// returns a read only string of the attribute node value with this name
        const DOMString & getAttributeString(const DOMString & name) const {
            const NodePtr myAttribute = getAttribute(name);
            if (myAttribute) {
                return myAttribute->nodeValue();
            } else {
                throw DomException(std::string("Attribute not found: ") + name +
                                   " in node: " + nodeName(),
                                   PLUS_FILE_LINE, DomException::NOT_FOUND_ERR);
            }
        }

        NamedNodeMap & attributes() {
            return _myAttributes;
        }
        const NamedNodeMap & attributes() const {
            return _myAttributes;
        }
        //@}
         asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos = 0, OpMode myMode = IMMEDIATE) {
            bool myUnmodifiedProxyFlag;
            Dictionaries myDicts;
            asl::AC_SIZE_TYPE myPos = debinarize(theSource, thePos, myDicts, myMode, myUnmodifiedProxyFlag);
            setVersion(1);
            return myPos;
        }
        void binarize(asl::WriteableStream & theDest) const {
            Dictionaries myDicts;
            binarize(theDest, myDicts, 0, B60_MAGIC);
        }
         void binarize(asl::WriteableStream & theDest, Dictionaries & theDicts, asl::Unsigned32 theMagic) const {
            binarize(theDest, theDicts, 0, theMagic);
        }
        // binarize and write an id/offset catalog for random node access
        // when theDataDest and theCatalogDest are the same stream, then
        // the catalog is appended to the data file
        // the last 8 Bytes are the size of the catalog including these 8 bytes
        // thus the catalog can be found by reading the last 8 bytes of the file
        // function will binarize from the root node of the DOM even when called
        // on a child to avoid partial catalogs
        void binarize(asl::WriteableStream & theDataDest, asl::WriteableStream & theCatalogDest) const;
       
        // debinarize using an existing dictionary which is needed for random element access
        asl::AC_SIZE_TYPE debinarize(Dictionaries & theDictionaries,
                                       const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos) {
            
            bool myUnmodifiedProxyFlag;
            asl::AC_SIZE_TYPE myPos = debinarize(theSource, thePos, theDictionaries, IMMEDIATE, myUnmodifiedProxyFlag);
            return myPos;
        }
        /// debinarize lazy using an existing catalog appended to the source file
        bool debinarizeLazy(asl::Ptr<asl::ReadableStreamHandle> theSource);
        /// debinarize lazy using an existing catalog from a separete catalog file
        bool debinarizeLazy(asl::Ptr<asl::ReadableStreamHandle> theSource, asl::Ptr<asl::ReadableStreamHandle> theCatalogSource);

        NodePtr loadElementById(const DOMString & theId, const DOMString & theIdAttribute,
                                const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos,
                                Dictionaries & theDictionaries,
                                const NodeOffsetCatalog & theCatalog)
        {
            asl::Unsigned64 myOffset = 0;
            if (theCatalog.getElementOffsetById(theId, theIdAttribute, myOffset)) {
               
               debinarize(theDictionaries, theSource, thePos + asl::AC_SIZE_TYPE(myOffset));  
               return self().lock();
            }
            return NodePtr(0); 
        }
          
        void makePatch(asl::WriteableStream & thePatch, asl::Unsigned64 theOldVersion) const {
            Dictionaries myDicts;
            binarize(thePatch, myDicts, theOldVersion + 1, P60_MAGIC);
        }
        /// return true if something has changed
        bool applyPatch(const asl::ReadableStream & thePatch, asl::AC_SIZE_TYPE thePos = 0) {
            bool myUnmodifiedProxyFlag = false;
            Dictionaries myDicts;
            debinarize(thePatch, thePos, myDicts, PATCH, myUnmodifiedProxyFlag);
            return !myUnmodifiedProxyFlag;
        }
        virtual void freeCaches() const;
        virtual bool flushUnusedChildren() const;
    protected:
        NodePtr loadPathById(const DOMString & theId, const DOMString & theIdAttribute);
        void collectOffsets(NodeOffsetCatalog & theCatalog, asl::AC_SIZE_TYPE theParentIndex = asl::AC_SIZE_TYPE(-1)) const {
            theParentIndex = theCatalog.enterUID(_myUniqueId, _mySavePosition, _mySaveEndPosition, theParentIndex);
            for (unsigned i = 0; i < getChildren().size(); ++i) {
                getChildren()[i].collectOffsets(theCatalog, theParentIndex);
            }
        }
        asl::AC_SIZE_TYPE debinarize(const asl::ReadableStream & theSource, asl::AC_SIZE_TYPE thePos, Dictionaries & theDict, OpMode theLoadMode, bool & theUnmodifiedProxyFlag);
        void binarize(asl::WriteableStream & theDest, Dictionaries & theDict, asl::Unsigned64 theIncludeVersion) const;
        void binarize(asl::WriteableStream & theDest, Dictionaries & theDict, asl::Unsigned64 theIncludeVersion, asl::Unsigned32 theMagic) const;
        void unregisterName();
        void registerName();
    public:
        void addSchema(const DOMString & theSchemaString, const DOMString & theNSPrefix);
        void addSchema(const dom::Node & theSchemaDoc, const DOMString & theNSPrefix);
        void loadSchemas(const DOMString & theURLS);
        void checkSchemas() const;
        const SchemaPtr getSchema() const;
        void setValueFactory(asl::Ptr<ValueFactory> theFactory) {
            makeSchemaInfo(false);
            _mySchemaInfo->_myValueFactory = theFactory;
        }
        asl::Ptr<ValueFactory> getValueFactory() {
            makeSchemaInfo(false);
            return _mySchemaInfo->_myValueFactory;
        }

        const asl::Ptr<ValueFactory> getValueFactory() const {
            return _mySchemaInfo->_myValueFactory;
        }

        void setFacadeFactory(FacadeFactoryPtr theFactory) {
            _mySchemaInfo->_myFacadeFactory = theFactory;
        }

        FacadeFactoryPtr getFacadeFactory() {
            if (_mySchemaInfo) {
                return _mySchemaInfo->_myFacadeFactory;
            } else {
                return FacadeFactoryPtr(0);
            }
        }
        const FacadeFactoryPtr getFacadeFactory() const {
            if (_mySchemaInfo) {
                return _mySchemaInfo->_myFacadeFactory;
            } else {
                return FacadeFactoryPtr(0);
            }
        }

        /// returns whether node has can have a facade object by trying to create one; as a side effect a facade is created
        // TODO: should be possibly rename in "tryFacade"
        bool hasFacade() const;

        /// returns facade object; if none exists yet, it is created; if it cant be created, an exception is thrown
        /// you can check with hasFacade() to avoid exception
        FacadePtr getFacade();
        const FacadePtr getFacade() const;
        
        /// return true if a facade has been already constructed
        bool hasExistingFacade() const;
            
        template <class TARGET_FACADE>
        asl::Ptr<TARGET_FACADE, dom::ThreadingModel> tryGetFacade() {
            return dynamic_cast_Ptr<TARGET_FACADE>(getFacade());
        }

        template <class TARGET_FACADE>
        const asl::Ptr<TARGET_FACADE, dom::ThreadingModel> tryGetFacade() const {
            return dynamic_cast_Ptr<TARGET_FACADE>(getFacade());
        }

        template <class TARGET_FACADE>
        asl::Ptr<TARGET_FACADE, dom::ThreadingModel> getFacade() {
            asl::Ptr<TARGET_FACADE, dom::ThreadingModel> myFacade = dynamic_cast_Ptr<TARGET_FACADE>(getFacade());
            if (!myFacade) {
                throw Facade::Exception("Dynamic cast of facade failed", PLUS_FILE_LINE);
            }
            return myFacade;
        }

        template <class TARGET_FACADE>
        const asl::Ptr<TARGET_FACADE, dom::ThreadingModel> getFacade() const {
            const asl::Ptr<TARGET_FACADE, dom::ThreadingModel> myFacade = dynamic_cast_Ptr<TARGET_FACADE>(getFacade());
            if (!myFacade) {
                throw Facade::Exception("Dynamic cast of facade failed", PLUS_FILE_LINE);
            }
            return myFacade;
        }

         template <class TARGET_FACADE>
         std::vector<asl::Ptr<TARGET_FACADE, dom::ThreadingModel> > getAllFacades(const std::string & theElementName) {
            std::vector<asl::Ptr<TARGET_FACADE, dom::ThreadingModel> > myResult;
            NodeIDRegistry::NodeSet * myNodes = getIDRegistry()->getNodes(theElementName);
            if (myNodes) {
                for (NodeIDRegistry::NodeSet::iterator it = myNodes->begin(); it!=myNodes->end();++it) {
                    myResult.push_back((*it)->self().lock()->getFacade<TARGET_FACADE>());
                }
            }
            return myResult;
        }

        const NodePtr childNodeByAttribute(const DOMString & theElementName,
                                           const DOMString & theAttributeName,
                                           const DOMString & theAttributeValue,
                                           int theIndex = 0) const;
        NodePtr childNodeByAttribute(const DOMString & theElementName,
                                           const DOMString & theAttributeName,
                                           const DOMString & theAttributeValue,
                                           int theIndex = 0);
        void  getNodesByTagName(const DOMString & theElementName,
                                bool theDeepSearchFlag,
                                std::vector<NodePtr> & theResults) const;
        void  getNodesByAttribute(const DOMString & theElementName,
                                  const DOMString & theAttributeName,
                                  const DOMString & theAttributeValue,
                                  bool theDeepSearchFlag,
                                  std::vector<NodePtr> & theResults) const;

        const NodePtr getChildElementById(const DOMString & theId, const DOMString & theIdAttribute) const;
        NodePtr getChildElementById(const DOMString & theId, const DOMString & theIdAttribute);
        const NodePtr getElementById(const DOMString & theId, const DOMString & theIdAttribute="id") const;
        NodePtr getElementById(const DOMString & theId, const DOMString & theIdAttribute="id");

        /// Does not return the document node, but the root element node
        /// A better name would is getRootElement(), but the name getRootNode() is defined
        /// in the dom-standard. Use "getRealRootNode()" or getDocumentNode() to get the document node.
        const Node * getRootNode() const;
        Node * getRootNode();
        const Node * getRealRootNode() const;
        Node * getRealRootNode();
        
        // some better names for above functions
        const Node * getRootElement() const {return getRootNode();}
        Node * getRootElement() {return getRootNode();}
        const Node * getDocumentNode() const {return getRealRootNode();}
        Node * getDocumentNode() {return getRealRootNode();}

        // EventTarget Interface
        void addEventListener(const DOMString & type,
                              EventListenerPtr listener,
                              bool useCapture);
        void removeEventListener(const DOMString & type,
                                 EventListenerPtr listener,
                                 bool useCapture);


        // call dispatchEvent on the target node to start event processing
        // this function does a recursive capture and bubbling dispatch on
        // the event target node hierarchy.
        // As specified in the W3C-standard, the order of the capturing and
        // bubbling phase node traversal order is determined ininially;
        // if any changes to the tree are made during traversal, they do not
        // affect the order of nodes visited with this event
        bool dispatchEvent(EventPtr evt);

        const NodeIDRegistryPtr getIDRegistry() const {
            if (_myParent) {
                return _myParent->getIDRegistry();
            }
            if (!_myIDRegistry) {
                _myIDRegistry = NodeIDRegistryPtr(new NodeIDRegistry);
            }
            return _myIDRegistry;
        }
        const NodeIDRefRegistryPtr getIDRefRegistry() const {
            if (_myParent) {
                return _myParent->getIDRefRegistry();
            }
            if (!_myIDRefRegistry) {
                _myIDRefRegistry = NodeIDRefRegistryPtr(new NodeIDRefRegistry);
            }
            return _myIDRefRegistry;
        }
/**
@memo used internally to determine if it is allowed to add a child of the particular type
@return true if it is allowed to add a child of type
@doc used internally to determine if if it is allowed to add a child of type<p>
Dependent on node type allowed children are:<p>
<ul>
<li>Document -- Element (maximum of one), ProcessingInstruction, Comment, DocumentType
<li>DocumentFragment -- Element, ProcessingInstruction, Comment, Text, CDATASection, EntityReference
<li>DocumentType -- Notation, Entity
<li>EntityReference -- Element, ProcessingInstruction, Comment, Text, CDATASection, EntityReference
<li>Element -- Element, Text, Comment, ProcessingInstruction, CDATASection, EntityReference
<li>Attribute -- Text, EntityReference
</ul>
<p>
*/
        bool allowedToHaveChild(NodeType type) const;

        /// returns true if node is a Attribute, Comment, Text, PI or CDATA Node
        bool mayHaveValue() const;

        static
        DOMString  getNameOfType(NodeType type);
        void printChangedNodes(const std::string & theLastVersion) const;
        void printChangedNodes(asl::Unsigned64 theLastVersion= 0xffffffffffffffffULL, int theLevel=0) const;
    protected:
        typedef std::map<EventListener*,EventListenerPtr> EventListenerSet;
        typedef std::map<DOMString, EventListenerSet> EventListenerMap;

        void callListeners(EventListenerMap & theListeners, EventPtr evt);

        NodeIDRegistryPtr getIDRegistry() {
            if (_myParent) {
                return _myParent->getIDRegistry();
            }
            if (!_myIDRegistry) {
                _myIDRegistry = NodeIDRegistryPtr(new NodeIDRegistry);
            }
            return _myIDRegistry;
        }
        NodeIDRefRegistryPtr getIDRefRegistry() {
            if (_myParent) {
                return _myParent->getIDRefRegistry();
            }
            if (!_myIDRefRegistry) {
                _myIDRefRegistry = NodeIDRefRegistryPtr(new NodeIDRefRegistry);
            }
            return _myIDRefRegistry;
        }
    protected:
        void getReferencingNodes(std::vector<NodePtr> & theResult);
        void setUpstreamVersion(asl::Unsigned64 theVersion);
        int parseSystemAttributes(const DOMString & is, int pos, int end_pos, const Node * doctype);
        NodePtr checkSchemaForElement(const DOMString & theName, asl::AC_SIZE_TYPE theParsePos);
        void checkSchemaForText(asl::AC_SIZE_TYPE theParsePos);
        const NodePtr getAttributeDeclaration(const DOMString & theAttributeName) const;
        //NodePtr getAttributeDeclaration(const DOMString & theAttributeName);

        /** set node name, must start with alpha, '_' or ':'
            and continue with alpha, digit, '_' , '-' or ':'
        */
        virtual void setName(const DOMString & name);

        int getNumberOfChildrenWithType(NodeType type) const;

        /**@name internal paramter checking*/
        //@{
        /** check name conformity. name must start with alpha, '_' or ':'
            and continue with alpha, digit, '_' , '-' or ':'
        */
        static void checkName(const DOMString & name, NodeType type);

 
        void reparent(Node * theNewParent, Node * theTopNewParent);

        int parseNextNode(const DOMString & is, int pos,
                             const Node * parent, const Node * doctype);
        // parse eg.  name="bla" width="10"
        int parseAttributes(const DOMString & is, int pos, int global_pos, const Node * doctype);

        void makeSchemaInfo(bool forceNew);

        void ensureValue() const;
        void assignValue(const asl::ReadableBlock & theValue);
        void trashValue() {
            _myValue = ValuePtr(0);
        }
        bool checkMyValueTypeFits() const;
        bool checkValueTypeFits(const ValueBase & theValue) const;
        void checkAndUpdateChildrenSchemaInfo(Node & theNewChild, Node * theTopNewParent);
        void checkAndUpdateAttributeSchemaInfo(Node & theNewAttribute, Node * theTopNewParent);

        void checkForArrayType(NodePtr mySchemaType, asl::AC_SIZE_TYPE theParsePos);
        void storeValue(const DOMString & theValue);

        // store file positions for saving
        void storeSavePosition(asl::Unsigned64 thePosition) const {
            _mySavePosition = thePosition;
        }
        asl::Unsigned64 getSavePosition() const {
            return _mySavePosition;
        }
        void storeSaveEndPosition(asl::Unsigned64 thePosition) const {
            _mySaveEndPosition = thePosition;
        }
        asl::Unsigned64 getSaveEndPosition() const {
            return _mySaveEndPosition;
        }
        void storeChildrenPosition(asl::Unsigned64 thePosition) const {
            _myChildrenPosition = thePosition;
        }
        asl::Unsigned64 getChildrenPosition() const {
            return _myChildrenPosition;
        }
        void loadChildren();
        NodeList & getChildren() {
            if (_lazyChildren) {
                loadChildren();
            }
            return _myChildrenList;
        }
        const NodeList & getChildren() const {
            if (_lazyChildren) {
                const_cast<Node&>(*this).loadChildren();
            }
            return _myChildrenList;
        }
    public:
        // main provided for debugging purposes
        bool hasLazyChildren() const {
            return _lazyChildren;
        }
       //@}
    private:
        void markPrecursorDependenciesOutdated();

        NodeType          _myType;
        mutable bool      _lazyChildren;
        DOMString         _myName;
        Node *            _myParent;
        NodeWeakPtr       _mySelf;
        mutable NodeIDRegistryPtr _myIDRegistry;
        mutable NodeIDRegistryWeakPtr _myNameRegistry;
        mutable NodeIDRefRegistryPtr _myIDRefRegistry;
        ValuePtr          _myValue;
        mutable NodeList  _myChildrenList; // entities when doctype
        TypedNamedNodeMap _myAttributes; // notations when doctype
        mutable asl::Unsigned32  _myParseCompletionPos;
        mutable asl::Unsigned32  _myDocSize;
        SchemaInfoPtr     _mySchemaInfo;
        mutable FacadePtr _myFacade;
        EventListenerMap  _myEventListeners;
        EventListenerMap  _myCapturingEventListeners;
        static const Node X_NO_NODE_;
        asl::Unsigned64   _myVersion;
        UniqueId          _myUniqueId;
        mutable asl::Unsigned64   _mySavePosition;
        mutable asl::Unsigned64   _myChildrenPosition;
        mutable asl::Unsigned64   _mySaveEndPosition;
    public:
        static Node Prototype;
    };

    /** standard output operator for node type
    @see dom::Node::print for details
    */
    inline
        std::ostream& operator<<(std::ostream& os, const Node& n) {
        return n.print(os,"");
    }
    /** standard output operator for node type
    @see dom::Node::Parse for details
    */
    std::istream& operator>>(std::istream& is, Node& n);



    // allowed children:
    // Element (maximum of one), ProcessingInstruction, Comment, DocumentType
    /// use to create a document node
    class Document : public Node {
    public:
        DEFINE_NESTED_EXCEPTION(Document,Exception,asl::Exception);
      /// create empty document node
        Document()
            : Node(DOCUMENT_NODE, 0) {}
        /// create document node from xml string
        Document(const DOMString & xml)
            : Node(DOCUMENT_NODE, 0) {
            Node::parseAll(xml);
        }
        Document & operator=(const Node& n) {
            if (n.nodeType() == DOCUMENT_NODE || n.nodeType() == X_NO_NODE) {
                this->Node::operator=(n);
                return *this;
            }
            throw Exception("Can not assign node of bad type to dom::Document", PLUS_FILE_LINE);
        }
        bool parse(const DOMString & xml) {
            Node::parseAll(xml);
            return operator bool();
        }
    };

    /// use to create a document-fragment node, serving as a "invisible" root node for children
    class DocumentFragment : public Node {
    public:
        /// create empty document-fragment node
        DocumentFragment(Node * theParent = 0)
            : Node(DOCUMENT_FRAGMENT_NODE, theParent) {}
        /// create document-fragment node from xml string
        DocumentFragment(const DOMString & xml)
            : Node(DOCUMENT_FRAGMENT_NODE, 0) {
            Node::parseAll(xml);
        }
        /// fill document fragment node from xml string, true if successful
        bool parse(const DOMString & xml) {
            Node::parseAll(xml);
            return operator bool();
        }
    };

    /// use to create a attribute node, serving as a "invisible" root node for children
    class Attribute : public Node {
    public:
        /// create empty attribute node
        Attribute(Node * theParent = 0)  {}
        /** create attribute with @param name and @param value
        */
        Attribute(const DOMString & theName,const DOMString & theValue, Node * theParent = 0)
            : Node(ATTRIBUTE_NODE, theName, theValue, theParent) {}

        Attribute(const DOMString & theName, ValuePtr theValue, Node * theParent = 0)
            : Node(ATTRIBUTE_NODE, theName, theValue, theParent) {}

    };
    /// use to create an entity node
    class Entity : public Node {
    public:
        /// create empty attribute node
        Entity(Node * theParent = 0) : Node(ENTITY_NODE, theParent) {}
        /** create entity with @param name and @param value
        */
        Entity(const DOMString & name,const DOMString & value, Node * theParent = 0)
            : Node(ENTITY_NODE, name, value, theParent) {}

    };
    /// use to create a document type node, use as child for a document node and as container for entities
    class DocumentType : public Node {
    public:
        /// create empty document type node with name "#undefined"
        DocumentType(Node * theParent = 0) : Node(DOCUMENT_TYPE_NODE, "#undefined", theParent) {}
        /// create element with given <b>name</b>
        DocumentType(const DOMString & name, Node * theParent = 0)
            : Node(DOCUMENT_TYPE_NODE,name, theParent) {}
    };

    /// use to create a entity reference node
    class EntityReference : public Node {
    public:
        /// create empty document type node with name "#undefined"
        EntityReference(Node * theParent = 0) : Node(ENTITY_REFERENCE_NODE, "#undefined", theParent) {}
        /// create element with given <b>name</b>
        EntityReference(const DOMString & name, Node * theParent = 0)
            : Node(ENTITY_REFERENCE_NODE,name, theParent) {}
    };

    /// use to create a element node, the most frequent node type
    class Element : public Node {
    public:
        /// create empty element node with name "#undefined"
        Element(Node * theParent = 0) : Node(ELEMENT_NODE, "#undefined", theParent) {}
        /// create element with given <b>name</b>
        Element(const DOMString & name, Node * theParent = 0)
            : Node(ELEMENT_NODE,name, theParent) {}
        /// create element with given <b>name</b> and <b>attributes</b>
        Element(const DOMString & name, const NamedNodeMap & attributes, Node * theParent = 0)
            : Node(name,attributes, theParent) {
        }
    };
/*
    // Element that contains a number of child elements representing some Value of Type T
    template <class T>
    class Array : public Element {
        Array(const DOMString & theArrayName, const DOMString & theChildElementsName) {
        }
    private:
        std::vector<T> _myArray;
    };
*/

    /// use to create a comment node
    class Comment : public Node {
    public:
        /// create empty comment node
        Comment(Node * theParent = 0) : Node(COMMENT_NODE, theParent) {}
        /// create comment node with given <b>value</b>
        Comment(const DOMString & value, Node * theParent = 0)
            : Node(COMMENT_NODE, value, theParent) {}
    };

    /// use to create a text node
    class Text : public Node {
    public:
        /// create empty text node
        Text(Node * theParent = 0) : Node(TEXT_NODE, theParent)  {}
        /// create text node with given <b>value</b>
        Text(const DOMString & value, Node * theParent = 0)
            : Node(TEXT_NODE, value, theParent) {}
        Text(const ValueBase & theValue, Node * theParent = 0)
            : Node(TEXT_NODE, "#text", theValue, theParent) {}
        Text(ValuePtr theValuePtr, Node * theParent = 0)
            : Node(TEXT_NODE, "#text", theValuePtr, theParent) {}
    };

    /// use to create a processing instruction node
    class ProcessingInstruction : public Node {
    public:
        /// create standard xml processing instruction node
        ProcessingInstruction(Node * theParent = 0)
            : Node(PROCESSING_INSTRUCTION_NODE,"xml","version='1.0'", theParent) {}
        /// create processing instruction node with given <b>target</b> and <b>value</b>
        ProcessingInstruction(const DOMString & target,const DOMString & value, Node * theParent = 0)
            : Node(PROCESSING_INSTRUCTION_NODE,target,value, theParent) {}
    };

    /// use to create a cdata node
    class CDataSection : public Node {
    public:
        /// create empty cdata node
        CDataSection(Node * theParent = 0) : Node(CDATA_SECTION_NODE, theParent) {}
        /// create cdata node with given <b>value</b>
        CDataSection(const DOMString & value, Node * theParent = 0)
            : Node(CDATA_SECTION_NODE, value, theParent) {}
    };

    /** replaces every attribute named attribute_name in every element node reachable from dom
    and named node_name with new_attribute_value
    @doc if the current attribute value
    matches old_attribute_value; a single "*" can be used as wildcard in node_name and
    old_attribute_value parameter, matching every string.
    @return the number of attributes replaced
    */
    int replace_attributes(Node& dom,
        const DOMString & node_name,
        const DOMString & attribute_name,
        const DOMString & old_attribute_value,
        const DOMString & new_attribute_value);

    inline
    unsigned int countNodes(const Node & theDom) {
        unsigned int myNodeCount = 1 + theDom.attributesLength();
        for (int i = 0; i < theDom.childNodesLength();++i) {
            myNodeCount += countNodes(*theDom.childNode(i));
        }
        return myNodeCount;
    }

 
    /* @} */

} //Namespace dom

// Some code
#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
