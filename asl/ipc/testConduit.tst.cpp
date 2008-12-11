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
//     Classes for networked or local communication between processes
//
// Last Review:  ms 2007-08-15
//
//  review status report: (perfect, ok, fair, poor, disaster, notapp (not applicable))
//    usefulness              :      ok
//    formatting              :      fair
//    documentation           :      ok
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
//    recommendations: add high-level documentation, improve doxygen documentation 
*/
#include "ConduitServer.h"

#include <algorithm>
#include "ConduitAcceptor.h"
#include "TCPPolicy.h"
#ifndef WIN32
#   include "LocalPolicy.h"
#endif
#include "MessageAcceptor.h"

#include <asl/base/os_functions.h>
#include <asl/base/string_functions.h>
#include <asl/base/file_functions.h>

#include <asl/base/UnitTest.h>
#include <asl/net/net.h>
#include <asl/base/Time.h>

#include <vector>
#include <string>
#include <iostream>
#include <cctype>
#include <signal.h>

using namespace asl;
using namespace std;

//
// As simple server which echos back strings
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
                return true;
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
            asl::Ptr<typename MessageAcceptor<POLICY>::Message> myMessage;
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

template<class POLICY>
class BrokenPipeTest : public TemplateUnitTest {
    public:
        typedef asl::Ptr<typename asl::ConduitAcceptor<POLICY> > AcceptorPtr;
        BrokenPipeTest(const char * theTemplateArgument, typename POLICY::Endpoint theEndpoint) 
            : TemplateUnitTest("BrokenPipeTest", theTemplateArgument),
            _myLocalEndpoint(theEndpoint)
            {  }

        void run() {
            std::string myBigString;
            for (int i = 0; i<10000; ++i) {
                myBigString += as_string(i);
            }
            AC_PRINT << "the following might cause 'broken pipe' warnings - this is normal";
            setSilentSuccess(true);
#ifdef WIN32
            int myIterations = 10; // aborting is VERY slow in windows.
#else
            int myIterations = 100; // don't increase this or the socket table will fill up with TIME_WAIT sockets, waiting to die
#endif            
            for (int i = 0; i < myIterations; ++i) {
                // start server thread
                AcceptorPtr myAcceptor = AcceptorPtr(new ConduitAcceptor<POLICY>(_myLocalEndpoint, 
                        myLowercaseServer<POLICY>::create));
                ENSURE(myAcceptor->start());
                // start client
                {
                    CharBuffer myInputBuffer;

                    typename Conduit<POLICY>::Ptr myClient(new Conduit<POLICY>(_myLocalEndpoint));
                    ENSURE(myClient);
                    ENSURE(myClient->sendData(myBigString.c_str(), myBigString.length()));
                    /*
                    string myReceiveString;
                    while (myReceiveString.length() < myBigString.length()  && myClient->isValid()) {
                        if (myClient->receiveData(myInputBuffer)) {
                            myReceiveString += string(&myInputBuffer[0], myInputBuffer.size());
                        }
                    }
                    ENSURE(myReceiveString == myBigString);
                    */
                }
                // close the server (also test if the server is cancelable)
                ENSURE_MSG(myAcceptor->stop(), "Cancelling server thread");
                myAcceptor = AcceptorPtr(0);
            }
            setSilentSuccess(false);
        }
     private:
        typename POLICY::Endpoint _myLocalEndpoint;
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

        void run() {
            // start server thread
            _myAcceptor = AcceptorPtr(new ConduitAcceptor<POLICY>(_myLocalEndpoint, 
                        myLowercaseServer<POLICY>::create));
            ENSURE(_myAcceptor->start());
            
             // XXX uncomment this to play with telnet
             //while (true);

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
            // now start two clients simultaneously
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
            // check already-is-use behaviour
            {
                AcceptorPtr myAcceptor;
                ENSURE_EXCEPTION(myAcceptor = AcceptorPtr(new ConduitAcceptor<POLICY>(_myLocalEndpoint, 
                        myLowercaseServer<POLICY>::create)), ConduitInUseException);
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
        asl::Unsigned16 myTestPort = 5678;
        get_environment_var_as("AC_TEST_PORT_START", myTestPort);
        AC_PRINT << "Using port " << myTestPort << " for tests";

        UnitTestSuite::setup(); // called to print a launch message
        addTest(new BrokenPipeTest<TCPPolicy>("TCPPolicy", 
                    TCPPolicy::Endpoint("127.0.0.1",myTestPort)));
        addTest(new ConduitTest<TCPPolicy>("TCPPolicy", 
                    TCPPolicy::Endpoint("127.0.0.1",myTestPort)));
        addTest(new MessageConduitTest<TCPPolicy>("TCPPolicy", 
                    TCPPolicy::Endpoint("127.0.0.1",myTestPort)));
#ifndef WIN32
        deleteFile(UnixAddress::PIPE_PREFIX + "TestConduit");
        addTest(new ConduitTest<LocalPolicy>("LocalPolicy", 
                    "TestConduit"));
        addTest(new MessageConduitTest<LocalPolicy>("LocalPolicy", 
                    "TestConduit"));
        addTest(new BrokenPipeTest<LocalPolicy>("LocalPolicy", "TestConduit"));
#endif   
    }
};


int main(int argc, char *argv[]) {
#ifdef LINUX
    // enable coredump in case a SIGPIPE gets through (shouldn't happen!)
    sighandler_t myReturnCode = signal( SIGPIPE , (void (*) (int)) &abort );
#endif

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();
    std::cerr << ">> Finished test suite '" << argv[0] << "'"
              << ", return status = " << mySuite.returnStatus() << std::endl;

    return mySuite.returnStatus();

}
