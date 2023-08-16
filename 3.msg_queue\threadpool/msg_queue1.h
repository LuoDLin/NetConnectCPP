#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__

#include <queue>
#include <mutex>
#include <semaphore>


template<typename T>
class msg_queue{
private:
    std::queue<T> _que;
    std::mutex _mtx;
    std::counting_semaphore<1> _sem{0};
public:
    msg_queue() = default;
    ~msg_queue() = default;
    void push(const T& msg){
        std::unique_lock<std::mutex> lock(_mtx);
        _que.push(msg);
        _sem.release();
    }
    void push(T&& msg){
        std::unique_lock<std::mutex> lock(_mtx);
        _que.push(std::move(msg));
        _sem.release();
    }
    T pop(){
        _sem.acquire();
        std::unique_lock<std::mutex> lock(_mtx);
        T msg = std::move(_que.front());
        _que.pop();
        return msg;
    }
    
    size_t size(){
        std::unique_lock<std::mutex> lock(_mtx);
        return _que.size();
    }
};



#endif