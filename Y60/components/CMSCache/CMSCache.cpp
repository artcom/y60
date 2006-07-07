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

namespace y60 {

CMSCache::CMSCache(const std::string & theServerURI,
                       const string & theLocalPath,
                       const string & thePresentationURI,                       
                       const std::string & theUsername,
                       const std::string & thePassword) :
     _myServerURI(theServerURI),
     _myPresentationURI(thePresentationURI),
     _myLocalPath( theLocalPath ),
     _myUsername(theUsername),
     _myPassword(thePassword),
     _myPresentationFile(new dom::Document())
{
    _myPresentationFile->parseFile(_myPresentationURI);
}

CMSCache::~CMSCache() {
    //AC_DEBUG << "closing Zip " << _myZipFilename;
}

void
CMSCache::login() {
    std::string someAsset = _myAssets.begin()->second->getAttributeString("uri");
    
    //OCS doesn't like foreign user agents, that's why we claim to be wget! [jb,ds]
    RequestPtr myLoginRequest(new Request( someAsset, "Wget/1.10.2"));
    if (_myUsername.size() && _myPassword.size()) {
        myLoginRequest->setCredentials(_myUsername, _myPassword);
    }
    _myRequestManager.performRequest(myLoginRequest);
    
    // TODO: timeouts
    int myRunningCount = 0;
    do {
        myRunningCount = _myRequestManager.handleRequests(); 
        asl::msleep(10);
    } while (myRunningCount);

    if (myLoginRequest->getResponseCode() != 200 ||
        myLoginRequest->getResponseHeader("Set-Cookie").size() == 0) {
            throw CMSCacheException("Login failed for user '"+_myUsername+"' at URL '"+someAsset+"'.", PLUS_FILE_LINE);
    }
    
    _mySessionCookie = myLoginRequest->getResponseHeader("Set-Cookie");
}

/*
void
CMSCache::loginCMS() {
   // std::string someAsset = _myAssets[0]->getAttributeString("uri");
    
    //OCS doesn't like foreign user agents, that's why we claim to be wget! [jb,ds]
    RequestPtr myLoginRequest(new Request("http://welt.bmw.artcom.de:8080/logged_in?__ac_name=zope&__ac_password=zope31", "Wget/1.10.2"));
    _myRequestManager.performRequest(myLoginRequest);
    
    int myRunningCount = 0;
    do {
        myRunningCount = _myRequestManager.handleRequests(); 
        asl::msleep(10);
    } while (myRunningCount);

    if (myLoginRequest->getResponseCode() != 200 ||
        myLoginRequest->getResponseHeader("Set-Cookie").size() == 0) {
            throw CMSCacheException("Login failed for user '"+_myUsername+"' at URL '"+_myServerURI+"'.", PLUS_FILE_LINE);
    }
    
    _mySessionCookie = myLoginRequest->getResponseHeader("Set-Cookie");
}
*/

void
CMSCache::collectExternalAssetList() {
    if ( ! _myPresentationFile || _myPresentationFile->childNodesLength() == 0 ) {
        throw CMSCacheException("No presentation file. Bailing out.", PLUS_FILE_LINE);
    }
    
    dom::NodePtr myRoot;
    if (_myPresentationFile->childNode(0)->nodeType() == dom::Node::PROCESSING_INSTRUCTION_NODE) {
        myRoot = _myPresentationFile->childNode(1)->childNode("themepool", 0);
    } else {
        myRoot = _myPresentationFile->childNode(0)->childNode("themepool", 0);
    }
    
    if ( ! myRoot ) {
        throw CMSCacheException("Failed to find themepool", PLUS_FILE_LINE);
    }
    
    collectAssets(myRoot);

    AC_PRINT << "Found " << _myAssets.size() << " assets.";
}

void
CMSCache::collectAssets(dom::NodePtr theParent) {
    for (unsigned i = 0; i < theParent->childNodesLength(); ++i) {
        dom::NodePtr myChild = theParent->childNode( i );
        if (myChild->nodeName() == "externalcontent") {
            const std::string & myPath = myChild->getAttributeString("path");
            if (_myAssets.find( myPath ) == _myAssets.end()) {
                _myAssets.insert( std::make_pair(myPath, myChild ));
            }
        }
        collectAssets( myChild );
    }    
}

void
CMSCache::addAssetRequest(dom::NodePtr theAsset) {
    std::string myLocalPath = _myLocalPath + "/" + theAsset->getAttributeString("path");
    AC_PRINT << "Fetching " << myLocalPath;
    AssetRequestPtr myRequest(new AssetRequest( theAsset->getAttributeString("uri"),
                                      myLocalPath, _mySessionCookie));
    //myRequest->setCookie(_mySessionCookie, true);
    _myAssetRequests.insert(std::make_pair( & ( * theAsset), myRequest));
    _myRequestManager.performRequest(myRequest);
}

void
CMSCache::synchronize() {

    collectExternalAssetList();

    updateDirectoryHierarchy();
    removeStalledAssets();
    
    if ( ! _myAssets.empty()) {
        login();
        collectOutdatedAssets();
        fillRequestQueue();
    }
}

void
CMSCache::collectOutdatedAssets() {
    std::map<std::string, dom::NodePtr>::iterator myIter = _myAssets.begin();
    for (; myIter != _myAssets.end(); myIter++) {
        if ( isOutdated( myIter->second )) {
            AC_PRINT << "Asset " << myIter->second->getAttributeString("path")
                     << " is outtdated.";
            _myOutdatedAssets.push_back( myIter->second );
        }
    }
}

bool
CMSCache::isOutdated( dom::NodePtr theAsset ) {
    std::string myFile = _myLocalPath + "/" + theAsset->getAttributeString("path");
    if ( fileExists( myFile )) {
        time_t myLocalTimestamp = getLastModified( myFile );
        time_t myServerTimestamp = Request::getTimeFromHTTPDate( theAsset->getAttributeString("lastmodified"));
        if (myServerTimestamp <= myLocalTimestamp) {
            return false;
        } 
    }
    return true;
}

void
CMSCache::fillRequestQueue() {
    for (unsigned i = _myAssetRequests.size(); i < MAX_REQUESTS; ++i) {
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
                std::string myFilename = _myLocalPath + "/" + myIter->first->getAttributeString("path");
                /*
                const asl::Block & myBlock = myIter->second->getResponseBlock();
                AC_PRINT << "Recieved " << myFilename << " (" << myBlock.size() << "bytes).";
                writeFile( myFilename,  myBlock);
                */
                time_t myTime = Request::getTimeFromHTTPDate(
                        myIter->first->getAttributeString("lastmodified"));
                _myAssetRequests.erase( myIter++ );
                setLastModified(myFilename, myTime);
            } else {
                // TODO: retry handling
            }
        } else {
            ++myIter;
        }
    }

    fillRequestQueue();
    return _myOutdatedAssets.empty() && _myAssetRequests.empty() && (myRunningCount == 0);
}


void
CMSCache::updateDirectoryHierarchy() {
    if ( ! fileExists( _myLocalPath )) {
        createPath( _myLocalPath );
    }
    std::map<std::string, dom::NodePtr>::iterator myIter = _myAssets.begin();
    while (myIter != _myAssets.end()) {
        std::string myPath = _myLocalPath + "/" + getDirectoryPart(myIter->second->getAttributeString("path"));
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

void
CMSCache::scanStalledEntries(const std::string & thePath) {
    std::vector<std::string> myEntries = getDirectoryEntries(thePath);
    std::vector<std::string>::iterator myIter = myEntries.begin();
    for (; myIter != myEntries.end(); ++myIter) {
        std::string myEntry = thePath + "/" + (*myIter);
        if (isDirectory(myEntry)) {
            scanStalledEntries(myEntry);
        } else {
            std::string myFilename = myEntry.substr(_myLocalPath.size() + 1, myEntry.size() - _myLocalPath.size());
            if (_myAssets.find(myFilename) == _myAssets.end()) {
                AC_PRINT << "Removing '" << myFilename << "'.";
                deleteFile(_myLocalPath + "/" + myFilename);
            }
        }
    }
}


}
