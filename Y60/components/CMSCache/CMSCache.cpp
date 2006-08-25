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
#include <y60/Request.h>

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
     _myPresentationDocument(thePresentationDocument),
     _myLocalPath( theLocalPath ),
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
}

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

    VERBOSE_PRINT << "Found " << _myAssets.size() << " assets.";
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
CMSCache::addAssetRequest(dom::NodePtr theAsset) {
    VERBOSE_PRINT << "Fetching " << _myLocalPath + "/" + theAsset->getAttributeString("path");
    AssetRequestPtr myRequest;
    if (_myUserAgent.empty()) {
        myRequest = AssetRequestPtr(new AssetRequest( theAsset, _myLocalPath, _mySessionCookie));
    } else {
        myRequest = AssetRequestPtr(new AssetRequest( theAsset, _myLocalPath, _mySessionCookie, _myUserAgent));
    }

    if (_myBackendType == SVN) {
        myRequest->setCredentials(_myUsername, _myPassword, DIGEST);
    }
    _myAssetRequests.insert(std::make_pair( & ( * theAsset), myRequest));
    theAsset->getAttribute("status")->nodeValue("downloading");
    theAsset->appendAttribute("progress", "0.0");
    _myRequestManager.performRequest(myRequest);
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

    if ( ! _myAssets.empty()) {
        if ( ! _myUsername.empty() && ! _myPassword.empty()) {
            login();
        }
        collectOutdatedAssets();
        fillRequestQueue();
    }
}

bool
CMSCache::testConsistency() {
    cerr << "Testing CMS Consistency..." << endl;

    _myAssets.clear();
    collectExternalAssetList();
    std::map<std::string, dom::NodePtr>::iterator it = _myAssets.begin();
    if (!_myUsername.empty() && !_myPassword.empty()) {
        login();
    }

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

void
CMSCache::collectOutdatedAssets() {
    std::map<std::string, dom::NodePtr>::iterator myIter = _myAssets.begin();
    for (; myIter != _myAssets.end(); myIter++) {
        if ( isOutdated( myIter->second )) {
            VERBOSE_PRINT << "Asset " << myIter->second->getAttributeString("path")
                     << " is outdated.";
            myIter->second->getAttribute("status")->nodeValue("outdated");
            _myOutdatedAssets.push_back( myIter->second );
        } else {
            VERBOSE_PRINT << "Asset " << myIter->second->getAttributeString("path")
                     << " is uptodate.";
        }
    }
}

bool
CMSCache::isOutdated( dom::NodePtr theAsset ) {
    std::string myFile = _myLocalPath + "/" + theAsset->getAttributeString("path");
    if ( fileExists( myFile )) {
        time_t myLocalTimestamp = getLastModified( myFile );
        time_t myServerTimestamp = Request::getTimeFromHTTPDate(
                    theAsset->getAttributeString("lastmodified"));
        if (myServerTimestamp <= myLocalTimestamp) {
            return false;
        }
    }
    return true;
}

void
CMSCache::fillRequestQueue() {
    for (unsigned i = _myAssetRequests.size(); i < _myMaxRequestCount; ++i) {
        if ( ! _myOutdatedAssets.empty()) {
            dom::NodePtr myAsset = _myOutdatedAssets.back();
            _myOutdatedAssets.pop_back();
            addAssetRequest( myAsset );
        } else {
            break;
        }
    }
}

bool
CMSCache::isSynchronized() {
    int myRunningCount = _myRequestManager.handleRequests();

    // TODO: Error statistics and handling
    AssetRequestMap::iterator myIter = _myAssetRequests.begin();
    while (myIter != _myAssetRequests.end()) {
        if (myIter->second->isDone()) {
            int myResponseCode = myIter->second->getResponseCode();
            if ( myResponseCode == 200) {
                std::string myFilename = _myLocalPath + "/" +
                        myIter->first->getAttributeString("path");
                time_t myTime = Request::getTimeFromHTTPDate(
                        myIter->first->getAttributeString("lastmodified"));
                setLastModified(myFilename, myTime);
            } else {
                string myReason = myIter->second->getResponseHeader("X-ORA-CONTENT-Info");
                if (!myReason.empty()) {
                    AC_PRINT << "OCS server reason: '" << myReason << "' .";
                }
                // TODO: retry handling
            }
            _myAssetRequests.erase( myIter++ );
        } else {
            AC_TRACE << "  still running:" << myIter->second->getResponseCode() << ":" << myIter->first->getAttributeString("path");
            ++myIter;
        }
    }

    fillRequestQueue();
    return _myOutdatedAssets.empty() && _myAssetRequests.empty() && (myRunningCount == 0);
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
