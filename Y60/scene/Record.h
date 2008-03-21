//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//=============================================================================

#ifndef _ac_scene_record_h_
#define _ac_scene_record_h_

#include <y60/DataTypes.h>
#include <y60/CommonTags.h>
#include <y60/NodeValueNames.h>
#include <y60/PropertyNames.h>
#include <y60/TypeTraits.h>
#include <dom/Facade.h>
#include <dom/PropertyPlug.h>
#include <dom/AttributePlug.h>
#include <asl/Logger.h>

#include "PropertyListFacade.h"

namespace y60 {

    //                  TagName             Type           PropertyName                    Default

	class RecordFacade :
		public PropertyListFacade,
		public IdTag::Plug,
		public NameTag::Plug
	{
		public:
			RecordFacade(dom::Node & theNode) :
			    PropertyListFacade(theNode),
				IdTag::Plug(theNode),
				NameTag::Plug(theNode)
			{}
			IMPLEMENT_FACADE(RecordFacade);
	};
	typedef asl::Ptr<RecordFacade, dom::ThreadingModel> RecordFacadePtr;

}

#endif

