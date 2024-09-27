#include <chrono>
#include <fcntl.h>          // For O_* constants  
#include <sys/mman.h>       // For mmap()  
#include <sys/stat.h>       // For mode constants  
#include <thread>
#include <unistd.h>         // For close()  
#include <stdio.h>  
#include <stdlib.h>
#include <string>

static std::string shm_id = "/myshm";
static uint32_t shm_size = 4096;
// static uint32_t write_num = 10;

int main() {
  int shm_fd = shm_open(shm_id.c_str(), O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  // 使用mmap()函数将共享内存映射到调用进程的地址空间。
  void *ptr = mmap(0, shm_size, PROT_READ , MAP_SHARED, shm_fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    close(shm_fd);
    exit(EXIT_FAILURE);
  }

  char *str = (char *)ptr;
  while (true) {
    printf("Read from shared memory: %s\n", str);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  // 取消映射和关闭共享内存对象
  if (munmap(ptr, 4096) == -1) {
    perror("munmap");
    close(shm_fd);
    exit(EXIT_FAILURE);
  }

  close(shm_fd);
}