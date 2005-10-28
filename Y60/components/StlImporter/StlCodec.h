//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $RCSfile: $
//
//   $Author: $
//
//   $Revision: $
//
//=============================================================================

#ifndef _ac_StlImporter_StlCodec_h_
#define _ac_StlImporter_StlCodec_h_

#include <asl/Stream.h>
#include <y60/Shape.h>
#include <dom/Nodes.h>

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

    bool _myBigEndianFlag;
    asl::Ptr<asl::WriteableArrangedFile<asl::X86ByteOrder> > _myBigStream;
    asl::Ptr<asl::WriteableArrangedFile<asl::PowerPCByteOrder> > _myLittleStream;
};

} //namespace y60

#endif // _ac_StlImporter_StlCodec_h_
