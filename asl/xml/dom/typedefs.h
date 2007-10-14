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

#include "ThreadingModel.h" 
#include <asl/Ptr.h>
#include <asl/Time.h>
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

    class Document;
    typedef asl::Ptr<Document,ThreadingModel> DocumentPtr;

    /* @} */
} //Namespace dom

#endif
