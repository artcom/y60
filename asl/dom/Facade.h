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
//   $RCSfile: Facade.h,v $
//
//   $Revision: 1.11 $
//
//   Description:
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _xml_Facade_h_
#define _xml_Facade_h_

#include "asl_dom_settings.h"

#include "typedefs.h"
#include "NodeList.h"
#include "Value.h"

#include <asl/base/Exception.h>
#include <asl/base/Ptr.h>

#include <map>
#include <set>

namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

    class Node;

    class Facade;
    typedef asl::Ptr<Facade,ThreadingModel> FacadePtr;
    typedef asl::WeakPtr<Facade,ThreadingModel> FacadeWeakPtr;

    class ASL_DOM_EXPORT Facade : public TypedNamedNodeMap {
        public:
            DEFINE_NESTED_EXCEPTION(Facade, Exception, asl::Exception);
            DEFINE_NESTED_EXCEPTION(Facade, InvalidNullPointerPassed, Exception);
            DEFINE_NESTED_EXCEPTION(Facade, NoParentNode, Exception);
            DEFINE_NESTED_EXCEPTION(Facade, DuplicateChildName, Exception);

            virtual ~Facade() {}

            virtual Facade * createNew(Node & theNode) const = 0;

            Node & getNode() {
                return _myNode;
            }
            const Node & getNode() const {
                return _myNode;
            }

            virtual void setSelf(FacadePtr theSelf);
            virtual FacadePtr getSelf() {
                return _mySelf.lock();
            }
            virtual int registerDependencies() {return 0;}
            virtual void registerDependenciesRegistrators() {}

            void markPrecursorDependenciesOutdated();
            void markAllDirty();

            void registerChildName(const std::string & theChildName);
            bool hasRegisteredChild(const std::string & theChildName) const;
            
            typedef std::map<std::string, NodePtr> PropertyMap;
            PropertyMap & getProperties() const;
            NodePtr getProperty(const std::string & theName) const;

            static NodePtr ensureChild(const Node & theNode, const DOMString & theName);
            static NodePtr createChild(const Node & theNode, const DOMString & theName);

            const NodePtr getChildNode(const DOMString & theName) const;
            NodePtr getChildNode(const DOMString & theName);
        protected:
           Facade(Node & theNode);
           virtual void ensureProperties() const {};
           mutable PropertyMap   _myPropertyNodes; 

           void setNode( Node & theNode);

        private:
           Facade(); // no default constructor. Otherwise the member
           // _myNode points to nirvana [DS]

           std::set<std::string>          _myChildNames;             
           Node &                         _myNode;
           FacadeWeakPtr                  _mySelf; 
    }; 

#define IMPLEMENT_FACADE(CLASS) \
    dom::Facade * \
    createNew(dom::Node & theNode) const { \
        return new CLASS(theNode); \
    } \
    template <class TAG> \
    const typename TAG::TYPE & get() const { \
        TAG::Plug::ensureDependencies(); \
        return TAG::Plug::getValue(getNode()); \
    } \
    template <class TAG> \
    unsigned long long getVersion() const { \
        return TAG::Plug::getVersion(getNode()); \
    } \
    template <class TAG> \
    bool isDirty() const { \
        TAG::Plug::ensureDependencies(); \
       return TAG::Plug::getValuePtr()->isDirty(); \
    } \
    template <class TAG> \
    void debug() const { \
        return TAG::Plug::debug(); \
    } \
    template <class TAG> \
    const typename TAG::TYPE & set(const typename TAG::TYPE & theValue) { \
         TAG::Plug::ensureDependencies(); \
        return TAG::Plug::setValue(getNode(), theValue); \
    }

#define IMPLEMENT_PARENT_FACADE(CLASS) \
    IMPLEMENT_FACADE(CLASS) \
    template <class TAG> \
    const asl::Ptr< typename TAG::CHILDFACADE, dom::ThreadingModel> getChild() const{ \
        return TAG::Plug::getChildNode(getNode())->dom::Node::getFacade<typename TAG::CHILDFACADE>(); \
    } \
    template <class TAG> \
    asl::Ptr< typename TAG::CHILDFACADE, dom::ThreadingModel> getChild() { \
        return TAG::Plug::getChildNode(getNode())->dom::Node::getFacade<typename TAG::CHILDFACADE>(); \
    }

#define IMPLEMENT_DYNAMIC_FACADE(CLASS) \
    IMPLEMENT_FACADE(CLASS) \
    virtual dom::NodePtr getNamedItem(const dom::DOMString & name) { \
        dom::NodePtr myAttributeNode = dom::NamedNodeMap::getNamedItem(name); \
        updateAttribute(name, myAttributeNode); \
        return myAttributeNode; \
    } \
    virtual const dom::NodePtr getNamedItem(const dom::DOMString & name) const { \
        dom::NodePtr myAttributeNode = dom::NamedNodeMap::getNamedItem(name); \
        const_cast<CLASS*>(this)->CLASS::updateAttribute(name, myAttributeNode); \
        return myAttributeNode; \
    }

    struct FacadeKey {
        DOMString _myNodeName;
        DOMString _myParentName;
        FacadeKey(const DOMString & theType, const DOMString & theParentNodeName = "") 
            : _myNodeName(theType), _myParentName(theParentNodeName){}
        bool operator<(const FacadeKey & second) const {
            return (this->_myNodeName < second._myNodeName ||  
                    (this->_myNodeName == second._myNodeName && 
                     this->_myParentName < second._myParentName) );
        }
    };
    class ASL_DOM_EXPORT FacadeFactory {
        public:
            FacadeFactory() {};
            Facade * createFacade(const DOMString & theType, Node & theNode,
                                  const DOMString & theParentNodeName = "") const;
            void registerPrototype(const DOMString & theType, FacadePtr thePrototype,
                                   const DOMString & theParentNodeName = "");
            const FacadePtr findPrototype(const FacadeKey & thePrototypeKey) const;            
        private:
            typedef std::map<FacadeKey, FacadePtr> ProtoMap;
            ProtoMap _myPrototypes;
    };

    typedef asl::Ptr<FacadeFactory> FacadeFactoryPtr;

    /* @} */
}

#endif
