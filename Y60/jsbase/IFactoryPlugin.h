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
//   $RCSfile: IFactoryPlugin.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2004/10/27 03:25:50 $
//
//
//=============================================================================

#ifndef _ac_jslib_IFactoryPlugin_h_
#define _ac_jslib_IFactoryPlugin_h_

#include "IScriptablePlugin.h"
#include <dom/Nodes.h>

namespace jslib {
	struct IFactoryPlugin {
		virtual ~IFactoryPlugin() {};
		virtual void initClasses(JSContext * theContext, JSObject *theGlobalObject) = 0;
		virtual jslib::IScriptablePluginPtr createInstance() = 0;
	};
	typedef asl::Ptr<IFactoryPlugin> IFactoryPluginPtr;
}

#endif

