
#ifndef ASL_THREAD_CONCURRENT_QUEUE_H_
#define ASL_THREAD_CONCURRENT_QUEUE_H_

#include <queue>
#include <deque>

#include <boost/thread.hpp>

namespace asl {
namespace thread {

/**
 * @class concurrent_queue
 * @brief Locked multi-reader multi-writer queue.
 *
 * This class models a thread-safe queue that can be accessed
 * concurrently by multiple readers and multiple writers.
 *
 * It is lock-based and locks on all operations.
 *
 */
template<typename Value_Type_>
class concurrent_queue {

public:

	/**
	 * Type of queue elements.
	 */
    typedef Value_Type_ value_type;
    
    /**
     * Type of the inner queue.
     */
    typedef std::queue<value_type> queue_type;
    
    /**
     * Container type used by the inner queue.
     */
    typedef typename queue_type::container_type container_type;
    
    /**
     * Type for sizes concerning the queue.
     */
    typedef typename container_type::size_type       size_type;

    /**
     * Type for size differences concerning the queue.
     */
    typedef typename container_type::difference_type difference_type;
    
    /**
     * Reference type for elements.
     */
    typedef typename container_type::reference       reference;

    /**
     * Const reference type for elements.
     */
    typedef typename container_type::const_reference const_reference;

private:

    /**
     * The inner queue containing our elements.
     */
    queue_type queue_;

    /**
     * Mutex protecting the inner queue.
     */
    mutable boost::mutex mutex_;

    /**
     * Condition variable signaling pushes to the queue.
     */
    boost::condition_variable condition_variable_;

public:

    /**
     * Determine if the queue is empty.
     */
    bool empty() const
    {
        boost::mutex::scoped_lock lock(mutex_);

        return queue_.empty();
    }
    
    /**
     * Determine the number of elements on the queue.
     */
    size_type size() const
    {
    	boost::mutex::scoped_lock lock(mutex_);
    	
    	return queue_.size();
    }
	
    /**
     * Push an element onto the queue.
     */
    void push(const_reference data)
    {
        boost::mutex::scoped_lock lock(mutex_);

        queue_.push(data);

        lock.unlock();

        condition_variable_.notify_one();
    }

    /**
     * Remove all elements from the queue atomically.
     */
    void flush()
    {
    	boost::mutex::scoped_lock lock(mutex_);
    	
    	while(!queue_.empty()) {
    		queue_.pop();
    	}
    }

    /**
     * Try to pop an element off the queue without waiting.
     */
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

    /**
     * Pop an element off the queue, waiting for one if the queue is empty.
     */
    void wait_and_pop(reference popped_value)
    {
        boost::mutex::scoped_lock lock(mutex_);

        while(queue_.empty()) {
            condition_variable_.wait(lock);
        }
        
        popped_value = queue_.front();

        queue_.pop();
    }

    /**
     * Pop an element off the queue, waiting for at most
     * the specified duration if the queue is empty.
     */
    template<typename Duration_Type_>
    bool timed_wait_and_pop(
    		reference popped_value,
    		Duration_Type_ wait_duration
    		)
    {
        boost::system_time const timeout =
        		boost::get_system_time() + wait_duration;

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
