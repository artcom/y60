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
//   $Id: ISceneDecoder.h,v 1.1 2005/03/10 11:27:07 danielk Exp $
//   $RCSfile: ISceneDecoder.h,v $
//   $Author: danielk $
//   $Revision: 1.1 $
//   $Date: 2005/03/10 11:27:07 $
//
//=============================================================================

#ifndef _ISCENEDECODER_INCLUDED
#define _ISCENEDECODER_INCLUDED

#include "IProgressNotifier.h"

#include <y60/IDecoder.h>
#include <asl/Ptr.h>
#include <dom/Nodes.h>

namespace y60 {

class ISceneDecoder : public IDecoder {
    public:
        virtual bool decodeScene(asl::Ptr<asl::ReadableStreamHandle>, dom::DocumentPtr theScene) = 0;
        virtual bool setProgressNotifier(IProgressNotifierPtr theNotifier) = 0;
        virtual bool setLazy(bool) = 0;
        virtual bool getLazy() const {return false;}
        virtual bool addSource(asl::Ptr<asl::ReadableStreamHandle>) = 0;
};

}
#endif

