//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: IScene.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: IScene.h,v $
//   $Author: valentin $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//=============================================================================

#ifndef _ISCENE_INCLUDED
#define _ISCENE_INCLUDED

#include <asl/Ptr.h>
#include <dom/ThreadingModel.h>

namespace y60 {
    class IResourceManager;
    class IScene {
        public:
            virtual ~IScene();
            virtual const IResourceManager * getResourceManager() const = 0;
            virtual IResourceManager * getResourceManager() = 0;        
    };

    typedef asl::Ptr<IScene, dom::ThreadingModel> IScenePtr;

}
#endif
