//=============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: testWMADecoder.tst.cpp,v $
//     $Author: christian $
//   $Revision: 1.1 $
//       $Date: 2005/04/08 18:38:36 $
//
//  WMV audio reader.
//
//=============================================================================

#include <audio/AudioController.h>
#include <y60/IDecoder.h>
#include <asl/PlugInManager.h>
#include <y60/DecoderManager.h>

#include <asl/UnitTest.h>
#include <asl/Time.h>

using namespace AudioApp;
using namespace std;
using namespace asl;
using namespace y60;

class WMADecoderTest : public UnitTest {
    public:
        WMADecoderTest() : UnitTest("WMADecoderTest") {  }

        void play(const std::string & theFilename, unsigned int theTargetRate = 44100) {
            AudioController & myController = AudioController::get();
            myController.init(theTargetRate);

            std::string myId = myController.play(theFilename, "Mixer", 1.0, 0.0, false);
            ENSURE(myId != "");
            cerr << "playing id=" << myId << endl;
            asl::msleep(3000);

            cerr << "pause id=" << myId << endl;
            myController.pause(myId);
            asl::msleep(1000);

            cerr << "unpause id=" << myId << endl;
            myController.pause(myId);
            asl::msleep(3000);

            cerr << "stop id=" << myId << endl;
            myController.stop(myId);
            cerr << "stopped id=" << myId << endl;
            asl::msleep(1000);

            SUCCESS("WMADecoder");
        }

        void run() {
            PlugInManager::get().setSearchPath("${PRO}/lib");
            PlugInBasePtr myPlugIn = PlugInManager::get().getPlugIn("y60WMADecoder");
            if (IDecoderPtr myDecoder = dynamic_cast_Ptr<IDecoder>(myPlugIn)) {
                cerr << "Plug y60WMADecoder" << endl;
                DecoderManager::get().addDecoder(myDecoder);
            }

            play("../../testfiles/music_cut_wm9.wma", 44100);
            play("http://himmel/testfiles/Leben.wma", 20000);
            play("http://himmel/testfiles/track2.mp3");
            play("http://himmel/testfiles/helsing.wma"); // Van Helsing - the soundtrack
            SUCCESS("WMADecoder");
        }
};


class MyTestSuite : public UnitTestSuite {
    public:
        MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
        void setup() {
            UnitTestSuite::setup();
            addTest(new WMADecoderTest);
        }
};

int main(int argc, char *argv[])
{
    MyTestSuite mySuite(argv[0]);

    try {
        mySuite.run();
    } catch (const asl::Exception & ex) {
        cerr << "Exception during test execution: " << ex << endl;
        return -1;
    }

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}
