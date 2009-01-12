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
//    $RCSfile: typedefs.h,v $
//
//   $Revision: 1.6 $
//
// Description: tiny fast XML-Parser and DOM
// 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _asl_xml_typedefs_included_
#define _asl_xml_typedefs_included_

#include "asl_dom_settings.h"

#include "ThreadingModel.h" 
#include <asl/base/Ptr.h>
#include <asl/base/Time.h>
#include <string>

namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

    class Node;
    typedef asl::Ptr<Node,ThreadingModel> NodePtr;
    typedef asl::WeakPtr<Node,ThreadingModel> NodeWeakPtr;
    
    class Schema;
    typedef asl::Ptr<Schema,ThreadingModel> SchemaPtr;

    typedef std::string DOMString;
    typedef std::string String;

    class Event;
    typedef asl::Ptr<Event, ThreadingModel> EventPtr;

    class EventTarget;
    typedef asl::Ptr<EventTarget, ThreadingModel> EventTargetPtr;

    class EventListener;
    typedef asl::Ptr<EventListener, ThreadingModel> EventListenerPtr;

    class EventFactory;

    typedef asl::Time DOMTimeStamp;

    class NodeIDRegistry;
    typedef asl::Ptr<NodeIDRegistry,ThreadingModel> NodeIDRegistryPtr;
    typedef asl::WeakPtr<NodeIDRegistry,ThreadingModel> NodeIDRegistryWeakPtr;

    class NodeIDRefRegistry;
    typedef asl::Ptr<NodeIDRefRegistry,ThreadingModel> NodeIDRefRegistryPtr;
    typedef asl::WeakPtr<NodeIDRefRegistry,ThreadingModel> NodeIDRefRegistryWeakPtr;

    class Document;
    typedef asl::Ptr<Document,ThreadingModel> DocumentPtr;

    struct Dictionaries;
    typedef asl::Ptr<Dictionaries,ThreadingModel> DictionariesPtr;
    
    enum OpMode { IMMEDIATE, PATCH, LAZY};
    /* @} */
} //Namespace dom

#endif
