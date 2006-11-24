//==============================================================================
// Copyright (C) 2006, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================

#include "CMSCache.h"
#include <asl/Logger.h>
#include <asl/file_functions.h>
#include <asl/os_functions.h>
#include <y60/Request.h>

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
     _mySessionCookie(theSessionCookie)
{
    dom::NodePtr myReport = dom::NodePtr( new dom::Element("report") );
    _myStatusDocument->appendChild( myReport );
    _myAssetReportNode = dom::NodePtr( new dom::Element("assets") );
    myReport->appendChild( _myAssetReportNode );
    _myStalledFilesNode = dom::NodePtr( new dom::Element("stalledfiles") );
    myReport->appendChild( _myStalledFilesNode );
    //dumpPresentationToFile("dump.txt");
}

CMSCache::~CMSCache() {
    if (_myRequestThread) {
        _myRequestThread->join();
    }
}
/*
void
CMSCache::login() {
    switch ( _myBackendType ) {
        case OCS:
            //loginOCS();
            break;
        case SVN:
            // nothing to do here for svn ...
            // we just send our credentials with each request
            break;
        default:
            AC_ERROR << "Unknown backend type '" << _myBackendType << "'";
    }
}
*/
/*
void
CMSCache::loginOCS() {
    VERBOSE_PRINT << "Using OCS backend with credentials:" << _myUsername << ", " << _myPassword;
    std::string someAsset = _myAssets.begin()->second->getAttributeString("uri");

    //OCS doesn't like foreign user agents, that's why we claim to be wget! [jb,ds]
    RequestPtr myLoginRequest(new Request( someAsset, "Wget/1.10.2"));
    // just do a head request ... no need to download the whole asset now.
    myLoginRequest->head();
    myLoginRequest->setCredentials(_myUsername, _myPassword, DIGEST);
    //myLoginRequest->setVerbose(true);

    _myRequestManager.performRequest(myLoginRequest);

    int myRunningCount = 0;
    do {
        myRunningCount = _myRequestManager.handleRequests();
        asl::msleep(10);
    } while (myRunningCount);

    if (myLoginRequest->getResponseCode() != 200 ||
            myLoginRequest->getResponseHeader("Set-Cookie").size() == 0) {
        throw CMSCacheException("Login failed for user '" + _myUsername +
                "' at URL '" + someAsset + "'.", PLUS_FILE_LINE);
    }

    _mySessionCookie = myLoginRequest->getResponseHeader("Set-Cookie");
}
*/
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
CMSCache::collectAssets(dom::NodePtr theParent) {
    for (unsigned i = 0; i < theParent->childNodesLength(); ++i) {
        dom::NodePtr myChild = theParent->childNode( i );
        if (myChild->nodeName() == "externalcontent") {
            const std::string & myPath = myChild->getAttributeString("path");
            if (_myAssets.find( myPath ) == _myAssets.end()) {
                if ( !myChild->getAttribute("uri") || myChild->getAttributeString("uri").empty() ) {
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

// theAsset->getAttributeString("path"), theAsset->getAttributeString("uri")
    
        std::vector<std::pair<std::string, std::string> > myOutdatedAssets;
        collectOutdatedAssets(myOutdatedAssets);
        _myRequestThread = RequestThreadPtr(new RequestThread(_myLocalPath, _myUsername, _myPassword, 
                    _myUserAgent, myOutdatedAssets, _myMaxRequestCount));
        // fillRequestQueue();
        AC_INFO << "forking";
        _myRequestThread->fork();
    }
}
#if 0
bool
CMSCache::testConsistency() {
    cerr << "Testing CMS Consistency..." << endl;

    _myAssets.clear();
    collectExternalAssetList();
    std::map<std::string, dom::NodePtr>::iterator it = _myAssets.begin();
    /*
    if (!_myUsername.empty() && !_myPassword.empty()) {
        login();
    }
    */

    unsigned myExistingAssetCount = 0;
    std::vector<dom::NodePtr> myMissingAssets;

    for (; it != _myAssets.end(); it++) {
        string myPath = it->second->getAttributeString("uri");
        //OCS doesn't like foreign user agents, that's why we claim to be wget! [jb,ds]
        RequestPtr myRequest(new Request(myPath, "Wget/1.10.2"));
        myRequest->setCookie(_mySessionCookie, true);
        myRequest->head();
        myRequest->setCredentials(_myUsername, _myPassword, DIGEST);
        _myRequestManager.performRequest(myRequest);
        int myRunningCount = 0;
        do {
            myRunningCount = _myRequestManager.handleRequests();
            asl::msleep(10);
        } while (myRunningCount);

        if (myRequest->getResponseCode() == 200) {
            myExistingAssetCount++;
        } else {
            myMissingAssets.push_back(it->second);
        }
        cerr << ".";
    }
    cerr << endl;

    cout << "Results: " << endl;
    cout << "  Existing Assets: " << myExistingAssetCount << endl;    
    cout << "  Missing Assets: " << myMissingAssets.size() << endl << endl;    
    for (unsigned i = 0; i < myMissingAssets.size(); ++i) {
        cout << "Missing Asset #" << i + 1 << ":" << endl;
        cout << "    name: " << myMissingAssets[i]->getAttributeString("name") << endl;
        cout << "    path: " << myMissingAssets[i]->getAttributeString("path") << endl;
    }

    return (myMissingAssets.size() == 0);
}
#endif
void
CMSCache::collectOutdatedAssets(std::vector<std::pair<std::string, std::string> > & theOutdatedAssets) {

    std::map<std::string, dom::NodePtr>::iterator myIter = _myAssets.begin();
    for (; myIter != _myAssets.end(); myIter++) {
        if ( isOutdated( myIter->second )) {
            VERBOSE_PRINT << "Asset " << myIter->second->getAttributeString("path")
                     << " may be outdated.";
            myIter->second->getAttribute("status")->nodeValue("outdated");
            theOutdatedAssets.push_back( make_pair(myIter->second->getAttributeString("path"), myIter->second->getAttributeString("uri")));
        } else {
            VERBOSE_PRINT << "Asset " << myIter->second->getAttributeString("path")
                     << " is uptodate.";
        }
    }

}

bool
CMSCache::isOutdated( dom::NodePtr theAsset ) {
    // always check by issuing a request
    return true;
}

void
CMSCache::updateDirectoryHierarchy() {
    std::map<std::string, dom::NodePtr>::iterator myIter = _myAssets.begin();
    while (myIter != _myAssets.end()) {
        std::string myPath = _myLocalPath + "/" +
                getDirectoryPart(myIter->second->getAttributeString("path"));
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
    while (_myRequestThread && _myRequestThread->popStatusMsg(myStatusMsg)) {
       std::map<std::string, dom::NodePtr>::iterator it = _myAssets.find(myStatusMsg.path);
       if (it == _myAssets.end()) {
            AC_WARNING << myStatusMsg.path << " not found in asset list";
       } else {
            if (it->second->getAttribute(myStatusMsg.attribute)) {
                it->second->getAttribute(myStatusMsg.attribute)->nodeValue(myStatusMsg.value);
            } else {
                it->second->appendAttribute(myStatusMsg.attribute, myStatusMsg.value);
            }
       }
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
