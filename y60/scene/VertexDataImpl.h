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
//   $RCSfile: VertexDataImpl.h,v $
//   $Author: martin $
//   $Revision: 1.1 $
//   $Date: 2005/04/21 16:25:02 $
//
//  Description:
//
//=============================================================================

#ifndef _ac_scene_VertexDataImpl_h_
#define _ac_scene_VertexDataImpl_h_

#include "y60_scene_settings.h"

#include "VertexData.h"

#include <asl/base/begin_end.h>

namespace y60 {

    template <class T, class VertexDataVector>
    class VertexDataImpl : public VertexData<T> {
        VertexDataVector _myData;
    public:
        size_t size() const {
            return _myData.size();
        }
        T & operator[](unsigned n) {
            return *(_myData.begin() + n);
        }
        const T & operator[](unsigned n) const {
            return *(_myData.begin() + n);
        }
        void push_back(const T & theElement) {
            _myData.push_back(theElement);
        }
        void copy(const VertexData<T> & theSourceData) {
            _myData.reserve(theSourceData.size());
            for (int i = 0; i < theSourceData.size(); ++i) {
                _myData.push_back(theSourceData[i]);
            }
        }
        void load(const VectorOfUnsignedInt & theIndices,
            dom::NodePtr theVertexDataNode)
        {
            const std::vector<T> & mySource =
                theVertexDataNode->dom::Node::nodeValueRef<std::vector<T> >();

            unsigned mySize = _myData.size();

            _myData.resize(mySize + theIndices.size());

            lock(true, false);
            for (unsigned i = 0; i < theIndices.size(); ++i) {
                _myData[mySize+i] = mySource[theIndices[i]];
            }
            unlock();
        }

        // write back vertex data to dom source arrays
        void unload(const VectorOfUnsignedInt & theIndices,
            dom::NodePtr theVertexDataNode) const
        {
            dom::Node::WritableValue<std::vector<T> > mySourceLock(theVertexDataNode);
            std::vector<T> & mySource = mySourceLock.get();

            if (_myData.size() != theIndices.size()) {
                throw UnloadVertexArraySizeMismatch(JUST_FILE_LINE);
            }

            for (unsigned i = 0; i < theIndices.size(); ++i) {
                mySource[theIndices[i]] = _myData[i];
            }
        }
        void lock(bool forWriting = true, bool forReading = false) const {}
        void unlock() const {}

        virtual void useAsPosition() const = 0;
        virtual void useAsNormal() const = 0;
        virtual void useAsColor() const = 0;
        virtual void useAsTexCoord() const = 0;

    protected:
        const void * getDataPtr() const {
            return asl::begin_ptr(_myData);
        }
        void * getDataPtr() {
            return asl::begin_ptr(_myData);
        }
        VertexDataVector & getDataVector() {
            return _myData;
        }
        const VertexDataVector & getDataVector() const {
            return _myData;
        }
    };
}

#endif


