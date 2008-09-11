//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_jslib_GLContextRegistry_h_
#define _ac_jslib_GLContextRegistry_h_

#include <asl/base/Singleton.h>
#include <asl/base/Ptr.h>
#include <set>
#include "AbstractRenderWindow.h"

namespace jslib {

/**
 * Registry of GL contexts. It is used to retrieve existing contexts
 * to share on the construction of new contexts.
 * @ingroup Y60jslib
 */ 
class GLContextRegistry : public asl::Singleton<GLContextRegistry> {
    public:
        struct ltweakptr
        {
            bool operator()(asl::WeakPtr<AbstractRenderWindow> p1, asl::WeakPtr<AbstractRenderWindow> p2) const
            {                
                return (p1.getRefCountPtr() < p2.getRefCountPtr());
            }
        };
        typedef std::set<asl::WeakPtr<AbstractRenderWindow>, ltweakptr> ContextSet;

        /**
         * Registers a context with the Registry it should be called after 
         * constrution of a AbstractRenderWindow is performed. 
         * @param theRenderWindow RenderWindow whichs context is to be registered.
         */
        void registerContext(asl::WeakPtr<AbstractRenderWindow> theRenderWindow);
        /**
         * Removes a context from the Registry.
         * @param theRenderWindow RenderWindow that now longer holds a GL context
         */
        void unregisterContext(asl::WeakPtr<AbstractRenderWindow> theRenderWindow);
        /**
         * @return the first available context in the Registry
         */
        asl::Ptr<AbstractRenderWindow> getContext();
        /**
         * @return a set of all contexts. 
         */
        const ContextSet & getContextSet() const { return _myContexts; }
        /**
         * @return Number of contexts in the Registry.
         */
        unsigned size() const { return _myContexts.size(); }
    protected:
        void cleanup();
    private:
        ContextSet _myContexts;
};

}

#endif // _ac_jslib_GLContextRegistry_h_
