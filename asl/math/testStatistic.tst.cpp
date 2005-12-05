/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testStatistic.tst.cpp,v $
//
//   $Revision: 1.1.1.1 $
//
// Description: unit test template file - change Lock to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Statistic.h"

#include <asl/UnitTest.h>
#include <asl/Time.h>

#include <math.h>

using namespace std; 

class StatisticUnitTest : public UnitTest {
    public:
        StatisticUnitTest() : UnitTest("StatisticUnitTest") {  }
        void run();
        void testStatisticBase();
        void testBoolStatistic();
        void testFrequencyStatistic();

    private:
};
 
void
StatisticUnitTest::testStatisticBase() {
    cerr << "Testing asl::Statistic" << endl;

    asl::Statistic myStatistic;

    // Test standard functions
    myStatistic.addSample(1.0);
    myStatistic.addSample(5.0);
    myStatistic.addSample(3.0);    
    myStatistic.addSample(3.0);    

    cerr << myStatistic << endl;
    ENSURE_MSG(myStatistic.getNumberOfSamples() == 4, "getNumberOfSamples == 4"); 
    ENSURE_MSG(myStatistic.getMaximum() == 5.0, "getMaximum == 5.0"); 
    ENSURE_MSG(myStatistic.getMinimum() == 1.0, "getMinimum == 1.0"); 
    ENSURE_MSG(myStatistic.getAverage() == 3.0, "getAverage == 3.0"); 
    ENSURE_MSG(myStatistic.getVariance() == 2.0, "getVariance == 2.0"); 
    //ENSURE_MSG(myStatistic.getStandardDeviation() == sqrt(2.0), "getStandardDev. == sqrt(2)");    

    // Test reset
    myStatistic.reset();
    myStatistic.addSample(1.0);

    cerr << myStatistic << endl;
    ENSURE_MSG(myStatistic.getNumberOfSamples() == 1, "getNumberOfSamples == 1"); 
    ENSURE_MSG(myStatistic.getMaximum() == 1.0, "getMaximum == 1.0"); 
    ENSURE_MSG(myStatistic.getMinimum() == 1.0, "getMinimum == 1.0"); 
    ENSURE_MSG(myStatistic.getAverage() == 1.0, "getAverage == 1.0"); 
    ENSURE_MSG(myStatistic.getVariance() == 0.0, "getVariance == 0.0"); 
    ENSURE_MSG(myStatistic.getStandardDeviation() == 0.0, "getStandardDev. == 0.0"); 

    // Test enable/disable
    myStatistic.disable();
    myStatistic.addSample(1.0);
    ENSURE_MSG(myStatistic.getNumberOfSamples() == 1, "disable statistic"); 

    myStatistic.enable();
    myStatistic.addSample(1.0);
    ENSURE_MSG(myStatistic.getNumberOfSamples() == 2, "enable statistic");     
 
    cerr << "Testing Statistic auto-output" << endl;
    asl::Statistic myStatistic2("Output after 2 samples", 2);
    myStatistic2.addSample(0);
    myStatistic2.addSample(1);
    myStatistic2.addSample(2);

    asl::Statistic myStatistic3("Output after 0.1 seconds", 0.1);
    myStatistic3.addSample(0);
    asl::msleep(60);    
    myStatistic3.addSample(1);
    asl::msleep(60);
    myStatistic3.addSample(2);
}

void
StatisticUnitTest::testBoolStatistic() {
    cerr << "Testing asl::BoolStatistic" << endl;

    // Test standard functions
    asl::BoolStatistic myStatistic;
    myStatistic.addSample(true);
    myStatistic.addSample(true);
    myStatistic.addSample(false);

    cerr << myStatistic << endl;
    ENSURE_MSG(myStatistic.getNumberOfSamples() == 3, "getNumberOfSamples == 3"); 
    ENSURE_MSG(myStatistic.getMaximum() == 1.0, "getMaximum == 1.0"); 
    ENSURE_MSG(myStatistic.getMinimum() == 0.0, "getMinimum == 0.0"); 
    ENSURE_MSG(myStatistic.getNumberOfHits() == 2, "getNumberOfHits == 2");

    cerr << "Testing BoolStatistic auto-output" << endl;
    asl::BoolStatistic myStatistic2("Output after 2 samples", 2);
    myStatistic2.addSample(false);
    myStatistic2.addSample(true);
    myStatistic2.addSample(false);

    asl::BoolStatistic myStatistic3("Output after 0.1 seconds", 0.1);
    myStatistic3.addSample(true);
    asl::msleep(60);    
    myStatistic3.addSample(false);
    asl::msleep(60);
    myStatistic3.addSample(true);
}

void
StatisticUnitTest::testFrequencyStatistic() {
    cerr << "Testing asl::FrequencyStatistic" << endl;

    asl::FrequencyStatistic myStatistic;
    myStatistic.addSample();
    asl::msleep(100);
    myStatistic.addSample();
    asl::msleep(100);
    myStatistic.addSample();

    ENSURE_MSG(myStatistic.getNumberOfSamples() == 2, "getNumberOfSamples == 2"); 
    ENSURE_MSG(myStatistic.getMaximum() > 7 , "getMaximum > 7"); 
    ENSURE_MSG(myStatistic.getMaximum() < 13 , "getMaximum < 13");    
    ENSURE_MSG(myStatistic.getMinimum() > 7, "getMinimum > 7"); 
    ENSURE_MSG(myStatistic.getMinimum() < 13, "getMinimum < 13"); 

    ENSURE_MSG(myStatistic.getAverage() > 7, "getAverage > 7"); 
    ENSURE_MSG(myStatistic.getAverage() < 13, "getAverage < 13");     

    cerr << myStatistic << endl;
    
    myStatistic.reset();
    myStatistic.addSample();
    asl::msleep(50);
    myStatistic.addSample();
    ENSURE_MSG(myStatistic.getMaximum() > 15, "reset (maximum > 15)");
    ENSURE_MSG(myStatistic.getMinimum() > 15, "reset (minimum > 15)");    

    cerr << myStatistic << endl;

    // Test auto-output
    asl::FrequencyStatistic myStatistic2("Output after 2 samples", 2);
    myStatistic2.addSample();
    asl::msleep(50);
    myStatistic2.addSample();
    asl::msleep(50);
    myStatistic2.addSample();

    asl::FrequencyStatistic myStatistic3("Output after 0.1 seconds", 0.1);
    myStatistic3.addSample();
    asl::msleep(60);    
    myStatistic3.addSample();
    asl::msleep(60);
    myStatistic3.addSample();
}

void
StatisticUnitTest::run() {
    testStatisticBase();
    testBoolStatistic();
    testFrequencyStatistic();
}

int main(int argc, char *argv[]) {

    StatisticUnitTest myTest;

    myTest.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << myTest.returnStatus() << endl;

    return myTest.returnStatus();

}
