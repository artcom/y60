//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: VertexMemory.h,v $
//   $Author: martin $
//   $Revision: 1.1 $
//   $Date: 2005/04/21 16:25:02 $
//
//  Description: 
//
//=============================================================================

#ifndef _ac_scene_VertexMemoryBase_h
#define _ac_scene_VertexMemoryBase_h

#include "VertexDataImpl.h"
#include <asl/Exception.h>


namespace y60 {
     
    /**
     * @ingroup y60scene
     * Standard Main Memory VertexData managed by new() and delete() on heap. 
     * @warn This implementation cannot be rendered.
     */
    template <class T>
    class VertexMemoryBase : public VertexDataImpl<T, std::vector<T> > {
        typedef std::vector<T> VertexDataVector;
    public:
        void useAsPosition() const {
            throw Exception("Not implemented for this class", PLUS_FILE_LINE);
        }
        void useAsNormal() const {
            throw Exception("Not implemented for this class", PLUS_FILE_LINE);
        }
        void useAsColor() const {
            throw Exception("Not implemented for this class", PLUS_FILE_LINE);
        }
        void useAsTexCoord() const {
            throw Exception("Not implemented for this class", PLUS_FILE_LINE);
        }
        DEFINE_NESTED_EXCEPTION(VertexMemoryBase,Exception,asl::Exception);
    };
} // namespace y60


#endif
