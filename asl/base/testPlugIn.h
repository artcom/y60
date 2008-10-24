
#ifndef ASL_TESTPLUGIN_H_
#define ASL_TESTPLUGIN_H_

#include <string>

#include <asl/base/PlugInBase.h>

namespace asl {

class ITestPlugIn : public PlugInBase {
public:
    ITestPlugIn(DLHandle theDLHandle)
        : PlugInBase(theDLHandle) {}

    virtual void touchThatString(std::string& theString) = 0;
};

}

#endif /* !ASL_TESTPLUGIN_H_ */
