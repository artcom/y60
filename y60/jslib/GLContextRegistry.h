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

#ifndef _ac_jslib_GLContextRegistry_h_
#define _ac_jslib_GLContextRegistry_h_

#include "y60_jslib_settings.h"

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
                return (p1.lock() < p2.lock());
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
