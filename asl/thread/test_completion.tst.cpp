
#include <boost/thread.hpp>

#include "completion.h"

asl::thread::completion our_completion;

void
wait_and_signal() {
	boost::thread::sleep(boost::get_system_time() + boost::posix_time::seconds(2));
	our_completion.signal();
}

int
main(void) {

	boost::thread signaller(wait_and_signal);

	our_completion.wait();

	if(!our_completion.is_complete()) {
		return 1;
	}

	return 0;
}
