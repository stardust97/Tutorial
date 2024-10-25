#include <thread>
#include <iostream>

#include "single_p_c.cpp"


void TestSinglePopPush() {
    SinglePopPush<int>  que;
    std::thread t1([&]() {
        for (int i = 0; i < 10000; i++) {
            que.push(i);
            std::cout << "push data is " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });

    std::thread t2([&]() {
        for (int i = 0; i < 10000; i++) {
           auto p =  que.pop();
           if (p == nullptr) {
               std::this_thread::sleep_for(std::chrono::milliseconds(10));
               continue;
           }
           
           std::cout << "pop data is " << *p << std::endl;
        }
      });

    t1.join();
    t2.join();
}

int main(){

  TestSinglePopPush();
}