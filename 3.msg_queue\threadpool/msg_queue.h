#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__

#include <queue>
#include <mutex>
#include <condition_variable>


template<typename T>
class msg_queue{
private:
    std::queue<T> _que;
    std::mutex _mtx;
    std::condition_variable _cv;
public:
    msg_queue() = default;
    ~msg_queue() = default;
    void push(const T& msg){
        std::unique_lock<std::mutex> lock(_mtx);
        _que.push(msg);
        _cv.notify_one();
    }
    void push(T&& msg){
        std::unique_lock<std::mutex> lock(_mtx);
        _que.push(std::move(msg));
        _cv.notify_one();
    }
    T pop(){
        std::unique_lock<std::mutex> lock(_mtx);
        _cv.wait(lock, [this]{return !_que.empty();});
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