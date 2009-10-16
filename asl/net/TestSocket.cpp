/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: 
//    C++ Library fuer TCP-Sockets (based on Sockets.c++ from Pavel 11.9.92)
//
// Last Review:  ms & ab 2007-08-14
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      ok
//    documentation           :      fair
//    test coverage           :      ok
//    names                   :      ok
//    style guide conformance :      ok
//    technological soundness :      ok
//    dead code               :      ok
//    readability             :      ok
//    understandability       :      ok
//    interfaces              :      ok
//    confidence              :      ok
//    integration             :      ok
//    dependencies            :      ok
//    error handling          :      ok
//    logging                 :      notapp
//    cheesyness              :      ok
//
//    overall review status   :      ok
//
//    recommendations: move to inner class of *.tst.cpp file 
*/

//own header
#include "TestSocket.h"

#include "Socket.h"
#include "TCPClientSocket.h"
#include "UDPSocket.h"
#include "UDPConnection.h"
#include <asl/base/os_functions.h>
#include <asl/base/Logger.h>

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
    SocketErrorCode myErrCode;

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
    catch (SocketError & e) {
        cerr << e.what() << endl;
        myErrCode = e.getErrorCode();
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
    catch (SocketError & e) {
        cerr << e.what() << endl;
        myErrCode = e.getErrorCode();
    }
    ENSURE (myErrCode == HOST_NOT_FOUND ||
            myErrCode == NO_DATA);

    ENSURE(addr2Str(inHostAddress) == string(inHostDotName));

    INetEndpoint myEndpoint("localhost",3000);
    DPRINT(myEndpoint);
}

void TestSocket::BaseTest() {
    Socket theSocket(INADDR_ANY, 80);
    ENSURE_EXCEPTION(theSocket.peek(10), SocketException);
    char myData[32];
    ENSURE_EXCEPTION(theSocket.receive(myData, 32), SocketException);
    ENSURE_EXCEPTION(theSocket.send("Hallo",5), SocketException);
}

TCPServer * TestSocket::createTCPServer(){

    TCPServer * myTCPServer = NULL;
    asl::Unsigned16 serverPort;
    const asl::Unsigned16 MIN_PORT = 8000;
    const asl::Unsigned16 MAX_PORT = 8080;

    // TODO: linux test (not that easy, socket to be reused must be in state TIME_WAIT
    // see http://www.unixguide.net/network/socketfaq/4.5.shtml for more info
#ifdef _WIN32    
    // try to create a tcp socket twice (already in use test)
    // since windows 2003 server it's neccessary to open both
    // sockets with SO_REUSEADDR, not only the second one.
    // See http://msdn.microsoft.com/en-us/library/ms740621(VS.85).aspx
    // section: Enhanced Socket Security
    try {
        myTCPServer = new TCPServer(INADDR_ANY, MIN_PORT, true);
    }
    catch (SocketException & se)
    {
        cerr << "TestSocket::TCPTest() " << "failed to listen on port " << MIN_PORT << ":" << se << endl;
        myTCPServer = 0;
    }
    // do it again
    try {
        myTCPServer = new TCPServer(INADDR_ANY, MIN_PORT, true);
    }
    catch (SocketException & se)
    {
        cerr << "TestSocket::TCPTest() " << "failed to listen on port " << MIN_PORT << ":" << se << endl;
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

    return myTCPServer;
}

void TestSocket::TCPTest() {

    TCPServer * myTCPServer = createTCPServer();

    pthread_t myThread;
    int myResult;

    // start server thread
    myResult = pthread_create(&myThread, NULL, TCPServerThread, (void *)myTCPServer);
    ENSURE(myResult==0);

    // start client
    Unsigned32 inHostAddress = getHostAddress("localhost");

    TCPClientSocket myClient;
    myClient.setRemoteAddr(inHostAddress, myTCPServer->getPort());
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

    // start another server thread
    myResult = pthread_create(&myThread, NULL, TCPServerThread, (void *)myTCPServer);
    ENSURE(myResult==0);

    myClient.connect();
    myInputBuffer[3] = ' '; // invalidate buffer
    myClient.receive(myInputBuffer,5);
    ENSURE(strncmp(myInputBuffer,"READY",5)==0);
    myClient.send("QUIT!",5);
    myClient.close();

    // wait for server thread to close
    int threadResult;
    pthread_join(myThread, (void **)&threadResult);
    ENSURE_MSG(threadResult==0, "Error in server thread");
}

void * TestSocket::TCPServerThread(void *arg) {
    TCPServer * theServer = reinterpret_cast<TCPServer*>(arg);

    char myInputBuffer[5];
    long myResult = 0;

    AC_DEBUG <<"TCPServerThread waiting to connect" << endl;
    TCPSocket *mySocket = theServer->waitForConnection();
    AC_DEBUG <<"TCPServerThread connected" << endl;
    mySocket->send("READY",5);

    do {
        mySocket->receive(myInputBuffer, 5);
        AC_DEBUG << "TCPServerThread received " << myInputBuffer;
        if (strncmp(myInputBuffer, "BREAK",5)==0){
            AC_DEBUG << "TCPServerThread breaking the connection";
            pthread_exit((void *)myResult);
            return (void*)myResult;
        }

    } while (strncmp(myInputBuffer, "QUIT!", 5)!=0);

    AC_DEBUG << "TCPServerThread received quit";
    delete mySocket;
    mySocket = 0;

    pthread_exit((void *)myResult);

    return 0;
}

void TestSocket::UDPTest() {
    pthread_t myThread;
    int myResult;
    asl::Unsigned16 serverPort;
    asl::Unsigned16 clientPort;
    const asl::Unsigned16 MIN_PORT = 8000;
    const asl::Unsigned16 MAX_PORT = 8080;

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


void TestSocket::testConnectionTimeout(){

    TCPServer * myTCPServer = createTCPServer();

    pthread_t myThread;
    int myResult;

    // start server thread
    myResult = pthread_create(&myThread, NULL, TCPServerThread, (void *)myTCPServer);
    ENSURE(myResult==0);

    // start client
    Unsigned32 inHostAddress = getHostAddress("localhost");

    TCPClientSocket myClient;
    myClient.setRemoteAddr(inHostAddress, myTCPServer->getPort());

    myClient.setConnectionTimeout(1);
    myClient.connect();

    char myInputBuffer[5];
    myClient.receive(myInputBuffer,5);
    ENSURE(strncmp(myInputBuffer,"READY",5)==0);

    // break the server thread 
    myClient.send("BREAK",5);
    int threadResult;
    pthread_join(myThread, (void **)&threadResult);
    ENSURE_MSG(threadResult==0, "Error in server thread");

    ENSURE(myClient.getConnectionTimeout() == 1);

    ENSURE_EXCEPTION(myClient.receive(myInputBuffer,5), SocketError);

    myClient.close();

    delete myTCPServer;
}

void TestSocket::run() {
    testHostAddressMethods();
    BaseTest();
    TCPTest();
    UDPTest();
    testConnectionTimeout();
}
