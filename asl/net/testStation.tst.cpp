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
//    recommendations: add comments 
*/

#include "Station.h"
#include "net.h"
#include "net_functions.h"

#include <asl/base/UnitTest.h>
#include <asl/base/string_functions.h>
#include <asl/base/os_functions.h>
#include <asl/base/Time.h>
#include <asl/base/Logger.h>

#include <string>
#include <iostream>

using namespace std;
using namespace asl;

class StationTest : public TemplateUnitTest {
    public:
        StationTest() : TemplateUnitTest("StationTest-",asl::as_string(1).c_str()) {  }
        void run() {

            inet::initSockets();

            try {

                Station myStation;

                ENSURE(!myStation);
                asl::Unsigned16 myTestPort = Station::defaultBroadcastPort()+1;
                get_environment_var_as("AC_TEST_PORT_START", myTestPort);
                AC_PRINT << "Using port " << myTestPort << " for tests";

                //myStation.openStationDefault(Station::defaultBroadcastAddress(),
                myStation.openStationDefault(asl::hostaddress("127.255.255.255"),
                                      myTestPort,
                                      myTestPort,
#ifdef IGNORE_OWN_PACKETS
                                      Station::defaultOwnIPAddress(),
#else
                                      0,
#endif
                                      Station::defaultOwnIPAddress());
                ENSURE(myStation);
                DPRINT(myTestPort);

                asl::Block testData;

                for (int i = 0; i < 2000; ++i) {
                //for (int i = 0; i < 500; ++i) {
                    testData.appendString(asl::as_string(i));

                }
                cerr << "Testdata size = " << testData.size() << ":" << endl;
                // cerr << testData << endl;
#ifdef DEBUG_VARIANT
                const int testCount = 30;
#else
                const int testCount = 10000;
                setSilentSuccess();
#endif
                asl::Time start;
                asl::Time lastTime;
                for (int i = 0; i < testCount; ++i) {
//#define WITH_STATISTIC
#ifdef WITH_STATISTIC
                    Station::Statistic myReceiveStat = myStation.getReceiveStatistic();
                    Station::Statistic myTransmitStat = myStation.getTransmitStatistic();
                    Station::Statistic myErrorStat = myStation.getReceiveErrorStatistic();
#endif

                    myStation.broadcast(testData);
                    asl::Block replyData;
                    unsigned long someSender;
                    unsigned long itsStationID;
                    while (myStation.receive(replyData, someSender, itsStationID)) {
                        ENSURE(replyData == testData);
                        //ENSURE(someSender == 0x01020304);
                        ENSURE(someSender == 0x0);
                        //DPRINT(asl::as_dotted_address(someSender) << ":" << itsStationID);
                    }
                    //usleep(30000);
#ifdef WITH_STATISTIC
                    asl::Time nowTime;

                    cerr << "Overall Received           :" << myStation.getReceiveStatistic() << endl;
                    cerr << "Overall Transmitted        :" << myStation.getTransmitStatistic() << endl;
                    cerr << "Overall Dropped            :" << myStation.getReceiveErrorStatistic() << endl;

                    double timeTakenOverall = nowTime - start;
                    cerr << "Overall Received/sec       :" << myStation.getReceiveStatistic()/timeTakenOverall << endl;
                    cerr << "Overall Transmitted/sec    :" << myStation.getTransmitStatistic()/timeTakenOverall << endl;
                    cerr << "Overall Current Dropped/sec:" << myStation.getReceiveErrorStatistic()/timeTakenOverall << endl;

                    double timeTaken = nowTime - lastTime;
                    cerr << "Current Received/sec       :" << (myStation.getReceiveStatistic()-myReceiveStat)/timeTaken<< endl;
                    cerr << "Current Transmitted/sec    :" << (myStation.getTransmitStatistic()-myTransmitStat)/timeTaken<< endl;
                    cerr << "Current Dropped/sec        :" << (myStation.getReceiveErrorStatistic()-myErrorStat)/timeTaken<< endl;

                    lastTime = nowTime;
#endif
                }
                asl::Time ready;
                double takenTime = ready-start;
                cerr << "Time = " << takenTime << " secs, throughput = " << testData.size()*testCount/takenTime/1024/1024 << " MB/sec" << endl;
            }
            catch (asl::Exception & ex) {
                cerr << ex << endl;
                FAILURE("Exception");
            }

            inet::terminateSockets();

        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message

        addTest(new StationTest);

    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
