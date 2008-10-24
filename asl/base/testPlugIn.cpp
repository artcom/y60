
#include <string>
#include <iostream>

#include "testPlugIn.h"


namespace asl {

using namespace std;

class RealTestPlugIn : public ITestPlugIn {
public:

    RealTestPlugIn (DLHandle theDLHandle)
        : ITestPlugIn(theDLHandle) {};

    int
    initialize() {
        cout << "TestPlugIn initializing!" << endl;
        // cout << "SingletonManager in MyPlugIn :: initialize" << &SingletonManager::get() << endl;
        // cout << "PlugInManager search path:" << PlugInManager::get().getSearchPath() << endl;
        return 0;
    }

    void
    cleanUp() {
        cout << "TestPlugIn cleaning up!" << endl;
    }
    
    void
    touchThatString(std::string& theString) {
        theString = "touched";
    }
};

}

extern "C"
EXPORT asl::PlugInBase*
asltestplugin_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new asl::RealTestPlugIn(myDLHandle);
}

