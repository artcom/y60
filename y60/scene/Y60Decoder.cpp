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
//   $RCSfile: $
//
//   $Author: $
//
//   $Revision: $
//
//=============================================================================

// own header
#include "Y60Decoder.h"

#include <asl/base/Logger.h>
#include <asl/base/string_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/MappedBlock.h>

using namespace asl;

namespace y60 {
    std::string
    Y60Decoder::canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream) {
        if (theStream) {
            std::string myXML;
            // we read the first 100 bytes of the stream. If they contain <scene>, we
            // assume this to be an x60.
            theStream->getStream().readString(myXML, 100, 0);
            if (myXML.find("<scene") != std::string::npos) { //TODO: smells cheesy
                return MIME_TYPE_X60;
            }
            asl::Unsigned32 myMagic;
            theStream->getStream().readUnsigned32(myMagic, 0);
            if (myMagic == dom::Node::B60_MAGIC) {
                return MIME_TYPE_B60;
            }
            if (myMagic == dom::Node::D60_MAGIC) {
                return MIME_TYPE_D60;
            }
        } else {
            if (asl::toLowerCase(asl::getExtension(theUrl)) == "x60") {
                return MIME_TYPE_X60;
            }
            if (asl::toLowerCase(asl::getExtension(theUrl)) == "b60") {
                return MIME_TYPE_B60;
            }
            if (asl::toLowerCase(asl::getExtension(theUrl)) == "d60") {
                return MIME_TYPE_D60;
            }
        }
        return "";
    }

    bool
    Y60Decoder::setProgressNotifier(IProgressNotifierPtr theNotifier) {
        return false;
    }
    bool
    Y60Decoder::setLazy(bool theFlag) {
        _myLazyMode = theFlag;
        return theFlag;
    }

    bool
    Y60Decoder::addSource(asl::Ptr<asl::ReadableStreamHandle> theSource) {
        _mySources.push_back(theSource);
        return theSource != 0;
    }

    bool
    Y60Decoder::decodeScene(asl::Ptr<asl::ReadableStreamHandle> theSource, dom::DocumentPtr theDocument) {
        AC_DEBUG << "  Y60Decoder::decodeScene: theSource = '" << theSource->getName() << "'";
        asl::Time loadStart;
        asl::Time parseStart;
        bool myBinaryFlag = true;
        asl::ReadableStream & theStream = theSource->getStream();
        std::string myMimeType = canDecode("", theSource);
        if (myMimeType == MIME_TYPE_X60) {
            myBinaryFlag = false;
        }
        AC_DEBUG << "  Y60Decoder::decodeScene: MIME type = '"<<myMimeType<<"'";

        unsigned long myFileSize = 0;
        unsigned long myNodeCount = 0;

        if (myBinaryFlag) {
            AC_DEBUG << "  Y60Decoder::decodeScene: using binary xml loader, lazy="<<getLazy();
            myFileSize = theStream.size();
            //double myLoadTime = asl::Time() - loadStart;
            if (dynamic_cast<asl::MappedBlock*>(&theStream)) {
                AC_INFO << "  Mapped Block size " << myFileSize << " bytes ";
            } else  {
                AC_INFO << "  Readable Stream size: " << myFileSize << " bytes ";
            }
            parseStart.setNow();
            if (getLazy() && (myMimeType == MIME_TYPE_D60)) {
                AC_INFO << "  Y60Decoder::decodeScene: using lazy mode";
                theDocument->debinarizeLazy(theSource);
                myNodeCount = 1; // TODO: get node count from catalog
            } else {
                theDocument->debinarize(theStream);
                myNodeCount = countNodes(*theDocument);
            }
        } else {
            AC_DEBUG << "  Y60Decoder::decodeScene: using unicode xml loader";
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
            myNodeCount = countNodes(*theDocument);
            removeComments(dom::NodePtr(theDocument)); // PM: Why is this necessary?
        }

        asl::Time processingEnd;
        double myParseTime = processingEnd - parseStart;
        double myProcessingTime = processingEnd - loadStart;

        AC_INFO<< "  Parse time: " << myParseTime << " sec, " << long(myFileSize/myParseTime/1024) << " kb/sec";

        AC_INFO << "  Number of XML-Nodes: " << myNodeCount << ", "<< long(myNodeCount/myParseTime)
            << " nodes/sec, " << long(myFileSize/myNodeCount)<<" bytes/node";
        AC_INFO << "  Total XML processing time: " << myProcessingTime << " sec, "
            << long(myFileSize/myProcessingTime/1024)<<" kb/sec";
        return true;
    }

    void
    Y60Decoder::removeComments(dom::NodePtr theNode) {
        if (theNode->nodeType() == dom::Node::COMMENT_NODE) {
            AC_INFO << "found comment node and removed: " << (*theNode);
            theNode->parentNode()->removeChild(theNode);
            return;
        }

        if (theNode->hasChildNodes()) {
            for (int i=(theNode->childNodesLength()-1); i>=0; --i) {
                removeComments(theNode->childNode(i));
            }
        }
    }
}
