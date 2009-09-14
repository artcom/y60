
#ifndef ASL_THREAD_CONCURRENT_QUEUE_H_
#define ASL_THREAD_CONCURRENT_QUEUE_H_

#include <queue>
#include <deque>

#include <boost/thread.hpp>

namespace asl {
namespace thread {

template<typename Value_Type_>
class concurrent_queue {

public:

    typedef Value_Type_ value_type;
    
    typedef std::queue<value_type> queue_type;
    
    typedef typename queue_type::container_type container_type;
    
    typedef typename container_type::size_type       size_type;
    typedef typename container_type::difference_type difference_type;
    
    typedef typename container_type::reference       reference;
    typedef typename container_type::const_reference const_reference;

private:

    // our inner queue
    queue_type queue_;

    // mutex protecting queue
    mutable boost::mutex mutex_;

    // condition variable signalling pushes
    boost::condition_variable condition_variable_;

public:

    bool empty() const
    {
        boost::mutex::scoped_lock lock(mutex_);

        return queue_.empty();
    }
    
    size_type size() const
    {
    	boost::mutex::scoped_lock lock(mutex_);
    	
    	return queue_.size();
    }
	
    void push(const_reference data)
    {
        boost::mutex::scoped_lock lock(mutex_);

        queue_.push(data);

        lock.unlock();

        condition_variable_.notify_one();
    }

    void flush()
    {
    	boost::mutex::scoped_lock lock(mutex_);
    	
    	while(!queue_.empty()) {
    		queue_.pop();
    	}
    }

    bool try_pop(reference popped_value)
    {
        boost::mutex::scoped_lock lock(mutex_);

        if(queue_.empty()) {
            return false;
        }
        
        popped_value = queue_.front();

        queue_.pop();

        return true;
    }

    void wait_and_pop(reference popped_value)
    {
        boost::mutex::scoped_lock lock(mutex_);

        while(queue_.empty()) {
            condition_variable_.wait(lock);
        }
        
        popped_value = queue_.front();

        queue_.pop();
    }

    template<typename Duration_Type_>
    bool timed_wait_and_pop(reference popped_value, Duration_Type_ wait_duration)
    {
        boost::system_time const timeout = boost::get_system_time() + wait_duration;

        boost::mutex::scoped_lock lock(mutex_);

        while(queue_.empty()) {
            if(!condition_variable_.timed_wait(lock, timeout)) {
                return false;
            }
        }
        
        popped_value = queue_.front();

        queue_.pop();
        
        return true;
    }

};

}
}

#endif // ASL_THREAD_CONCURRENT_QUEUE_H_
