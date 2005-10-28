/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: Events.h,v $
//
//   $Revision: 1.5 $
//
// Description: dom event interface 
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#ifndef _xml_Events_h_included_
#define _xml_Events_h_included_

#include <asl/settings.h>
#include "typedefs.h"
#include <map>

namespace dom {

    /*! \addtogroup aslxml */
    /* @{ */

    // Introduced in DOM Level 2:

    extern const char * PhaseTypeName[];

    class Event {
    public:
        // PhaseType
        enum PhaseType { CAPTURING_PHASE                = 1,
                        AT_TARGET                      = 2,
                        BUBBLING_PHASE                 = 3
        };
        virtual ~Event() {};

        virtual const DOMString & type() const = 0;
        virtual EventTargetPtr target() const = 0;
        virtual EventTargetPtr currentTarget() const = 0;
        virtual PhaseType eventPhase() const = 0;
        virtual bool bubbles() const = 0;
        virtual bool cancelable() const = 0;
        virtual const DOMTimeStamp &  timeStamp() const = 0;
        virtual void stopPropagation() = 0;
        virtual void preventDefault() = 0;
        virtual void initEvent(const DOMString & eventTypeArg, 
                                bool canBubbleArg, 
                                bool cancelableArg) = 0;
        /*
        // Introduced in DOM Level 3:
        readonly attribute DOMString       namespaceURI;
        // Introduced in DOM Level 3:
        boolean            isCustom();
        // Introduced in DOM Level 3:
        void               stopImmediatePropagation();
        */
        // Introduced in DOM Level 3:
        virtual bool isDefaultPrevented() const = 0;
        /*
        // Introduced in DOM Level 3:
        void               initEventNS(in DOMString namespaceURIArg, 
                                        in DOMString eventTypeArg, 
                                        in boolean canBubbleArg, 
                                        in boolean cancelableArg);
        */
        // added functions for automatic dispatch and propagation
        friend class EventTarget;
        friend class Node;
        friend class EventFactory;
    protected:
        virtual void target(EventTargetPtr) = 0;
        virtual void currentTarget(EventTargetPtr) = 0;
        virtual void eventPhase(PhaseType) = 0;
        virtual void timeStamp(DOMTimeStamp) = 0;
        virtual bool isPropagationStopped() const = 0;
        virtual EventPtr clone() = 0;
    };

    // Introduced in DOM Level 2:
    class EventTarget {
    public:
        virtual ~EventTarget() {}
        virtual void addEventListener(const DOMString & type, 
                                        EventListenerPtr listener, 
                                        bool useCapture) = 0;
        virtual void removeEventListener(const DOMString & type, 
                                        EventListenerPtr listener, 
                                        bool useCapture) = 0;
        // Modified in DOM Level 3:
        virtual bool dispatchEvent(EventPtr evt) = 0;
        /*
        // Introduced in DOM Level 3:
        void               addEventListenerNS(in DOMString namespaceURI, 
                                                in DOMString type, 
                                                in EventListener listener, 
                                                in boolean useCapture, 
                                                in DOMObject evtGroup);
        // Introduced in DOM Level 3:
        void               removeEventListenerNS(in DOMString namespaceURI, 
                                                in DOMString type, 
                                                in EventListener listener, 
                                                in boolean useCapture);
        // Introduced in DOM Level 3:
        boolean            willTriggerNS(in DOMString namespaceURI, 
                                        in DOMString type);
        // Introduced in DOM Level 3:
        boolean            hasEventListenerNS(in DOMString namespaceURI, 
                                            in DOMString type);
        */
    };

    // Introduced in DOM Level 2:
    class EventListener {
    public:
        virtual ~EventListener() {}
        virtual void handleEvent(EventPtr evt) = 0;
    };

    // Introduced in DOM Level 2:
    class DocumentEvent {
    public:
        virtual ~DocumentEvent() {}
        virtual EventPtr createEvent(const DOMString & eventType) = 0; //raises(DOMException);
        /*
        // Introduced in DOM Level 3:
        boolean            canDispatch(in DOMString namespaceURI, 
                                        in DOMString type);
        */
    };

    template <class PAYLOAD>
    class GenericEvent : public Event {
    public:
        GenericEvent(const DOMString theType,
                     bool canBubbleArg = true, 
                     bool cancelableArg = true,
                     DOMTimeStamp theTimeStamp = DOMTimeStamp(),
                     asl::Ptr<PAYLOAD> thePayload = asl::Ptr<PAYLOAD>(0))
                     : 
        _myType(theType),
        _myBubblesFlag(canBubbleArg),
        _myCancelableFlag(cancelableArg),
        _myTimeStamp(theTimeStamp),
        _myEventPhase(CAPTURING_PHASE),
        _myPropagationStopped(false),
        _myDefaultPrevented(false),
        _myPayload(thePayload)
        {}
        ~GenericEvent() {}
        virtual const DOMString & type() const {
            return _myType;
        }
        virtual EventTargetPtr target() const {
            return _myTarget;
        }
        virtual EventTargetPtr currentTarget() const {
            return _myCurrentTarget;
        }
        virtual PhaseType eventPhase() const {
            return _myEventPhase;
        }
        virtual bool bubbles() const {
            return _myBubblesFlag;
        }
        virtual bool cancelable() const {
            return _myCancelableFlag;
        }
        virtual const DOMTimeStamp &  timeStamp() const {
            return _myTimeStamp;
        }
        virtual void stopPropagation() {
            if (cancelable()) {
                _myPropagationStopped = true;
            }
        }
        virtual void preventDefault() {
            _myDefaultPrevented = true;
        }
        virtual void initEvent(const DOMString & eventTypeArg, 
                                bool canBubbleArg, 
                                bool cancelableArg)
        {
            _myBubblesFlag = canBubbleArg;
            _myCancelableFlag = cancelableArg;
        }
        /*
        // Introduced in DOM Level 3:
        readonly attribute DOMString       namespaceURI;
        // Introduced in DOM Level 3:
        boolean            isCustom();
        // Introduced in DOM Level 3:
        void               stopImmediatePropagation();
        */
        // Introduced in DOM Level 3:
        virtual bool isDefaultPrevented() const {
            return _myDefaultPrevented;
        }
        /*
        // Introduced in DOM Level 3:
        void               initEventNS(in DOMString namespaceURIArg, 
                                        in DOMString eventTypeArg, 
                                        in boolean canBubbleArg, 
                                        in boolean cancelableArg);
        */
        asl::Ptr<PAYLOAD> getPayload() {
            return _myPayload;
        }
        const asl::Ptr<PAYLOAD> & getPayload() const {
            return _myPayload;
        }
        void setPayload(asl::Ptr<PAYLOAD> thePayload) {
            _myPayload = thePayload;
        }
        // added functions for automatic dispatch and propagation
    protected:
        virtual void target(EventTargetPtr theTarget) {
            _myTarget = theTarget;
        }
        virtual void currentTarget(EventTargetPtr theCurrentTarget) {
            _myCurrentTarget = theCurrentTarget;
        }
        virtual void eventPhase(PhaseType theEventPhase) {
            _myEventPhase = theEventPhase;
        }
        virtual void timeStamp(DOMTimeStamp theTimeStamp) {
            _myTimeStamp  = theTimeStamp;
        }
        virtual bool isPropagationStopped() const {
            return _myPropagationStopped;
        }
        virtual EventPtr clone() {
            return EventPtr(new GenericEvent(_myType, _myBubblesFlag, _myCancelableFlag));
        }
    private:
        DOMString _myType;
        EventTargetPtr _myTarget;
        EventTargetPtr _myCurrentTarget;
        PhaseType _myEventPhase;
        bool _myBubblesFlag;
        bool _myCancelableFlag;
        DOMTimeStamp _myTimeStamp;
        bool _myPropagationStopped;
        bool _myDefaultPrevented;
        asl::Ptr<PAYLOAD> _myPayload;
    };

    class EventFactory {
    public:
		EventFactory();
		EventPtr createEvent(const DOMString & theType) const;
 		void registerPrototype(const DOMString & theType, EventPtr thePrototype);
		const EventPtr findPrototype(const DOMString & theType) const;
	private:
		typedef std::map<DOMString,EventPtr> ProtoMap;
		ProtoMap _myPrototypes;
	};

	extern void registerStandardTypes(EventFactory & theFactory);

    inline
    EventFactory &
    StandardEventFactory() {
        static asl::Ptr<EventFactory> ourFactory;
        if (!ourFactory) {
            ourFactory = asl::Ptr<EventFactory>(new EventFactory);
            registerStandardTypes(*ourFactory);
        }
        return *ourFactory;
    }

/*
// Introduced in DOM Level 3:
interface CustomEvent : Event {
  void               setDispatchState(in EventTarget target, 
                                      in unsigned short phase);
  boolean            isPropagationStopped();
  boolean            isImmediatePropagationStopped();
};
*/


/*
// Introduced in DOM Level 2:
interface UIEvent : Event {
  readonly attribute views::AbstractView view;
  readonly attribute long            detail;
  void               initUIEvent(in DOMString typeArg, 
                                 in boolean canBubbleArg, 
                                 in boolean cancelableArg, 
                                 in views::AbstractView viewArg, 
                                 in long detailArg);
  // Introduced in DOM Level 3:
  void               initUIEventNS(in DOMString namespaceURI, 
                                   in DOMString typeArg, 
                                   in boolean canBubbleArg, 
                                   in boolean cancelableArg, 
                                   in views::AbstractView viewArg, 
                                   in long detailArg);
};

// Introduced in DOM Level 3:
interface TextEvent : UIEvent {
  readonly attribute DOMString       data;
  void               initTextEvent(in DOMString typeArg, 
                                   in boolean canBubbleArg, 
                                   in boolean cancelableArg, 
                                   in views::AbstractView viewArg, 
                                   in DOMString dataArg);
  void               initTextEventNS(in DOMString namespaceURI, 
                                     in DOMString type, 
                                     in boolean canBubbleArg, 
                                     in boolean cancelableArg, 
                                     in views::AbstractView viewArg, 
                                     in DOMString dataArg);
};

// Introduced in DOM Level 2:
interface MouseEvent : UIEvent {
  readonly attribute long            screenX;
  readonly attribute long            screenY;
  readonly attribute long            clientX;
  readonly attribute long            clientY;
  readonly attribute boolean         ctrlKey;
  readonly attribute boolean         shiftKey;
  readonly attribute boolean         altKey;
  readonly attribute boolean         metaKey;
  readonly attribute unsigned short  button;
  readonly attribute EventTarget     relatedTarget;
  void               initMouseEvent(in DOMString typeArg, 
                                    in boolean canBubbleArg, 
                                    in boolean cancelableArg, 
                                    in views::AbstractView viewArg, 
                                    in long detailArg, 
                                    in long screenXArg, 
                                    in long screenYArg, 
                                    in long clientXArg, 
                                    in long clientYArg, 
                                    in boolean ctrlKeyArg, 
                                    in boolean altKeyArg, 
                                    in boolean shiftKeyArg, 
                                    in boolean metaKeyArg, 
                                    in unsigned short buttonArg, 
                                    in EventTarget relatedTargetArg);
  // Introduced in DOM Level 3:
  void               initMouseEventNS(in DOMString namespaceURI, 
                                      in DOMString typeArg, 
                                      in boolean canBubbleArg, 
                                      in boolean cancelableArg, 
                                      in views::AbstractView viewArg, 
                                      in long detailArg, 
                                      in long screenXArg, 
                                      in long screenYArg, 
                                      in long clientXArg, 
                                      in long clientYArg, 
                                      in boolean ctrlKeyArg, 
                                      in boolean altKeyArg, 
                                      in boolean shiftKeyArg, 
                                      in boolean metaKeyArg, 
                                      in unsigned short buttonArg, 
                                      in EventTarget relatedTargetArg, 
                                      in boolean altGraphKeyArg);
  // Introduced in DOM Level 3:
  readonly attribute boolean         altGraphKey;
};

// Introduced in DOM Level 3:
interface KeyboardEvent : UIEvent {

  // KeyLocationCode
  const unsigned long       DOM_KEY_LOCATION_STANDARD      = 0x00;
  const unsigned long       DOM_KEY_LOCATION_LEFT          = 0x01;
  const unsigned long       DOM_KEY_LOCATION_RIGHT         = 0x02;
  const unsigned long       DOM_KEY_LOCATION_NUMPAD        = 0x03;
  const unsigned long       DOM_KEY_LOCATION_UNKNOWN       = 0x04;

  readonly attribute DOMString       keyIdentifier;
  readonly attribute unsigned long   keyLocation;
  readonly attribute boolean         ctrlKey;
  readonly attribute boolean         shiftKey;
  readonly attribute boolean         altKey;
  readonly attribute boolean         metaKey;
  readonly attribute boolean         altGraphKey;
  void               initKeyboardEvent(in DOMString typeArg, 
                                       in boolean canBubbleArg, 
                                       in boolean cancelableArg, 
                                       in views::AbstractView viewArg, 
                                       in DOMString keyIdentifierArg, 
                                       in unsigned long keyLocationArg, 
                                       in boolean ctrlKeyArg, 
                                       in boolean shiftKeyArg, 
                                       in boolean altKeyArg, 
                                       in boolean metaKeyArg, 
                                       in boolean altGraphKeyArg);
  void               initKeyboardEventNS(in DOMString namespaceURI, 
                                         in DOMString type, 
                                         in boolean canBubbleArg, 
                                         in boolean cancelableArg, 
                                         in views::AbstractView viewArg, 
                                         in DOMString keyIdentifierArg, 
                                         in unsigned long keyLocationArg, 
                                         in boolean ctrlKeyArg, 
                                         in boolean shiftKeyArg, 
                                         in boolean altKeyArg, 
                                         in boolean metaKeyArg, 
                                         in boolean altGraphKeyArg);
};

// Introduced in DOM Level 2:
interface MutationEvent : Event {

  // attrChangeType
  const unsigned short      MODIFICATION                   = 1;
  const unsigned short      ADDITION                       = 2;
  const unsigned short      REMOVAL                        = 3;

  readonly attribute Node            relatedNode;
  readonly attribute DOMString       prevValue;
  readonly attribute DOMString       newValue;
  readonly attribute DOMString       attrName;
  readonly attribute unsigned short  attrChange;
  void               initMutationEvent(in DOMString typeArg, 
                                       in boolean canBubbleArg, 
                                       in boolean cancelableArg, 
                                       in Node relatedNodeArg, 
                                       in DOMString prevValueArg, 
                                       in DOMString newValueArg, 
                                       in DOMString attrNameArg, 
                                       in unsigned short attrChangeArg);
  // Introduced in DOM Level 3:
  void               initMutationEventNS(in DOMString namespaceURI, 
                                         in DOMString typeArg, 
                                         in boolean canBubbleArg, 
                                         in boolean cancelableArg, 
                                         in Node relatedNodeArg, 
                                         in DOMString prevValueArg, 
                                         in DOMString newValueArg, 
                                         in DOMString attrNameArg, 
                                         in unsigned short attrChangeArg);
};

// Introduced in DOM Level 3:
interface MutationNameEvent : MutationEvent {
  readonly attribute DOMString       prevNamespaceURI;
  readonly attribute DOMString       prevNodeName;
  // Introduced in DOM Level 3:
  void               initMutationNameEvent(in DOMString typeArg, 
                                           in boolean canBubbleArg, 
                                           in boolean cancelableArg, 
                                           in Node relatedNodeArg, 
                                           in DOMString prevNamespaceURI, 
                                           in DOMString prevNodeName);
  // Introduced in DOM Level 3:
  void               initMutationNameEventNS(in DOMString namespaceURI, 
                                             in DOMString typeArg, 
                                             in boolean canBubbleArg, 
                                             in boolean cancelableArg, 
                                             in Node relatedNodeArg, 
                                             in DOMString prevNamespaceURI, 
                                             in DOMString prevNodeName);
};
*/

/* @} */
} // namespace
#endif
