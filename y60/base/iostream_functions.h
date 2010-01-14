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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ac_y60_stream_functions_h_
#define _ac_y60_stream_functions_h_

#include "y60_base_settings.h"

#include "RankedFeature.h" // Needed to make sure [de]binarize(RankedFeature) gets called :-(.
#include "typedefs.h"

#include <asl/base/begin_end.h>
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

    Y60_BASE_DECL std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfBool & theVector);

    Y60_BASE_DECL std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfUnsignedInt & theVector);

    Y60_BASE_DECL std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfSignedInt & theVector);

    Y60_BASE_DECL std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfFloat & theVector);

    Y60_BASE_DECL std::ostream &
    operator << (std::ostream & theStream, const y60::VectorOfString & theStringVector);

    Y60_BASE_DECL std::istream &
    operator >> (std::istream & theStream, y60::VectorOfString & theStringVector);

    Y60_BASE_DECL std::ostream &
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
            o << asl::ReadableBlockAdapter((const unsigned char*)begin_ptr(m), (const unsigned char*)end_ptr(m));
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
                asl::WriteableBlockAdapter myCurrentImage((const unsigned char*)begin_ptr(m), (const unsigned char*)end_ptr(m));
                if (myCurrentImage.size() != myBlock.size()) {
                    m.resize(mySize[0], mySize[1]);
                    asl::WriteableBlockAdapter myImage((const unsigned char*)begin_ptr(m), (const unsigned char*)end_ptr(m));
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

