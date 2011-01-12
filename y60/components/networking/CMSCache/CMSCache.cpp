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

#include "CMSCache.h"
#include <asl/base/Logger.h>
#include <asl/base/file_functions.h>
#include <asl/base/os_functions.h>
#include <y60/inet/Request.h>

#ifdef verify
	#ifndef _SETTING_NO_UNDEF_WARNING_ 
		#warning Symbol 'verify' defined as macro, undefining. (Outrageous namespace pollution by Apples AssertMacros.h, revealing arrogance and incompetence)
	#endif
#undef verify
#endif

using namespace std;
using namespace asl;
using namespace inet;

#define VERBOSE_PRINT \
    _myVerboseFlag && AC_PRINT


IMPLEMENT_ENUM( y60::BackendType, y60::BackendTypeStrings);


namespace y60 {

CMSCache::CMSCache(const string & theLocalPath,
                       dom::NodePtr thePresentationDocument,
                       BackendType theBackendType,
                       const std::string & theUsername,
                       const std::string & thePassword,
                       const std::string & theSessionCookie) :
     _myLocalPath(theLocalPath),
     _myPresentationDocument(thePresentationDocument),
     _myUsername(theUsername),
     _myPassword(thePassword),
     _myStatusDocument( new dom::Document()),
     _myVerboseFlag( false ),
     _myBackendType( theBackendType ),
     _myMaxRequestCount( 1 ),
     _myCleanupFlag(false),
     _mySessionCookie(theSessionCookie),
     _myProxy("")
{
    _myReportNodeVersion = 0;
    _myReportNode = dom::NodePtr( new dom::Element("report") );
    _myReportNode->appendAttribute("version", as_string(_myReportNodeVersion));
    _myStatusDocument->appendChild( _myReportNode );

    _myAssetReportNode = dom::NodePtr( new dom::Element("assets") );
    _myReportNode->appendChild( _myAssetReportNode );

    _myStalledFilesNode = dom::NodePtr( new dom::Element("stalledfiles") );
    _myReportNode->appendChild( _myStalledFilesNode );

    //dumpPresentationToFile("dump.txt");
}

CMSCache::~CMSCache() {
    if (_myRequestThread) {
        _myRequestThread->join();
    }
}
void
CMSCache::collectExternalAssetList() {
    if ( ! _myPresentationDocument || _myPresentationDocument->childNodesLength() == 0 ) {
        throw CMSCacheException("No presentation file. Bailing out.", PLUS_FILE_LINE);
    }

    dom::NodePtr myRoot;
    if (_myPresentationDocument->childNode(0)->nodeType() == dom::Node::PROCESSING_INSTRUCTION_NODE) {
        myRoot = _myPresentationDocument->childNode(1)->childNode("themepool", 0);
    } else {
        myRoot = _myPresentationDocument->childNode(0)->childNode("themepool", 0);
    }

    if ( ! myRoot ) {
        throw CMSCacheException("Failed to find themepool", PLUS_FILE_LINE);
    }

    collectAssets(myRoot);

    AC_INFO << "Found " << _myAssets.size() << " assets.";
}

void
CMSCache::setProxy(const std::string & theProxy) {
    _myProxy = theProxy;
}

void
CMSCache::collectAssets(dom::NodePtr theParent) {
    for (unsigned i = 0; i < theParent->childNodesLength(); ++i) {
        dom::NodePtr myChild = theParent->childNode( i );
        if (myChild->nodeName() == "externalcontent") {
            const std::string & myPath = myChild->childNode("path")->childNode("#cdata-section")->nodeValue();
            if (_myAssets.find( myPath ) == _myAssets.end()) {
                if ( !myChild->childNode("uri") || myChild->childNode("path")->childNode("#cdata-section")->nodeValue().empty() ) {
                    // Assets without URIs are allowed (used for references)
                } else {
                    dom::NodePtr myClone = myChild->cloneNode( dom::Node::DEEP );
                    _myAssetReportNode->appendChild( myClone );
                    myClone->appendAttribute("status", "uptodate");
                    _myAssets.insert( std::make_pair(myPath, myClone ));
                }
            }
        }
        collectAssets( myChild );
    }
}

void
CMSCache::synchronize() {

    if ( ! fileExists( _myLocalPath )) {
        createPath( _myLocalPath );
    }

    collectExternalAssetList();
    if (_myCleanupFlag) {
        removeStalledAssets();
    }
    updateDirectoryHierarchy();
    AC_INFO << "asset count " << _myAssets.size();

    if ( ! _myAssets.empty()) {
        /*
        if ( ! _myUsername.empty() && ! _myPassword.empty()) {
            login();
        }
        */
        AC_INFO << "creating request thread";

    
        std::vector<std::pair<std::string, std::string> > myOutdatedAssets;
        collectOutdatedAssets(myOutdatedAssets);
        _myRequestThread = RequestThreadPtr(new RequestThread(_myLocalPath, _myUsername, _myPassword, 
                    _mySessionCookie, _myUserAgent, _myProxy, myOutdatedAssets, _myMaxRequestCount,
                    _myVerboseFlag));
        // fillRequestQueue();
        AC_INFO << "forking";
        _myRequestThread->fork();
    }
}
void
CMSCache::collectOutdatedAssets(std::vector<std::pair<std::string, std::string> > & theOutdatedAssets) {

    std::map<std::string, dom::NodePtr>::iterator myIter = _myAssets.begin();
    for (; myIter != _myAssets.end(); myIter++) {
        if ( isOutdated( myIter->second )) {
            VERBOSE_PRINT << "Asset " << myIter->second->childNode("path")->childNode("#cdata-section")->nodeValue()
                     << " may be outdated.";
            myIter->second->getAttribute("status")->nodeValue("outdated");
            theOutdatedAssets.push_back( make_pair(myIter->second->childNode("path")->childNode("#cdata-section")->nodeValue(), myIter->second->childNode("uri")->childNode("#cdata-section")->nodeValue()));
        } else {
            VERBOSE_PRINT << "Asset " << myIter->second->childNode("path")->childNode("#cdata-section")->nodeValue()
                     << " is uptodate.";
        }
    }

}

bool
CMSCache::isOutdated( dom::NodePtr theAsset ) {
    return !asl::fileExists(theAsset->childNode("path")->childNode("#cdata-section")->nodeValue());
}

void
CMSCache::updateDirectoryHierarchy() {
    std::map<std::string, dom::NodePtr>::iterator myIter = _myAssets.begin();
    while (myIter != _myAssets.end()) {
        std::string myPath = _myLocalPath + "/" +
                getDirectoryPart(myIter->second->childNode("path")->childNode("#cdata-section")->nodeValue());
        if ( ! fileExists( myPath )) {
            createPath( myPath );
        }
        myIter++;
    }
}

void
CMSCache::removeStalledAssets() {
    scanStalledEntries( _myLocalPath );
}

bool
isDirectoryEmpty(const std::string & theDirectory) {
    return getDirectoryEntries( theDirectory ).empty();
}

void
CMSCache::scanStalledEntries(const std::string & thePath) {
    std::vector<std::string> myEntries = getDirectoryEntries(thePath);
    std::vector<std::string>::iterator myIter = myEntries.begin();
    for (; myIter != myEntries.end(); ++myIter) {
        std::string myEntry = thePath + "/" + (*myIter);
        if (isDirectory(myEntry)) {
            scanStalledEntries(myEntry);
            if ( isDirectoryEmpty( myEntry )) {
                VERBOSE_PRINT << "Removing empty directory '" << myEntry << "'.";
                removeDirectory( myEntry );
            }
        } else {
            std::string myFilename = myEntry.substr(_myLocalPath.size() + 1,
                        myEntry.size() - _myLocalPath.size());
            if (_myAssets.find(myFilename) == _myAssets.end()) {
                VERBOSE_PRINT << "Removing '" << myFilename << "'.";
                dom::NodePtr myFileNode( new dom::Element("file"));
                myFileNode->appendAttribute("path", myFilename );
                myFileNode->appendAttribute("status", "removed");
                _myStalledFilesNode->appendChild( myFileNode );
                deleteFile( _myLocalPath + "/" + myFilename );
            }
        }
    }
}

dom::NodePtr
CMSCache::getStatusReport() {
    // update asset report
    StatusMsg myStatusMsg("","","");
    bool myModifiedFlag = false;
    while (_myRequestThread && _myRequestThread->popStatusMsg(myStatusMsg)) {
        std::map<std::string, dom::NodePtr>::iterator it = _myAssets.find(myStatusMsg.path);
        if (it == _myAssets.end()) {
            AC_WARNING << myStatusMsg.path << " not found in asset list";
        } else {
            myModifiedFlag = true;
            if (it->second->getAttribute(myStatusMsg.attribute)) {
                it->second->getAttribute(myStatusMsg.attribute)->nodeValue(myStatusMsg.value);
            } else {
                it->second->appendAttribute(myStatusMsg.attribute, myStatusMsg.value);
            }
        }
    }

    if (myModifiedFlag) {
        _myReportNode->getAttribute("version")->nodeValue(as_string(_myReportNodeVersion++));
    }

    return _myStatusDocument;
}

void
CMSCache::dumpPresentation() {
    AC_PRINT << _myPresentationDocument;
}

void
CMSCache::dumpPresentationToFile(const string & theFilename) {
    Path myPath(theFilename, UTF8);
    std::ofstream myFile(myPath.toLocale().c_str());
    myFile << *_myPresentationDocument;
    if (!myFile) {
        throw CMSCacheException("Dump to file failed for file '" + myPath.toLocale() +
                "'.", PLUS_FILE_LINE);
    }
}

}
