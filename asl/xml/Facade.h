/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2003, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
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

#include "typedefs.h"
#include "NodeList.h"
#include "Value.h"

#include <asl/Exception.h>
#include <asl/Ptr.h>

#include <map>

namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

    class Node;

    class Facade;
    typedef asl::Ptr<Facade,ThreadingModel> FacadePtr;
    typedef asl::WeakPtr<Facade,ThreadingModel> FacadeWeakPtr;

    class Facade : public TypedNamedNodeMap {
        public:
            DEFINE_NESTED_EXCEPTION(Facade, Exception, asl::Exception);
            DEFINE_NESTED_EXCEPTION(Facade, InvalidNullPointerPassed, Exception);

            virtual ~Facade() {};

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
        protected:
            Facade(Node & theNode);
        private:
            Node & _myNode;
            FacadeWeakPtr _mySelf;
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

    class FacadeFactory {
        public:
            FacadeFactory() {};
            Facade * createFacade(const DOMString & theType, Node & theNode) const;
        	void registerPrototype(const DOMString & theType, FacadePtr thePrototype);
        	const FacadePtr findPrototype(const DOMString & theType) const;
            bool isChildDependentFacade(const DOMString & theType) const;
        private:
            typedef std::map<DOMString, FacadePtr> ProtoMap;
        	ProtoMap _myPrototypes;
	};

	typedef asl::Ptr<FacadeFactory> FacadeFactoryPtr;

    /* @} */
}

#endif
