
#include "TuttleApplication.h"

namespace tuttle {

    Application::Application() {
    }

    bool Application::run() {
        while(!_myTerminate) {
            
            if(!handleRequests())
                goto fail;

            if(!process())
                goto fail;
            
            usleep(_myInterval);
        }

        return true;

    fail:
        return false;
    }

    bool Application::handleRequests() {
        return true;
    }

    bool Application::process() {
        return true;
    }

}
