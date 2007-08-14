/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2007, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
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
#include "TestINetMonitor.h"
#include "INetMonitor.h"
#include "TCPServer.h"
#include "SocketException.h"

#include <asl/Exception.h>
#include <asl/Time.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include <vector>
#include <string>
#include <stdio.h>

using namespace std;
using namespace asl;

//#define TEST_WITH_INTERNET
using namespace inet;

void* TestINetMonitor::startDummyServer(void *arg) {

    char myInputBuffer[1024];
    TCPServer * myTCPServer = (TCPServer *)arg;

    const char * myResponse="HTTP/1.1 200 OK\x0D\x0AContent-Type: text/plain\x0D\x0AContent-Length: 4\x0D\x0A\x0D\x0AOK\x0D\x0A";

    try {
        inet::TCPSocket *mySocket = myTCPServer->waitForConnection();

        // mySocket->write("READY",5);

        //while (mySocket) {
        mySocket->receive(myInputBuffer, sizeof(myInputBuffer));
        cerr << myInputBuffer << endl;
        mySocket->send(myResponse, strlen(myResponse));
        msleep(1000);
        delete mySocket;
        mySocket = 0;
        //}
    } catch (inet::SocketException & se) {
        cerr << "Socket Exception: " << se.where() << "endl";
    }
    pthread_exit(0);
    return 0;
}

void
TestINetMonitor::run() {

    int myServerPort = 2000;
    int serverPort;
    const int MIN_PORT = 8000;
    const int MAX_PORT = 8080;
    TCPServer * myTCPServer;

    // start dummy webserver
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
    pthread_t myThread;
    int myResult = pthread_create(&myThread, NULL, startDummyServer, myTCPServer);
    ENSURE(myResult==0);


    vector<string> localHostList;
    char myURL[256];

    sprintf(myURL,"http://localhost:%d",serverPort);
    localHostList.push_back(string(myURL));

    inet::INetMonitor::setHostList(localHostList);
    msleep(1000);
    ENSURE(inet::INetMonitor::isInternetAvailable());

    // wait for server thread to close
    void * threadResult;
    pthread_join(myThread, &threadResult);
    delete myTCPServer;

    msleep(1000);
    localHostList.clear();
    inet::INetMonitor::setHostList(localHostList);
    msleep(2000);
    ENSURE(!inet::INetMonitor::isInternetAvailable());


#ifdef TEST_WITH_INTERNET
    vector<string> realHostList;
    realHostList.push_back("http://www.allianz.de");
    realHostList.push_back("http://www.whitehouse.gov");
    realHostList.push_back("http://www.microsoft.com");

    inet::INetMonitor::setHostList(realHostList);
    sleep(8);
    ENSURE(inet::INetMonitor::isInternetAvailable());
#endif

    // Disable INetMonitor so we don't disturb other tests.
    vector<string> nullHostList;
    inet::INetMonitor::setHostList(nullHostList);

}


