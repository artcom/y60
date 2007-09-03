
#include <sigsegv.h>

#include <iostream>

#include "TuttleDebugger.h"
#include "TuttleShell.h"

using namespace std;
using namespace tuttle;

static int
sigsegv_handler(void *fault_address, int serious)
{
    if(serious) {
        cout << "Unhandled page fault accessing " << fault_address << endl;
    }
    return 0;
}

int
main(int argc, char **argv)
{
    Debugger myDebugger;
    Shell    myShell(myDebugger);

    bool_t mySuccess;

    sigsegv_install_handler(&sigsegv_handler);
  
    mySuccess = myShell.launch();

    sleep(1000);

    sigsegv_deinstall_handler();
  
    return mySuccess ? 0 : 1;

}
