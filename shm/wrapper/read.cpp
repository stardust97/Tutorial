#include "shm.h"

#include <iostream>
#include <thread>

using namespace sd::commu;

int main() {

  ShareMemory shm ("test",4096);
  while (true) {
    char buf[1024];
    shm.Read(buf, sizeof(buf));
    std::cout << buf << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}