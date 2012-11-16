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

#include "y60_scene_settings.h"

#include "VertexDataImpl.h"
#include <asl/base/Exception.h>


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
