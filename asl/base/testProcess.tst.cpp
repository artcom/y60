
#include <asl/base/Process.h>

int
main(int argc, char** argv) {
    asl::Process p("ls /tmp");

    p.launch();
    p.waitForTermination();

    return 0;
}
