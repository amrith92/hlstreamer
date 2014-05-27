#ifndef CONCURRENT_QUEUE
#define CONCURRENT_QUEUE

#include <mutex>
#include <thread>
#include <condition_variable>
#include <deque>

/**
 *  concurrent_queue implements a C++11 style thread
 *  -safe queue. It ain't perfect, but it works.
 *  NOTE that pop() blocks indefinitely (until there is
 *       at least one element in the queue)
 */
template <typename T>
class concurrent_queue
{
public:
    T pop()
    {
        std::unique_lock<std::mutex> mlock(mutex_);

        while (queue_.empty()) {
            cond_.wait(mlock);
        }

        auto item = queue_.front();
        queue_.pop_front();
        return item;
    }

    void pop(T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (queue_.empty()) {
            cond_.wait(mlock);
        }
        item = queue_.front();
        queue_.pop_front();
    }

    void push(const T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push_back(item);
        mlock.unlock();
        cond_.notify_one();
    }

    void push(T&& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push_back(item);
        mlock.unlock();
        cond_.notify_one();
    }

    bool empty()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        bool isEmpty = queue_.empty();
        mlock.unlock();

        return isEmpty;
    }

    inline std::deque<T> &raw()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        std::deque<T> &queue = queue_;
        mlock.unlock();

        return queue;
    }

    size_t size()
    {
        size_t size = 0;

        std::unique_lock<std::mutex> mlock(mutex_);
        size = queue_.size();
        mlock.unlock();

        return size;
    }

protected:
    std::deque<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

#endif /* CONCURRENT_QUEUE */
