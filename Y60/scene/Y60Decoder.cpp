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
#include <asl/MappedBlock.h>

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
            } else if (myXML.find(std::string(1, char(5)) + "scene") != std::string::npos) { //TODO: smells cheesy
                return MIME_TYPE_B60;
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
    }

    bool 
    Y60Decoder::addSource(asl::Ptr<asl::ReadableStreamHandle> theSource) {
        _mySources.push_back(theSource);
    }
 
    bool 
    Y60Decoder::decodeScene(asl::Ptr<asl::ReadableStreamHandle> theSource, dom::DocumentPtr theDocument) {
        AC_DEBUG << "  Y60Decoder::decodeScene: theSource = '" << theSource->getName() << "'";
        asl::Time loadStart;
        asl::Time parseStart;
        bool myBinaryFlag = true;
        asl::ReadableStream & theStream = theSource->getStream();
        if (canDecode("", theSource) == MIME_TYPE_X60) {
            myBinaryFlag = false;
        }

        unsigned long myFileSize = 0;
        unsigned long myNodeCount = 0;
        
        if (myBinaryFlag) {
            myFileSize = theStream.size();
            double myLoadTime = asl::Time() - loadStart;
            if (dynamic_cast<asl::MappedBlock*>(&theStream)) {
                AC_INFO << "  Mapped Block size " << myFileSize << " bytes ";
            } else  {
                AC_INFO << "  Readable Stream size: " << myFileSize << " bytes ";
            }
            parseStart.setNow();
            if (getLazy()) {
                theDocument->debinarizeLazy(theSource);
                myNodeCount = 1; // TODO: get node count from catalog
            } else {
                theDocument->debinarize(theStream);
                myNodeCount = countNodes(*theDocument);
            }
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
