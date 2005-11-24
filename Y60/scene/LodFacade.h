/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2003, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//   $RCSfile: LodFacade.h,v $
//
//   $Revision: 1.7 $
//
//   Description: 
// 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _scene_LodFacade_h_
#define _scene_LodFacade_h_

#include "TransformHierarchyFacade.h"

namespace y60 {

    //                  theTagName  theType             theAttributeName        theDefault
    DEFINE_ATTRIBUT_TAG(RangesTag,  VectorOfFloat, LODRANGES_ATTRIB,  TYPE());
/*
    struct RangesTag {
		typedef VectorOfFloat TYPE;
        typedef dom::AttributePlug<RangesTag> Plug;
        static const char * getName() { return LODRANGES_ATTRIB; }
		static const TYPE getDefault() { return VectorOfFloat(); }
    };
*/
    class LodFacade : 
        public TransformHierarchyFacade,
        public RangesTag::Plug
    {
        public:
            LodFacade(dom::Node & theNode)  :
                TransformHierarchyFacade(theNode),
                RangesTag::Plug(theNode)
            {}
            IMPLEMENT_FACADE(LodFacade);
    };
    
    typedef asl::Ptr<LodFacade, dom::ThreadingModel> LodFacadePtr;
}

#endif // _scene_LodFacade_h_
