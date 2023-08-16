#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__

#include <queue>
#include <mutex>
#include <semaphore.h>


template<typename T>
class msg_queue{
private:
    std::queue<T> _que;
    std::mutex _mtx;
    sem_t _sem;
public:
    msg_queue(){
        sem_init(&_sem, 0, 0);
    }

    ~msg_queue(){
        sem_destroy(&_sem);
    }

    void push(const T& msg){
        std::unique_lock<std::mutex> lock(_mtx);
        _que.push(msg);
        sem_post(&_sem);
    }

    void push(T&& msg){
        std::unique_lock<std::mutex> lock(_mtx);
        _que.push(std::move(msg));
        sem_post(&_sem);
    }

    T pop(){
        sem_wait(&_sem);
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