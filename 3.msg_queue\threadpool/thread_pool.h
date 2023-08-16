#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include "msg_queue.h"
#include <thread>
#include <vector>
#include <future>
#include <functional>

class thread_pool
{
private:
    // 线程池
    std::vector<std::thread> _workers;
    msg_queue<std::function<void()>> _tasks;
    bool _stop = false;
public:
    thread_pool(size_t);
    ~thread_pool();
    template <class F, class... Args>
    auto enqueue(F &&f, Args &&...args) -> std::future<decltype(f(args...))>;
};

inline thread_pool::thread_pool(size_t threads){
    for (size_t i = 0; i < threads; ++i){
        _workers.emplace_back([this]{
            for (;;){
                std::function<void()> task = std::move(_tasks.pop());
                if( _stop ) return;
                task();
            }}
        );
    }
}

template<class F, class... Args>
auto thread_pool::enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))>{
    using return_type = decltype(f(args...));
    auto task = std::make_shared< std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
    std::future<return_type> res = task->get_future();
    _tasks.push([task](){ (*task)(); });
    return res;
}

inline thread_pool::~thread_pool(){
    _stop = true;
    for (size_t i = 0; i < _workers.size(); ++i)
        _tasks.push([]{});
    
    for (size_t i = 0; i < _workers.size(); ++i)
        _workers[i].join();
    
}

#endif // __THREAD_POOL_H__