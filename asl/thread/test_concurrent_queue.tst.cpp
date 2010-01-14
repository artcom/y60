
#include <asl/thread/concurrent_queue.h>

#include <boost/date_time/posix_time/posix_time.hpp>

int
main(int argc, char** argv) {
	asl::thread::concurrent_queue<int> q;

	q.push(5);
	q.push(23);
	q.push(42);

	int r;
	q.try_pop(r);
	q.wait_and_pop(r);
	q.timed_wait_and_pop(r, boost::posix_time::minutes(1));

	return 0;
}
