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

#ifndef _ac_StlImporter_StlCodec_h_
#define _ac_StlImporter_StlCodec_h_

#include "y60_stlimporter_settings.h"

#include <asl/base/Stream.h>
#include <y60/scene/Shape.h>
#include <asl/dom/Nodes.h>

namespace y60 {
struct StlHeader {
    char text[80];
};

/**
 *
 * @ingroup Y60componentsStlImporter
 */
class StlCodec {
public:
    const static int HEADER_SIZE = 80;
    const static int FACET_SIZE = 50; // NOTE: the record size is 50 bytes,
    //       but sizeof(StlFacet) = 52 (arch dependant)
    const static int NUM_FACET_SIZE = 4;
    const static unsigned short StlColorValid = 0x8000;
    const static unsigned short StlColorMask = 0x01f;

    struct StlFacet {
        asl::Vector3f normal;
        asl::Vector3f vertex[3];
        unsigned short color;
    };

    enum OutputMode {
        LITTLEENDIAN,
        BIGENDIAN,
        BLOCK
    };
    StlCodec(asl::Ptr<asl::ResizeableBlock> theBlock);
    StlCodec(std::string theFilename, bool theBigEndianFlag);
    virtual ~StlCodec();
    void exportShape(const dom::NodePtr theNode);
    void exportShapes(const dom::NodePtr theNode, const std::vector<std::string> & theIds);
    void close();
private:
    template <class AC_BYTE_ORDER_LOCAL>
    void exportShapeToStream(const y60::ShapePtr theShape, asl::WriteableArrangedStream<AC_BYTE_ORDER_LOCAL> & theStream, bool theWriteHeadersFlag = true);
    template <class AC_BYTE_ORDER_LOCAL>
    void exportHeader(const std::string & theName, unsigned theNumShapes, asl::WriteableArrangedStream<AC_BYTE_ORDER_LOCAL> & theStream);
    template <class AC_BYTE_ORDER_LOCAL>
    void exportFacet(const StlFacet & theFacet, asl::WriteableArrangedStream<AC_BYTE_ORDER_LOCAL> & theStream);

    OutputMode _myOutputMode;
    // depending on the outputmode, exactly one of the following are not null
    asl::Ptr<asl::WriteableArrangedFile<asl::X86ByteOrder> > _myBigStream;
    asl::Ptr<asl::WriteableArrangedFile<asl::PowerPCByteOrder> > _myLittleStream;
    asl::Ptr<asl::ResizeableBlock> _myBlock;
};

} //namespace y60

#endif // _ac_StlImporter_StlCodec_h_
