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

#include "Y60Decoder.h"
#include <asl/Logger.h>
#include <asl/string_functions.h>
#include <asl/file_functions.h>

namespace y60 {
    std::string
    Y60Decoder::canDecode(const std::string & theUrl, asl::ReadableStream * theStream) {
        if (theStream) {
            std::string myXML;
            // we read the first 100 bytes of the stream. If they contain <scene>, we
            // assume this to be an x60.
            theStream->readString(myXML, 100, 0);
            if (myXML.find("<scene") != std::string::npos) {
                return MIME_TYPE_X60;
            } else if (myXML.find(std::string(1, char(5)) + "scene") != std::string::npos) {
                return MIME_TYPE_B60;
            }
        } else {
            if (asl::toLowerCase(asl::getExtension(theUrl)) == "x60") {
                return MIME_TYPE_X60;
            }
            if (asl::toLowerCase(asl::getExtension(theUrl)) == "b60") {
                return MIME_TYPE_B60;
            }
        }
        return "";
    }

    bool 
    Y60Decoder::setProgressNotifier(IProgressNotifierPtr theNotifier) {
        return false;
    }

    bool 
    Y60Decoder::decodeScene(asl::ReadableStream & theStream, dom::DocumentPtr theDocument) {
        asl::Time loadStart;
        asl::Time parseStart;
        bool myBinaryFlag = true;
        if (canDecode("", &theStream) == MIME_TYPE_X60) {
            myBinaryFlag = false;
        }

        unsigned long myFileSize = 0;
        if (myBinaryFlag) {
            myFileSize = theStream.size();
            double myLoadTime = asl::Time() - loadStart;
            AC_INFO << "  Mapped File size: " << myFileSize << " bytes ";
            parseStart.setNow();
            theDocument->debinarize(theStream);

        } else {
            std::string myXMLFile;
            theStream.readString(myXMLFile, theStream.size(), 0);
            myFileSize = myXMLFile.size();
            double myLoadTime = asl::Time() - loadStart;
            AC_INFO << "  File size: " << myFileSize << " bytes ";
            AC_INFO << "  Load time: " << myLoadTime << " sec, "
                << long(myFileSize/myLoadTime/1024) << " kb/sec";
            parseStart.setNow();
            theDocument->parse(myXMLFile);
            if (!theDocument) {
                throw Y60DecodeException("### ERROR while parsing scene file", PLUS_FILE_LINE);
            }
        }

        asl::Time processingEnd;
        double myParseTime = processingEnd - parseStart;
        double myProcessingTime = processingEnd - loadStart;

        AC_DEBUG << "  Parse time: " << myParseTime << " sec, " << long(myFileSize/myParseTime/1024) << " kb/sec";
        unsigned long myNodeCount = countNodes(*theDocument);
        AC_DEBUG << "  Number of XML-Nodes: " << myNodeCount << ", "<< long(myNodeCount/myParseTime)
            << " nodes/sec, " << long(myFileSize/myNodeCount)<<" bytes/node";
        AC_DEBUG << "  Total XML processing time: " << myProcessingTime << " sec, "
            << long(myFileSize/myProcessingTime/1024)<<" kb/sec";
        return true;
    }
}
