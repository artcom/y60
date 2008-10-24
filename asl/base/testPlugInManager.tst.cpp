
#include <string>

#include <asl/base/UnitTest.h>

#include <asl/base/PlugInManager.h>

#include "testPlugIn.h"

using namespace std;
using namespace asl;

const string ourPlugInName = "asltestplugin";

class PlugInManagerTest : public UnitTest {
public:
    PlugInManagerTest()
        : UnitTest("PluginManagerTest") {}

    void
    run() {
        PlugInManager& myManager = PlugInManager::get();

        ENSURE(!myManager.isLoaded(ourPlugInName));

        {
            DTITLE("Loading PlugIn " << ourPlugInName << " for the first time");

            _myTouchIndicator = "untouched";

            Ptr<PlugInBase> myPlugIn = myManager.getPlugIn(ourPlugInName);
            // ensure we actually got something
            ENSURE(&(*myPlugIn));
            // ensure the manager considers the plugin loaded
            ENSURE(myManager.isLoaded(ourPlugInName));

            // cast the plugin to the expected class
            Ptr<ITestPlugIn> myTestPlugIn = dynamic_cast_Ptr<ITestPlugIn>(myPlugIn);
            // ensure that it was of the expected class
            ENSURE(&(*myPlugIn));
            // make the plugin change the indicator
            myTestPlugIn->touchThatString(_myTouchIndicator);
            // check the indicator
            ENSURE(_myTouchIndicator == "touched");
        }

        ENSURE(!myManager.isLoaded(ourPlugInName));

        {
            DTITLE("Loading PlugIn " << ourPlugInName << " for the second time");

            _myTouchIndicator = "untouched";

	        asl::Ptr<PlugInBase> myPlugIn = myManager.getPlugIn(ourPlugInName);
            // ensure we actually got something
            ENSURE(&(*myPlugIn));
            // ensure the manager considers the plugin loaded
            ENSURE(myManager.isLoaded(ourPlugInName));

            // cast the plugin to the expected class
            Ptr<ITestPlugIn> myTestPlugIn = dynamic_cast_Ptr<ITestPlugIn>(myPlugIn);
            // ensure that it was of the expected class
            ENSURE(&(*myPlugIn));
            // make the plugin change the indicator
            myTestPlugIn->touchThatString(_myTouchIndicator);
            // check the indicator
            ENSURE(_myTouchIndicator == "touched");
        }

        ENSURE(!myManager.isLoaded(ourPlugInName));

        // Ensure loading a non-existent plugin throws
        // NOTE: the strange string is an invalid filename.
        ENSURE_EXCEPTION(myManager.getPlugIn("\?*"), PlugInException);
    }

private:
    string _myTouchIndicator;

};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[])
        : UnitTestSuite(myName, argc, argv) {}
    
    void
    setup() {
        UnitTestSuite::setup();

        PlugInManager::get().setSearchPath(".");
        
        addTest(new PlugInManagerTest());
    }

};

int
main(int argc, char **argv) {
    MyTestSuite mySuite(argv[0], argc, argv);
    
    mySuite.run();
    
    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;
    
    return mySuite.returnStatus();
}
