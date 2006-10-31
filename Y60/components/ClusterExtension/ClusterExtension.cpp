//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: ClusterExtension.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.18 $
//
// Description:
//
//=============================================================================

#include <asl/PlugInBase.h>
#include <y60/IRendererExtension.h>
#include <y60/IScriptablePlugin.h>
#include <y60/JSScriptablePlugin.h>
#include <y60/AbstractRenderWindow.h>
#include <y60/Scene.h>
#include <y60/NodeNames.h>
#include <y60/KeyEvent.h>

#include <asl/Station.h>
#include <asl/MappedBlock.h>
#include <asl/Dashboard.h>

#include <asl/Dashboard.h>

#include <asl/ConduitServer.h>
#include <asl/ConduitAcceptor.h>
#include <asl/TCPPolicy.h>
#include <asl/MessageAcceptor.h>
#include <asl/net_functions.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;
using namespace dom;
using namespace y60;

DEFINE_EXCEPTION(CubeNotFound, asl::Exception)

#define DB(x)// x
#define DB_Y(x) x

#define USE_FILE_IO

const unsigned long CLUSTER_MAGIC_MESSAGE_ID = 0xc111a91c;
const unsigned long CLUSTER_SCENE_UPDATE = 0x1;

struct SceneUpdateHeader {
    unsigned long magic;
    unsigned long cmd;
    unsigned long long lastVersion;
    unsigned long long thisVersion;
};

class ClusterExtension :
    public PlugInBase,
    public y60::IRendererExtension,
    public IScriptablePlugin
{
public:
    ClusterExtension(asl::DLHandle theDLHandle);

    void onStartup(jslib::AbstractRenderWindow * theWindow);

    bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow);

    void handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent);
    void onFrame(AbstractRenderWindow * theWindow , double t);

    void onPreRender(AbstractRenderWindow * theWindow);
    void onPostRender(AbstractRenderWindow * theWindow);

    const char * ClassName() {
        static const char * myClassName = "rendexCluster";
        return myClassName;
    }

    void onUpdateSettings(dom::NodePtr theSettings) {
        cerr << "New Settings: " << *theSettings << endl;
    }
    void onGetProperty(const std::string & thePropertyName,
        PropertyValue & theReturnValue) const;

    void onSetProperty(const std::string & thePropertyName,
        const PropertyValue & thePropertyValue);

    void onKey(y60::Event & theEvent);
protected:
    typedef TCPPolicy POLICY;
    typedef asl::Ptr<asl::MessageAcceptor<POLICY> > AcceptorPtr;
    typedef POLICY::Endpoint EndPoint;
    typedef Conduit<POLICY>::Ptr ClientPtr;

    static unsigned long defaultLocalServerIPAddress() {
        unsigned long myAddress = 0;
        if (getenv("CLUSTER_LOCAL_SERVER_IP_ADDR")) {
            myAddress = asl::hostaddress(getenv("CLUSTER_LOCAL_SERVER_IP_ADDR"));
        } else {
            myAddress = asl::hostaddress(asl::localhostname());
        }
        return myAddress;
    }
    static unsigned short defaultServerPort() {
        unsigned short myBroadcastPort = 19668;
        if (getenv("CLUSTER_LOCAL_SERVER_PORT")) {
            myBroadcastPort = asl::as<unsigned short>(getenv("CLUSTER_LOCAL_SERVER_PORT"));
        }
        return myBroadcastPort;
    }
    static AcceptorPtr startServer(EndPoint & myLocalServerEndPoint) {
        myLocalServerEndPoint = EndPoint(defaultLocalServerIPAddress(), defaultServerPort());
        AcceptorPtr myAcceptor = AcceptorPtr(new asl::MessageAcceptor<POLICY>(myLocalServerEndPoint, asl::MessageServer<POLICY>::create));
        if (!myAcceptor->start()) {
            cerr <<"#ERROR: ClusterExtension::startServer(): could not start server."<<endl;
            return AcceptorPtr(0);
        }
        return myAcceptor;
    }

    static ClientPtr connectClient(const EndPoint & myRemoteEndPoint) {
        ClientPtr myClient(new Conduit<POLICY>(myRemoteEndPoint));
        return myClient;
    }
    void collectScene(jslib::AbstractRenderWindow * theWindow,
                        unsigned long long theLastVersion,
                        asl::ResizeableBlock & theDestination);
    void serveRequests(AbstractRenderWindow * theWindow);
    void ensureClientConnection(ClientPtr & theClient);
    void requestUpdate(AbstractRenderWindow * theWindow, ClientPtr & theClient, unsigned long long & theLastVersion);
    bool updateSceneDom(jslib::AbstractRenderWindow * theWindow,
                    const asl::ReadableBlock & theUpdate,
                    unsigned long long & theLastVersion,
                    unsigned long long & thePreviousLocalVersion);

    void updateRenderer(jslib::AbstractRenderWindow * theWindow, unsigned long long thePreviousDocumentVersion);
    bool receiveUpdate(AbstractRenderWindow * theWindow, ClientPtr & theClient, unsigned long long & theLastVersion);

private:
    Station _myStation;
    bool _myMasterFlag;
    bool _mySlaveFlag;
    bool _myEnableFlag;
    bool _myFileIOFlag;
    std::string _myRemoteServer;
    unsigned long long _myFrame;
    unsigned long long _myRemoteDomVersion;

    EndPoint _myLocalServerEndpoint;
    EndPoint _myRemoteEndpoint;
    AcceptorPtr _myAcceptor;
    ClientPtr _myClient;
    bool _updateRequested;
};

ClusterExtension :: ClusterExtension(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    IRendererExtension("rendexCluster"),
        _myMasterFlag(false),
        _mySlaveFlag(false),
        _myEnableFlag(true),
        _myFileIOFlag(true),
        _myFrame(0),
        _myRemoteDomVersion(0),
        _myRemoteServer(asl::localhostname()),
        _updateRequested(false)
{
    cerr << "ClusterExtension:: ClusterExtension() "  << endl;
}

void
ClusterExtension::onGetProperty(const std::string & thePropertyName,
                                    PropertyValue & theReturnValue) const
{
    if (thePropertyName == "master") {
        theReturnValue.set<bool>(_myMasterFlag);
    } else if (thePropertyName == "enabled") {
        theReturnValue.set<bool>(_myEnableFlag);
   } else if (thePropertyName == "slave") {
        theReturnValue.set<bool>(_mySlaveFlag);
    } else if (thePropertyName == "fileio") {
        theReturnValue.set<bool>(_myFileIOFlag);
    } else if (thePropertyName == "remoteserver") {
        theReturnValue.set<std::string>(_myRemoteServer);
    } else {
        cerr << "### WARNING: ClusterExtension::onGetProperty: Unknown property: " << thePropertyName << endl;
    }
}

void
ClusterExtension::onSetProperty(const std::string & thePropertyName,
                                const PropertyValue & thePropertyValue)
{
    cerr << "ClusterExtension::onSetProperty " << thePropertyName << "="<< thePropertyValue.get<std::string>() << endl;
    if (thePropertyName == "master") {
        _myMasterFlag = thePropertyValue.get<bool>();
    } else if (thePropertyName == "slave") {
        _mySlaveFlag = thePropertyValue.get<bool>();
    } else if (thePropertyName == "remoteserver") {
        _myRemoteServer = thePropertyValue.get<std::string>();
    } else if (thePropertyName == "enabled") {
        bool myNewEnableFlag = thePropertyValue.get<bool>();
        if (_myEnableFlag != myNewEnableFlag) {
            if (myNewEnableFlag) {
                _myStation.openStationDefault(
                    Station::defaultBroadcastAddress(),
                    Station::defaultBroadcastPort(),
                    Station::defaultBroadcastPort(),
                    Station::defaultIgnoreAddress(),
                    Station::defaultStationID(),
                    Station::defaultNetworkID());
            } else {
                _myStation.closeStation();
            }
            _myEnableFlag = myNewEnableFlag;
        }
    } else {
        cerr << "### WARNING: ClusterExtension::onSetProperty: Unknown property: " << thePropertyName << endl;
    }
}
void
ClusterExtension:: onStartup(jslib::AbstractRenderWindow * theWindow) {
    cerr << "ClusterExtension::onStartup() "  << endl;
}

bool
ClusterExtension::onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
    DB(AC_TRACE << "ClusterExtension::onSceneLoaded() "  << endl);

    if (_myMasterFlag) {
        _myAcceptor = startServer(_myLocalServerEndpoint);
    }
    return true;
}

void
printChangedNodes(NodePtr theNode, unsigned long long theLastVersion, int theLevel) {
    if (theNode->nodeVersion() > theLastVersion || theLastVersion == 0xffffffffffffffffULL) {
        std::string myPrefix;
        for (int i = 0; i < theLevel; ++i) {
            myPrefix += "  ";
        }
        cerr << myPrefix << theNode->nodeTypeName() << " '"<< theNode->nodeName() <<"', V="<<theNode->nodeVersion()<<", UID="<<theNode->getUniqueId()<<endl;
        //cerr << "=" << endl;
        //cerr << theNode->nodeValue() << endl;
        for (unsigned i = 0; i < theNode->attributesLength(); ++i ) {
            printChangedNodes(theNode->attributes().item(i), theLastVersion, theLevel+1);
        }
        for (unsigned i = 0; i < theNode->childNodesLength(); ++i ) {
            printChangedNodes(theNode->childNode(i), theLastVersion, theLevel+1);
        }
    }
}



void
ClusterExtension::collectScene(jslib::AbstractRenderWindow * theWindow,
                               unsigned long long theLastVersion,
                               asl::ResizeableBlock & theDestination) {
    MAKE_SCOPE_TIMER(ClusterExtension_collectFullScene);
    y60::ScenePtr myScene = theWindow->getRenderer()->getCurrentScene();
    if (myScene) {
        dom::DocumentPtr mySceneDom = myScene->getSceneDom();
        unsigned long long myDocumentVersion = mySceneDom->nodeVersion();

        SceneUpdateHeader myHeader;
        myHeader.magic = CLUSTER_MAGIC_MESSAGE_ID;
        myHeader.cmd = CLUSTER_SCENE_UPDATE;
        myHeader.lastVersion = theLastVersion;
        myHeader.thisVersion = myDocumentVersion;

        theDestination.appendData(myHeader);

        {
            MAKE_SCOPE_TIMER(ClusterExtension_reverseUpdateShapes);
            //myScene->reverseUpdateShapes();
        }

        if (theLastVersion != 0) {
            mySceneDom->makePatch(theDestination, theLastVersion);
            DB_Y(cerr << "ClusterExtension::collectScene: collected update patch from theLastVersion = "<<theLastVersion<<endl);
        } else {
            mySceneDom->binarize(theDestination);
            DB_Y(cerr << "ClusterExtension::collectScene: collected full scene, theLastVersion = "<<theLastVersion<<endl);
        }
    } else {
        cerr << "ClusterExtension::collectScene: no scene found." << endl;
    }
}

void
ClusterExtension::serveRequests(AbstractRenderWindow * theWindow) {
    if (_myAcceptor) {
        asl::Ptr<MessageAcceptor<POLICY>::Message> myMessage;
        while (myMessage = _myAcceptor->popIncomingMessage()) {
            DB(AC_TRACE << "ClusterExtension::serveRequests: incoming message:"<<endl<<myMessage->as_string()<<endl);
            dom::Document myRequest(myMessage->as_string());
            if (myRequest) {
                dom::Node & myDocElem = myRequest("y60master");
                if (myDocElem) {
                    if (Node & myLastVersionAttr = myDocElem("getscene")["lastversion"]) {
                        unsigned long long myLastVersion;
                        if (fromString(myLastVersionAttr.nodeValue(), myLastVersion)) {
                            asl::Block myResponse;
                            collectScene(theWindow, myLastVersion, myResponse);
                            DB_Y(cerr << "ClusterExtension::serveRequests: responding with msg size = "<<myResponse.size()<<endl);
                            _myAcceptor->pushOutgoingMessage(myMessage->server, myResponse);
                        } else {
                            cerr << "#ERROR: ClusterExtension::serveRequests: bad version number in: "<<endl<<myMessage->as_string()<<endl;
                        }
                    } else {
                        cerr << "#ERROR: ClusterExtension::serveRequests: bad nodes in: "<<endl<<myMessage->as_string()<<endl;
                    }
                } else {
                    cerr << "#ERROR: ClusterExtension::serveRequests: bad element node in: "<<endl<<myMessage->as_string()<<endl;
                }
            } else {
                cerr << "#ERROR: ClusterExtension::serveRequests: could not parse request message: "<<endl<<myMessage->as_string()<<endl;
            }
        }
    }
}

void
ClusterExtension::ensureClientConnection(ClientPtr & theClient) {
    while (!theClient) {
        try {
            _myRemoteEndpoint = EndPoint(asl::hostaddress(_myRemoteServer), defaultServerPort());
            theClient = connectClient(_myRemoteEndpoint);
        }
        catch (asl::Exception & ex) {
            cerr << "#ERROR: ClusterExtension::ensureClientConnection: connect failed:" << ex << endl;
            msleep(1000);
        }
    }
}

void
ClusterExtension::requestUpdate(AbstractRenderWindow * theWindow, ClientPtr & theClient, unsigned long long & theLastVersion) {
    bool delivered = false;
    do {
        ensureClientConnection(theClient);
        dom::Document myDocument;
        myDocument("y60master")("getscene")["lastversion"].nodeValue(asl::as_string(theLastVersion));
        std::string myRequestString = asl::as_string(myDocument);
        DB(AC_TRACE << "ClusterExtension::requestUpdate: "<<endl<<myRequestString<<endl);
        DB_Y(cerr << "ClusterExtension::requestUpdate from "<<theLastVersion<<", localversion="<<
            theWindow->getRenderer()->getCurrentScene()->getSceneDom()->nodeVersion()<<endl);
        try {
            theClient->send(myRequestString);
            delivered = true;
        }
        catch (asl::Exception & ex) {
            cerr << "#ERROR: ClusterExtension::requestUpdate: send failed:" << ex << endl;
            msleep(100);
            theClient = ClientPtr(0);
            theLastVersion = 0;
        }
    } while (!delivered);
}


bool
ClusterExtension::updateSceneDom(jslib::AbstractRenderWindow * theWindow,
                    const asl::ReadableBlock & theUpdate,
                    unsigned long long & theLastVersion,
                    unsigned long long & thePreviousLocalVersion)
{
    MAKE_SCOPE_TIMER(ClusterExtension_updateSceneDom);
    y60::ScenePtr myScene = theWindow->getRenderer()->getCurrentScene();
    if (myScene) {
        SceneUpdateHeader myHeader;
        int myPos = theUpdate.readData(myHeader, 0);
        if (myHeader.magic == CLUSTER_MAGIC_MESSAGE_ID &&
            myHeader.cmd == CLUSTER_SCENE_UPDATE &&
            myHeader.lastVersion == theLastVersion)
        {
            DB(AC_TRACE << "ClusterExtension::updateSceneDom: update matches, myHeader.thisVersion = " <<myHeader.thisVersion<< endl);
            try {
                dom::DocumentPtr mySceneDom = myScene->getSceneDom();
                thePreviousLocalVersion = mySceneDom->nodeVersion();
                DB_Y(cerr << "ClusterExtension::updateSceneDom: updating from localversion="<<thePreviousLocalVersion<<endl);
                if (theLastVersion != 0) {
                    mySceneDom->applyPatch(theUpdate, myPos);
                } else {
                    myScene->clear();
                    mySceneDom = myScene->getSceneDom();
                    myScene->setupEmptyDocument(*mySceneDom,"");
                    mySceneDom->debinarize(theUpdate, myPos);
                }
                theLastVersion = myHeader.thisVersion;
                return true;
            }
            catch (asl::Exception & ex) {
                cerr << "ClusterExtension::updateSceneDom: debinarize/applyPatch failed:" << ex << endl;
                myScene->clear();
                theLastVersion = 0;
                thePreviousLocalVersion = 0;
                return true;
            }
        } else {
            cerr << "ClusterExtension::updateSceneDom: update packet mismatch:" << endl;
            if (myHeader.magic != CLUSTER_MAGIC_MESSAGE_ID) {
                cerr << "ClusterExtension::updateSceneDom: myHeader.magic =" <<
                    myHeader.magic<<", expected "<<CLUSTER_MAGIC_MESSAGE_ID<<endl;
            }
            if (myHeader.cmd != CLUSTER_SCENE_UPDATE) {
                cerr << "ClusterExtension::updateSceneDom: myHeader.cmd =" <<
                    myHeader.cmd<<", expected "<<CLUSTER_SCENE_UPDATE<<endl;
            }
            if (myHeader.lastVersion != theLastVersion) {
                cerr << "ClusterExtension::updateSceneDom: myHeader.lastVersion =" <<
                    myHeader.lastVersion<<", expected "<<theLastVersion<<endl;
            }
         }
    } else {
        cerr << "ClusterExtension::updateSceneDom: no scene found." << endl;
    }
    return false;
}

#undef DB
#define DB(x) // x
void
ClusterExtension::updateRenderer(jslib::AbstractRenderWindow * theWindow,
                                 unsigned long long thePreviousDocumentVersion)
{
    // XXX DS: This function is almost the same like Scene->updateAllModified(). It
    //         also contains the last call to Scene->update() ....  :-(
    // TODO: merge with Scene->updateAllModified() in an ellegant way.
    y60::ScenePtr myScene = theWindow->getRenderer()->getCurrentScene();
    dom::DocumentPtr mySceneDom = myScene->getSceneDom();
    if (thePreviousDocumentVersion > mySceneDom->nodeVersion()) {
        // we have loaded a completely new dom
        myScene->setup();
    } else {
        dom::NodePtr mySceneElement = mySceneDom->childNode(y60::SCENE_ROOT_NAME);
        if (mySceneElement) {

            //printChangedNodes(mySceneElement, thePreviousDocumentVersion, 0);

            dom::NodePtr myWorldListElement = mySceneElement->childNode(y60::WORLD_LIST_NAME);
            dom::NodePtr myWorldElement = myWorldListElement ? myWorldListElement->childNode(y60::WORLD_NODE_NAME) : dom::NodePtr(0);
            dom::NodePtr myImagesElement = mySceneElement->childNode(y60::IMAGE_LIST_NAME);
            dom::NodePtr myShapesElement = mySceneElement->childNode(y60::SHAPE_LIST_NAME);
            dom::NodePtr myCanvasesElement = mySceneElement->childNode(y60::CANVAS_LIST_NAME);
            dom::NodePtr myMaterialsElement = mySceneElement->childNode(y60::MATERIAL_LIST_NAME);
            dom::NodePtr myAnimationsElement = mySceneElement->childNode(y60::ANIMATION_LIST_NAME);

            short myUpdateFlags = 0;
            DB(AC_TRACE <<"Updating:" << endl);
            if (myWorldElement->nodeVersion() > thePreviousDocumentVersion) {
                myUpdateFlags|=Scene::WORLD;
                DB(AC_TRACE << " WORLD");
            }
            if (myShapesElement->nodeVersion() > thePreviousDocumentVersion) {
                myUpdateFlags|=Scene::SHAPES;
                DB(AC_TRACE << " SHAPES");
            }
            if (myCanvasesElement->nodeVersion() > thePreviousDocumentVersion) {
                DB(AC_TRACE << " CANVASES");
            }
            if (myMaterialsElement->nodeVersion() > thePreviousDocumentVersion) {
                myUpdateFlags|=Scene::MATERIALS;
                DB(AC_TRACE << " MATERIALS");
            }
            if (myAnimationsElement->nodeVersion() > thePreviousDocumentVersion) {
                myUpdateFlags|=Scene::ANIMATIONS_LOAD;
                myUpdateFlags|=Scene::ANIMATIONS;
                DB(AC_TRACE << " ANIMATIONS");
            }
            DB(AC_TRACE << endl);
            myScene->update(myUpdateFlags);
        }
    }
}
#undef DB
#define DB(x)

bool
ClusterExtension::receiveUpdate(AbstractRenderWindow * theWindow, ClientPtr & theClient, unsigned long long & theLastVersion) {
    ensureClientConnection(theClient);
    try {
        asl::Block myBlock;
        if (theClient->receive(myBlock)) {
            DB(AC_TRACE << "ClusterExtension::receiveUpdate: received msg with size = "<<myBlock.size()<<endl);
            unsigned long long myPreviousLocalVersion = 0;
            if (updateSceneDom(theWindow, myBlock, theLastVersion, myPreviousLocalVersion)) {
                updateRenderer(theWindow, myPreviousLocalVersion);
                return true;
            }
        }
    }
    catch (asl::Exception & ex) {
        cerr << "ClusterExtension::receiveUpdate: receive failed:" << ex << endl;
    }
    return false;
}

void
ClusterExtension :: handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {
}

void
ClusterExtension::onKey(y60::Event & theEvent) {
    y60::KeyEvent & myKeyEvent = dynamic_cast<y60::KeyEvent&>(theEvent);
    DB(
        cerr << "Key Event" << endl;
    cerr << "    string: " << myKeyEvent.keyString << endl;
    cerr << "    state:  " << (myKeyEvent.type == y60::Event::KEY_DOWN) << endl;
    cerr << "    shift:  " << ((myKeyEvent.modifiers & y60::KEYMOD_SHIFT) !=0) << endl;
    cerr << "    ctrl:   " << ((myKeyEvent.modifiers & y60::KEYMOD_CTRL) !=0) << endl;
    cerr << "    alt:    " << ((myKeyEvent.modifiers & y60::KEYMOD_ALT) !=0) << endl;
    );
}


void
ClusterExtension::onFrame(AbstractRenderWindow * theWindow , double t) {
    DB(AC_TRACE << "ClusterExtension::onFrame() t = "  << t<< endl);
    if (_mySlaveFlag) {
        if (!_updateRequested) {
            // only request update if there is no request pending
            requestUpdate(theWindow, _myClient, _myRemoteDomVersion);
            _updateRequested = true;
        }
        if (receiveUpdate(theWindow, _myClient, _myRemoteDomVersion)) {
            // request new update if we got one
            _updateRequested = false;
        }
    }
    if (_myMasterFlag) {
        serveRequests(theWindow);
    }
}


void
ClusterExtension :: onPreRender(AbstractRenderWindow * theWindow) {
}

void
ClusterExtension::onPostRender(AbstractRenderWindow * theWindow) {

}

extern "C"
EXPORT PlugInBase* rendexCluster_instantiatePlugIn(DLHandle myDLHandle) {
    return new ClusterExtension(myDLHandle);
}
