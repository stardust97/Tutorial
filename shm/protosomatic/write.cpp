#include <chrono>
#include <cstddef>
#include <fcntl.h>          // For O_* constants  
#include <sys/mman.h>       // For mmap()  
#include <sys/stat.h>       // For mode constants  
#include <thread>
#include <unistd.h>         // For close()  
#include <stdio.h>  
#include <stdlib.h>
#include <string>
#include <vector>

static std::string shm_id = "/myshm";
static uint32_t shm_size = 4096;
static uint32_t write_num = 10;
static uint32_t cnt_num = 0;

void *shm_ptr = nullptr;

void func1() {
  // 写入数据到共享内存
  for (int i = 0; i < write_num; ++i) {
    sprintf((char *)shm_ptr, "Hello, World! id: %d\n", cnt_num++);
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // 假设其他进程也映射了这块共享内存，并读取了内容
  }
}


int main() {
  int shm_fd = shm_open(shm_id.c_str(), O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  // 设置共享内存的大小
  if (ftruncate(shm_fd, shm_size) == -1) { // 4096 bytes
    perror("ftruncate");
    close(shm_fd);
    exit(EXIT_FAILURE);
  }

  // 使用mmap()函数将共享内存映射到调用进程的地址空间。
  shm_ptr = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shm_ptr == MAP_FAILED) {
    perror("mmap");
    close(shm_fd);
    exit(EXIT_FAILURE);
  }

  std::vector<std::thread> v;
  for(int i =0;i<100;++i){
    v.push_back(std::thread(func1));
  }

  for(auto &t:v){
    t.join();
  }

  printf("cnt_num: %d\n", cnt_num);

  // 取消映射和关闭共享内存对象
  if (munmap(shm_ptr, 4096) == -1) {
    perror("munmap");
    close(shm_fd);
    exit(EXIT_FAILURE);
  }

  close(shm_fd);
}