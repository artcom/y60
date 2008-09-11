//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_BuilderBase_h_
#define _ac_BuilderBase_h_

#include <asl/dom/typedefs.h>
#include <asl/base/Exception.h>

namespace y60 {

    DEFINE_EXCEPTION(SomException, asl::Exception);

    class BuilderBase {
        public:
            BuilderBase();
            BuilderBase(const std::string & theNodeName);

            virtual ~BuilderBase();

            dom::NodePtr getNode() const;
            void setNode(const dom::NodePtr & theNode);
            const std::string & getId() const;

// XXX Dead code
#if 0 
            const std::string &
            appendNodeWithId(const BuilderBase & theChildBuilder,
                    dom::NodePtr theListNode,
                    const std::string & theIdPrefix, unsigned & theIdCount);
#endif

            /** Appends the content of theChildBuilder to theListNode. If the new child allready
             *  has an id nothing is changed.
             */
            const std::string &
            appendNodeWithId(const BuilderBase & theChildBuilder, dom::NodePtr theListNode);

            void removeNodeById(dom::NodePtr theListNode, const std::string & theId);
        protected:
            BuilderBase(const dom::NodePtr & theNode);
            void reset(const std::string & theNodeName);
        private:
            dom::NodePtr _myNode;

    };

}

#endif
