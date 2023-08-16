#include "msg_queue0.h"
#include <iostream>
#include <thread>

using namespace std;

int main()
{
    msg_queue<int> mq;
    for (int i = 0; i < 20; ++i)
    {
        thread pdct([&mq]
                    {
            for(int i = 0; i < 50'0000; ++i){
                mq.push(i);
            } });

        thread csmr([&mq]
                    {
            for(int i = 0; i < 50'0000; ++i){
                int msg = mq.pop();
            } });
        pdct.join();
        csmr.join();
    }
    return 0;
}
