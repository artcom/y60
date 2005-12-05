//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: testConduit.tst.cpp,v $
//   $Author: martin $
//   $Revision: 1.20 $
//   $Date: 2005/04/26 15:52:45 $
//
//  Description: Collects statistics about the render state
//
//=============================================================================

#include "ConduitServer.h"

#include <algorithm>
#include "ConduitAcceptor.h"
#include "TCPPolicy.h"
#include "LocalPolicy.h"
#include "MessageAcceptor.h"

#include <asl/os_functions.h>
#include <asl/string_functions.h>
#include <asl/file_functions.h>

#include <asl/UnitTest.h>
#include <asl/net.h>
#include <asl/Time.h>

#include <vector>
#include <string>
#include <iostream>
#include <cctype>

//using namespace std;
using namespace inet;
using namespace asl;
using namespace std;

//
// As simple server which echos back 5-byte strings
// converted to lower case. 
//
// All we need is a constructor, a static factory method
// and a processData() method.
//
// Pass the factory method to the ConduitAcceptor 
// and it will then spawn your server class for each
// incoming connection.
//
template <class POLICY>
class myLowercaseServer : public ConduitServer<POLICY> {
    public:
        static typename ConduitServer<POLICY>::Ptr 
            create(typename POLICY::Handle theHandle) 
        {
            return typename ConduitServer<POLICY>::Ptr(new myLowercaseServer(theHandle));
        }
        myLowercaseServer(typename POLICY::Handle theHandle) :
           ConduitServer<POLICY>(theHandle) {}; 

        virtual bool processData() {
            CharBuffer myInputBuffer;
            if (this->receiveData(myInputBuffer)) {
                std::string s(&myInputBuffer[0], myInputBuffer.size());
                transform(s.begin(), s.end(), s.begin(), ::tolower);
                this->sendData(s.c_str(),s.length());
            }
            return true;
        }
};


template <class POLICY>
class MessageConduitTest : public TemplateUnitTest {
    public:
        typedef asl::Ptr<typename asl::MessageAcceptor<POLICY> > AcceptorPtr;
        MessageConduitTest(const char * theTemplateArgument, typename POLICY::Endpoint theEndpoint) 
            : TemplateUnitTest("MessageConduitTest", theTemplateArgument),
            _myLocalEndpoint(theEndpoint), 
            _myAcceptor(0)  
            {  }

        void setup() {
            UnitTest::setup();
        }
        void run() {
            // start server thread
            _myAcceptor = AcceptorPtr(new MessageAcceptor<POLICY>(_myLocalEndpoint, MessageServer<POLICY>::create));
            ENSURE(_myAcceptor->start());
            msleep(100);
            // start client
            typename Conduit<POLICY>::Ptr myClient(new Conduit<POLICY>(_myLocalEndpoint));
            ENSURE(myClient);
            // send a message
            string myOutputString("Hello\0World!");
            myClient->send(myOutputString);
            
			// did the message get to the server?
            Ptr<typename MessageAcceptor<POLICY>::Message> myMessage;
            while (!(myMessage = _myAcceptor->popIncomingMessage())) {
                msleep(100);
            }
			ENSURE(myMessage);
			ENSURE(myMessage->as_string() == "Hello\0World!");
			// send a return message
            myOutputString = "You Too!";
			_myAcceptor->pushOutgoingMessage(myMessage->server, myOutputString);
            // did the message get back?
            string myInputString;
            while (!myClient->receive(myInputString)) {
                msleep(100);
            }
            ENSURE(myInputString == myOutputString);
            _myAcceptor = AcceptorPtr(0);
        }
     private:
        typename POLICY::Endpoint _myLocalEndpoint;
        AcceptorPtr _myAcceptor;
};

template <class POLICY>
class ConduitTest : public TemplateUnitTest {
    public:
        typedef asl::Ptr<typename asl::ConduitAcceptor<POLICY> > AcceptorPtr;
        ConduitTest(const char * theTemplateArgument, typename POLICY::Endpoint theEndpoint) 
            : TemplateUnitTest("ConduitTest", theTemplateArgument),
            _myLocalEndpoint(theEndpoint),
            _myAcceptor(0)  
            {  }

        void setup() {
            UnitTest::setup();
        }
        void run() {
            // start server thread
            _myAcceptor = AcceptorPtr(new ConduitAcceptor<POLICY>(_myLocalEndpoint, 
                        myLowercaseServer<POLICY>::create));
            ENSURE(_myAcceptor->start());
            
            // start client
            {
                CharBuffer myInputBuffer;
                typename Conduit<POLICY>::Ptr myClient(new Conduit<POLICY>(_myLocalEndpoint));
                ENSURE(myClient);
                setSilentSuccess(true);
                for (int i = 0; i < 20; ++i) {
                    myClient->sendData("HELLO",5);
                    while (!myClient->receiveData(myInputBuffer)) {
                        myClient->handleIO(10);
                    }
                    ENSURE(myInputBuffer.size()==5);
                    ENSURE(strncmp(&myInputBuffer[0],"hello",5)==0);
                }
                setSilentSuccess(false);
                // now send something, but don't pick up the reply
                myClient->sendData("GOOD BYE!",5);
            }
            msleep(100);
            // Stress Test
            setSilentSuccess(true);
            for (int i=0; i < 20; ++i) {
                CharBuffer myInputBuffer(5,0);
                typename Conduit<POLICY>::Ptr myClient(new Conduit<POLICY>(_myLocalEndpoint));
                ENSURE(myClient);
                //msleep(100); 
                string myOutput("Q");
                myOutput += as_string(i);

                ENSURE(myClient->sendData(myOutput.c_str(), myOutput.length()));
                while (!myClient->receiveData(myInputBuffer)) {
                    myClient->handleIO(10);
                }
                //cerr << "sent '" << myOutput << "', recv '" << &(myInputBuffer[0]) << "'" << endl;
                ENSURE(myInputBuffer.size()==myOutput.length());
                string myExpectedReply("q");
                myExpectedReply += as_string(i);
                ENSURE_MSG(strncmp(&myInputBuffer[0],myExpectedReply.c_str(),myInputBuffer.size())==0, " iterations.");
            }
            setSilentSuccess(false);
            // now start two clients simutaneously
            {
                CharBuffer myInputBuffer1;
                typename Conduit<POLICY>::Ptr myClient1(new Conduit<POLICY>(_myLocalEndpoint));
                ENSURE(myClient1);
                CharBuffer myInputBuffer2;
                typename Conduit<POLICY>::Ptr myClient2(new Conduit<POLICY>(_myLocalEndpoint));
                ENSURE(myClient2);
                myClient2->sendData("WORLD",5);
                myClient1->sendData("HELLO",5);
                bool myWaiting1 = true;
                bool myWaiting2 = true;
                while (myWaiting1 || myWaiting2) {
                    if (myWaiting1 && myClient1->receiveData(myInputBuffer1)) {
                        myWaiting1 = false;
                    };
                    if (myWaiting2 && myClient2->receiveData(myInputBuffer2)) {
                        myWaiting2 = false;
                    };
                }
                ENSURE(strncmp(&myInputBuffer1[0],"hello",5)==0);
                ENSURE(strncmp(&myInputBuffer2[0],"world",5)==0);
            }
            // how about a really big string
            {
                std::string myBigString;
                for (int i = 0; i<10000; ++i) {
                    myBigString += as_string(i);
                }
                CharBuffer myInputBuffer;
                
                typename Conduit<POLICY>::Ptr myClient(new Conduit<POLICY>(_myLocalEndpoint));
                ENSURE(myClient);
                ENSURE(myClient->sendData(myBigString.c_str(), myBigString.length()));
                string myReceiveString;
                while (myReceiveString.length() < myBigString.length()  && myClient->isValid()) {
                    if (myClient->receiveData(myInputBuffer)) {
                        myReceiveString += string(&myInputBuffer[0], myInputBuffer.size());
                    }
                }
                ENSURE(myReceiveString == myBigString);
            }
			
            // close the server (also test if the server is cancelable)
            msleep(100);
            ENSURE_MSG(_myAcceptor->stop(), "Cancelling server thread");
            msleep(100);
            _myAcceptor = AcceptorPtr(0);
        }

     private:
        typename POLICY::Endpoint _myLocalEndpoint;
        AcceptorPtr _myAcceptor;
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        inet::initSockets();
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new ConduitTest<TCPPolicy>("TCPPolicy", 
                    TCPPolicy::Endpoint("127.0.0.1",5678)));
#ifdef LINUX
        addTest(new ConduitTest<LocalPolicy>("LocalPolicy", 
                    "TestConduit"));

        deleteFile(UnixAddress::PIPE_PREFIX + "TestConduit");
        addTest(new MessageConduitTest<LocalPolicy>("LocalPolicy", 
                    "TestConduit"));
#endif        
        addTest(new MessageConduitTest<TCPPolicy>("TCPPolicy", 
                    TCPPolicy::Endpoint("127.0.0.1",5678)));

    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();
    std::cerr << ">> Finished test suite '" << argv[0] << "'"
              << ", return status = " << mySuite.returnStatus() << std::endl;

    return mySuite.returnStatus();

}
