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
//
//   $RCSfile: $
//
//   $Author: $
//
//   $Revision: $
//
//=============================================================================

#ifndef _ac_builder_CanvasBuilder_h_
#define _ac_builder_CanvasBuilder_h_

#include "BuilderBase.h"
#include <y60/NodeValueNames.h>
#include <asl/Vector234.h>

namespace y60 {

/**
 * Builder for Canvasses
 * @ingroup Y60builder
 */ 
class CanvasBuilder : public BuilderBase {
    public:
        CanvasBuilder(const std::string & theName);
        virtual ~CanvasBuilder();
    private:

};
typedef asl::Ptr<CanvasBuilder>  CanvasBuilderPtr;

}


#endif // _ac_builder_CanvasBuilder_h_
