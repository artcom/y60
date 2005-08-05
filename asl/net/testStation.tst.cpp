/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testStation.tst.cpp,v $
//
//   $Revision: 1.7 $
//
// Description: unit test for MultiThreadUnitTest classes
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Station.h"
#include "net.h"
#include "net_functions.h"

#include <asl/UnitTest.h>
#include <asl/string_functions.h>
#include <asl/Time.h>

#include <string>
#include <iostream>

using namespace std;
using namespace asl;

class StationTest : public TemplateUnitTest {
    public:
        StationTest() : TemplateUnitTest("StationTest-",asl::as_string(1).c_str()) {  }
        void run() {

            try {
                inet::initSockets();

                Station myStation;

                ENSURE(!myStation);

                myStation.openStationDefault(Station::defaultBroadcastAddress(),
                                      Station::defaultBroadcastPort()+1,
                                      Station::defaultBroadcastPort()+1,
#ifdef IGNORE_OWN_PACKETS
                                      Station::defaultOwnIPAddress(),
#else
                                      0,
#endif
                                      Station::defaultOwnIPAddress());
                ENSURE(myStation);

                asl::Block testData;

                for (int i = 0; i < 2000; ++i) {
                //for (int i = 0; i < 500; ++i) {
                    testData.appendString(asl::as_string(i));
                }
                cerr << "Testdata size = " << testData.size() << ":" << endl;
                cerr << testData << endl;
#ifdef DEBUG_VARIANT
                const int testCount = 30;
#else
                const int testCount = 10000;
                setSilentSuccess();
#endif
                asl::Time start;
                asl::Time lastTime;
                for (int i = 0; i < testCount; ++i) {
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
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message

        addTest(new StationTest);

    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
