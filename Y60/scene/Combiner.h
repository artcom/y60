//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef Y60_COMBINER_INCLUDED
#define Y60_COMBINER_INCLUDED

#include <string>

#include <asl/Exception.h>
#include <asl/Ptr.h>

#include <y60/DataTypes.h>
#include <y60/NodeValueNames.h>


namespace y60 {

    DEFINE_EXCEPTION(CombinerException, asl::Exception);

    class Combiner
    {
        public:
            Combiner();
            virtual ~Combiner();

        protected:

        private:
    };

    typedef asl::Ptr<Combiner, dom::ThreadingModel> CombinerPtr;
    typedef asl::WeakPtr<Combiner, dom::ThreadingModel> CombinerWeakPtr;
}

#endif // Y60_COMBINER_INCLUDED
