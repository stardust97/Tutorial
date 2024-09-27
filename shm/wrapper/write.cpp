#include "shm.h"
#include <chrono>
#include <ratio>
#include <string>
#include <thread>
#include <vector>

using namespace sd::commu;


int cnt = 0;

void func() {
  // for (int i = 0; i < 100; ++i) {
  //   std::string send_data = "hello world" + std::to_string(cnt++);
  //   shm.Write(send_data.data(), sizeof(send_data));
  //   std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // }
}
int main() {
  ShareMemory shm ("test",4096);
  // std::vector<std::thread> v;
  // for (int i = 0; i < 10; ++i) {
  //   v.push_back(std::thread(func));
  // }
  // for (auto &t : v) {
  //   t.join();
  // }
  return 0;
}
