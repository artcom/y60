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
//   $Id: iostream_functions.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: iostream_functions.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//
//=============================================================================

#ifndef _ac_y60_stream_functions_h_
#define _ac_y60_stream_functions_h_

#include "RankedFeature.h" // Needed to make sure [de]binarize(RankedFeature) gets called :-(.
#include "typedefs.h"

#include <asl/base/Block.h>
#include <asl/dom/Value.h>

namespace asl {

    template <class T>
    std::istream & parseVector(std::istream & is, std::vector<T> & v) {
        char myChar;
        is >> myChar;

        if (myChar != '[') {
            is.setstate(std::ios::failbit);
            return is;
        }

        is >> myChar;
        if (myChar != ']') {
            is.unget();
        }
        while (myChar != ']') {
            T myElement;
            is >> myElement;

            if (!is) {
                is.setstate(std::ios::failbit);
                return is;
            }
            is >> myChar;
            if (myChar != ',' && myChar != ']') {
                is.setstate(std::ios::failbit);
                return is;
            }

            v.push_back(myElement);
        }
        return is;
    }

    template <class T>
    std::ostream & operator<<(std::ostream & os, const std::vector<T> & t) {
        return asl::printVector(os,t,t.size()>4);
    }

    template <class T>
    std::istream & operator>>(std::istream & is, std::vector<T> & t) {
        return asl::parseVector<T>(is, t);
    }

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfBool & theVector);

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfUnsignedInt & theVector);

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfSignedInt & theVector);

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfFloat & theVector);

    std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfString & theStringVector);

    std::istream &
    operator >> (std::istream & theStream, y60::VectorOfString & theStringVector);

    std::ostream &
    operator << (std::ostream & theStream, const dom::ResizeableVector & theVector);

//
///  generic ostream output operator template for raster templates
//

    template <class T, class alloc, class D>
    std::ostream & operator<<(std::ostream& o, const asl::raster<T, alloc, D>& m)
    {
        asl::Vector2i mySize(m.hsize(), m.vsize());
        if (o << mySize) {
#ifdef UNCOMPRESSED_RASTER_STREAM
            o << asl::ReadableBlockAdapter((const unsigned char*)&(*m.begin()), (const unsigned char*)&(*m.end()));
#else
            asl::Block myRasterBlock;
            dom::binarize(m, myRasterBlock);
            o << myRasterBlock;
#endif
        }
        return o;
    };

    template <class T, class alloc, class D>
    std::istream & operator>>(std::istream & is, asl::raster<T, alloc, D>& m) {
        asl::Vector2i mySize;
        if (is >> mySize) {
#ifdef UNCOMPRESSED_RASTER_STREAM
            asl::Block myBlock;
            if (is >> myBlock) {
                asl::WriteableBlockAdapter myCurrentImage((unsigned char*)&(*m.begin()), (unsigned char*)&(*m.end()));
                if (myCurrentImage.size() != myBlock.size()) {
                    m.resize(mySize[0], mySize[1]);
                    asl::WriteableBlockAdapter myImage((unsigned char*)&(*m.begin()), (unsigned char*)&(*m.end()));
                    myImage.assign(myBlock);
                    return is;
                }
            } 
#else
            asl::Block myBlock;
            if (is >> myBlock) {
                if (dom::debinarize(m, myBlock, 0) == myBlock.size()) {
                    return is;
                }
            }
#endif
        }
        is.setstate(std::ios::failbit);
        return is;
    }

}

#endif

