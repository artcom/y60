#ifndef ASL_THREAD_COMPLETION_H_
#define ASL_THREAD_COMPLETION_H_

#include <boost/thread.hpp>

namespace asl {
namespace thread {

class completion {
public:
	completion() : complete_(false) {}
	~completion() {}

private:

	volatile bool complete_;

	boost::mutex mutex_;
	boost::condition_variable condition_variable_;

public:

	void signal() {
		complete_ = true;
		condition_variable_.notify_all();
	}

	bool is_complete() {
		return complete_;
	}

	void wait() {
		boost::mutex::scoped_lock lock(mutex_);

		while(!complete_) {
			condition_variable_.wait(lock);
		}
	}

	template<typename Duration_Type_>
	bool timed_wait(Duration_Type_ wait_duration) {
		boost::system_time const timeout = boost::get_system_time() + wait_duration;

		boost::mutex::scoped_lock lock(mutex_);

		while(!complete_) {
			if(!condition_variable_.timed_wait(lock, timeout)) {
				return false;
			}
		}

		return true;
	}
};

}
}

#endif // !ASL_THREAD_COMPLETION_H_
