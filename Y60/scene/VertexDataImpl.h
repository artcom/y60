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

#include "VertexData.h"

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
            dom::NodePtr theVertexDataNode,
            unsigned theBeginIndex, unsigned theEndIndex)  
        {
            const std::vector<T> & mySource = 
                theVertexDataNode->dom::Node::nodeValueRef<std::vector<T> >();

            _myData.reserve(theEndIndex - theBeginIndex);

            for (unsigned i = theBeginIndex; i < theEndIndex; ++i) {
                _myData.push_back(mySource[theIndices[i]]);
            }        
        }
        // write back vertex data to dom source arrays
        void unload(const VectorOfUnsignedInt & theIndices,
            dom::NodePtr theVertexDataNode,
            unsigned theBeginIndex, unsigned theEndIndex) const
        {
            dom::Node::WritableValue<std::vector<T> > mySourceLock(theVertexDataNode);
            std::vector<T> & mySource = mySourceLock.get();

            if (_myData.size() != theEndIndex - theBeginIndex) {
                throw UnloadVertexArraySizeMismatch(JUST_FILE_LINE);
            }

            for (unsigned i = theBeginIndex; i < theEndIndex; ++i) {
                mySource[theIndices[i]] = _myData[i];
            }        
        }
        void lock() {}
        void unlock() {}

        virtual void useAsPosition() const = 0;
        virtual void useAsNormal() const = 0;
        virtual void useAsColor() const = 0;
        virtual void useAsTexCoord() const = 0;

    protected:
        const void * getDataPtr() const {
            return &(*_myData.begin());
        }
        void * getDataPtr() {
            return &(*_myData.begin());
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


