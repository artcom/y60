
#ifndef _TUTTLE_APPLICATION_H_
#define _TUTTLE_APPLICATION_H_

namespace tuttle {

    class Application {

    public:

        Application();

        bool run();

    protected:
        virtual bool initJavascript(JSContext *theContext, JSObject *theGlobal);

    private:
        bool           _myTerminate;
        unsigned long  _myInterval;
        JSRuntime     *_myRuntime;
        JSContext     *_myContext;
    };

}

#endif
