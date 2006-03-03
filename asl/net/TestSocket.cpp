/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin AG
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: TestSocket.cpp,v $
//
//   $Revision: 1.17 $
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Socket.h"
#include "TCPServer.h"
#include "TestSocket.h"
#include "TCPClientSocket.h"
#include "UDPSocket.h"
#include "UDPConnection.h"
#include <asl/os_functions.h>
#include <asl/Logger.h>

#include <string>
#include <iostream>
#include <typeinfo>
#include <pthread.h>

using namespace std;
using namespace asl;
using namespace inet;


void TestSocket::testHostAddressMethods () {

    const int hostNameLength = 128;
#ifdef TEST_NON_LOCAL_HOST
    const char * inHostDotName = "10.1.1.1";
    const char * inHostName = "erwin.artcom.de";
    Unsigned32 inHostAddress = 0x0a010101;  // host byte order
#else
    const char * inHostDotName = "127.0.0.1";
    const char * inHostName = "localhost";
    const char * inFQHostName = "localhost.localdomain";
    Unsigned32 inHostAddress = 0x7f000001;  // host byte order
#endif
    int myErrCode;

    ENSURE (sizeof(inHostAddress) == 4);

    ENSURE (addrFromStr(inHostDotName) == inHostAddress);

    char outHostDotName [hostNameLength];
    addr2Str(inHostAddress, outHostDotName);
    ENSURE (strcmp(inHostDotName, outHostDotName) == 0);

    // static getHostAddress (const char *hostspec);
    // Liefert die Hostadresse aus einem Namen oder einer a.b.c.d Adresse
    // throws a SocketException if no valid address can be determined
    ENSURE (getHostAddress (inHostDotName) == inHostAddress);
    ENSURE (getHostAddress (inHostName) == getHostAddress (inHostDotName));
    ENSURE (getHostAddress ("BROADCAST") == (Unsigned32) 0xffffffff);

    try {
        getHostAddress ("does.not.exist");
    }
    catch (SocketException & e) {
        myErrCode = e.getErrorCode();
        cerr << e.what() << endl;
    }
    ENSURE (myErrCode = HOST_NOT_FOUND);
    myErrCode = 0;

    // Schreibt den von gethostbyname(3N) gelieferten String nach namedest
    // und liefert rnTRUE.
    // Wenn hostname nicht bekannt, wird <NAME UNKNOWN> eingetragen und
    // rnFALSE zurueckgeliefert.

    char outHostName [hostNameLength];
    Unsigned32 unknownHostAddress = 0x01010101;

    ENSURE (getHostName(inHostAddress, outHostName,  hostNameLength)==0);
    ENSURE (strcmp (outHostName, "<NAME UNKNOWN>") != 0);
    DPRINT (outHostName);
    //ENSURE (strcmp (outHostName, inHostName) == 0);

    ENSURE (getHostName(unknownHostAddress, outHostName,  hostNameLength)!=0);
    ENSURE (strcmp (outHostName, "<NAME UNKNOWN>") == 0);

    // test new modern string and exception based interface
    // throws a SocketException if the hostname is unknown
    string strHostname = getHostName(inHostAddress);
    DPRINT (strHostname);
    DPRINT (inHostName);
    DPRINT (asl::hostname());
    ENSURE (strHostname == string(inHostName) || strHostname == string(inFQHostName) || strHostname == asl::hostname());

    try {
        getHostName(unknownHostAddress);
    }
    catch (SocketException & e) {
        myErrCode = e.getErrorCode();
        cerr << e.what() << endl;
    }
    ENSURE (myErrCode == HOST_NOT_FOUND ||
            myErrCode == NO_DATA);

    ENSURE(addr2Str(inHostAddress) == string(inHostDotName));

}

void TestSocket::BaseTest() {
    Socket theSocket(INADDR_ANY, 80);
    ENSURE_EXCEPTION(theSocket.peek(10), SocketException);
    char myData[32];
    ENSURE_EXCEPTION(theSocket.receive(myData, 32), SocketException);
    ENSURE_EXCEPTION(theSocket.send("Hallo",5), SocketException);
}

void TestSocket::TCPTest() {
    pthread_t myThread;
    int myResult;

    TCPServer * myTCPServer;
    int serverPort;
    const int MIN_PORT = 8000;
    const int MAX_PORT = 8080;

    // TODO: linux test (not that easy, socket to be reused must be in state TIME_WAIT
    // see http://www.unixguide.net/network/socketfaq/4.5.shtml for more info
#ifdef WIN32    
    // try to create a tcp socket twice (already in use test)
    try {
        myTCPServer = new TCPServer(INADDR_ANY, MIN_PORT);
    }
    catch (SocketException & se)
    {
        cerr << "TestSocket::TCPTest() " << "failed to listen on port " << MIN_PORT << ":" << se.where() << endl;
        myTCPServer = 0;
    }
    // do it again
    try {
        myTCPServer = new TCPServer(INADDR_ANY, MIN_PORT, true);
    }
    catch (SocketException & se)
    {
        cerr << "TestSocket::TCPTest() " << "failed to listen on port " << MIN_PORT << ":" << se.where() << endl;
        myTCPServer = 0;
    }
    ENSURE_MSG(myTCPServer!=0, "Reuse a bound tcp server rcv socket");
#endif

    // try to find a free server port between MIN_PORT and MAX_PORT;
    for (serverPort = MIN_PORT; serverPort <= MAX_PORT; serverPort++)
    {
        try {
            myTCPServer = new TCPServer(INADDR_ANY, serverPort);
            break;
        }
        catch (SocketException & se)
        {
            cerr << "TestSocket::TCPTest() " << "failed to listen on port " << serverPort << ":" << se.where() << endl;
            myTCPServer = 0;
        }
    }
    ENSURE(myTCPServer != 0);

    // start server thread
    myResult = pthread_create(&myThread, NULL, TCPServerThread, (void *)myTCPServer);
    ENSURE(myResult==0);

    // start client
    Unsigned32 inHostAddress = getHostAddress("localhost");

    TCPClientSocket myClient;
    myClient.setRemoteAddr(inHostAddress, serverPort);
    myClient.connect();
    if (!myClient.isValid())
        myClient.retry(10);
    ENSURE(myClient.isValid());

    char myInputBuffer[5];

    myClient.receive(myInputBuffer,5);
    ENSURE(strncmp(myInputBuffer,"READY",5)==0);
    myClient.send("QUIT!",5);
    myClient.close();
    ENSURE_EXCEPTION(myClient.send("hallo",5), SocketException);

    myClient.connect();
    myClient.receive(myInputBuffer,5);
    ENSURE(strncmp(myInputBuffer,"READY",5)==0);
    myClient.send("QUIT!",5);
    myClient.close();

    // wait for server thread to close
    int threadResult;
    pthread_join(myThread, (void **)&threadResult);
    ENSURE_MSG(threadResult==0, "Error in server thread");

    delete myTCPServer;
}

void * TestSocket::TCPServerThread(void *arg) {
    TCPServer * theServer = reinterpret_cast<TCPServer*>(arg);

    char myInputBuffer[5];
    long myResult = 0;

    for (int i = 0; i<2; i++) {
        AC_DEBUG <<"TCPServerThread waiting to connect" << endl;
        TCPSocket *mySocket = theServer->waitForConnection();
        AC_DEBUG <<"TCPServerThread connected" << endl;
        mySocket->send("READY",5);
        mySocket->receive(myInputBuffer, 5);
        if (strncmp(myInputBuffer, "QUIT!", 5)!=0) {
            myResult = -1;
        }
        delete mySocket;
        mySocket = 0;
    }

    pthread_exit((void *)myResult);

    return 0;
}

void TestSocket::UDPTest() {
    pthread_t myThread;
    int myResult;
    int serverPort;
    int clientPort;
    const int MIN_PORT = 8000;
    const int MAX_PORT = 8080;

    UDPSocket * myUDPServer=0;
    UDPSocket * myUDPClient=0;
    UDPConnection * myUDPConnection=0;

    // try to find a free server port between MIN_PORT and MAX_PORT;
    for (serverPort = MIN_PORT; serverPort <= MAX_PORT; serverPort++)
    {
        try {
            myUDPServer = new UDPSocket(INADDR_ANY, serverPort);
            break;
        }
        catch (SocketException & se)
        {
            cerr << "TestSocket::UDPTest() " << "failed to listen on port " << serverPort << ":" << se.where() << endl;
            myUDPServer = 0;
        }
    }
    ENSURE(myUDPServer != 0);
    cerr << "setting up UDP Server on port " << serverPort << "\n";

    // start server thread
    myResult = pthread_create(&myThread, NULL, UDPServerThread, (void *)myUDPServer);
    ENSURE(myResult==0);

    // try to find a free client port between serverPort+1 and MAX_PORT
    for (clientPort = serverPort+1; clientPort <= MAX_PORT; clientPort++)
    {
        try {
            myUDPClient = new UDPSocket(INADDR_ANY, clientPort);
            break;
        }
        catch (SocketException & se)
        {
            cerr << "TestSocket::UDPTest() " << "failed to listen on port " << clientPort << ":" << se.where() << endl;
            myUDPClient = 0;
        }
    }
    ENSURE(myUDPClient != 0);
    cerr << "setting up UDP Client on port " << clientPort << "\n";

    Unsigned32 inHostAddress = getHostAddress("localhost");

    char myInputBuffer[5];

    myUDPClient->sendTo(inHostAddress, serverPort, "READY",5);
    cerr << "Client sent data to " << serverPort << "\n";

    myUDPClient->receiveFrom(0,0,myInputBuffer,sizeof(myInputBuffer));
    cerr << "Client Received UDP packet\n";

    ENSURE(strncmp(myInputBuffer,"READY",5)==0);

    // stop client
    delete myUDPClient;
    myUDPClient = 0;

    // try to find a free client port between serverPort+1 and MAX_PORT
    for (clientPort = serverPort+1; clientPort <= MAX_PORT; clientPort++)
    {
        try {
            myUDPConnection = new UDPConnection(INADDR_ANY, clientPort);
            break;
        }
        catch (SocketException & se)
        {
            cerr << "TestSocket::UDPTest() " << "failed to listen on port " << clientPort << ":" << se.where() << endl;
            myUDPConnection = 0;
        }
    }

    ENSURE(myUDPConnection);

    myUDPConnection->connect(inHostAddress, serverPort);

    myUDPConnection->send("QUIT!",5);

    delete myUDPConnection;
    myUDPConnection = 0;

    // wait for server thread to close
    void * threadResult;
    pthread_join(myThread, &threadResult);
    ENSURE(threadResult==0);
}

void * TestSocket::UDPServerThread(void *arg) {

    UDPSocket * myUDPSocket = (UDPSocket *)arg;

    char myInputBuffer[5];
    unsigned short otherPort;
    Unsigned32 otherHost;

#ifdef OSX 
    // mac doesn't have loopback broadcast capability,
    // so use network broadcast and flood the lan
    Unsigned32 broadcastLoopback = getHostAddress("BROADCAST");
#else    
    Unsigned32 broadcastLoopback = getHostAddress("127.255.255.255");
#endif    


    do {
        myUDPSocket->receiveFrom(&otherHost, &otherPort, myInputBuffer, sizeof(myInputBuffer) );
        cerr << "UDP Server received UDP Packet " << string(myInputBuffer, sizeof(myInputBuffer)) << " from " << otherPort << ":" << otherHost << endl;
        if (strncmp(myInputBuffer,"QUIT!",5)==0) {
            delete myUDPSocket;
            myUDPSocket = 0;
        }
        else
        {
            cerr << "UDP Server broadcasting " << string(myInputBuffer, sizeof(myInputBuffer)) << endl;
            myUDPSocket->sendTo(broadcastLoopback, otherPort, myInputBuffer, sizeof(myInputBuffer) );
            cerr << "UDP Server broadcast ok on port " << otherPort << endl;
        }
    } while (myUDPSocket);

    pthread_exit(0);

    return 0;
}

void TestSocket::run() {
    testHostAddressMethods();
    BaseTest();
    TCPTest();
    UDPTest();
}
