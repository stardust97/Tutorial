#include "shm.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace sd {
namespace commu {
ShareMemory::ShareMemory(std::string_view shm_name, uint32_t size)
    : shm_fd_{-1}, shm_size_{size}, shm_ptr_{nullptr},
      state_{ShmState::Default} {
  shm_fd_ = shm_open(shm_name.data(), O_CREAT | O_RDWR, 0666);
  if (shm_fd_ == -1) {
    perror("shm_open failed");
    throw std::runtime_error("shm_open failed");
  }
  state_ = ShmState::Open;

  if (ftruncate(shm_fd_, shm_size_) == -1) {
    perror("ftruncate");
    throw std::runtime_error("shm_ftruncate failed");
  }
  state_ = ShmState::Truncated;

  shm_ptr_ = mmap(0, shm_size_, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0);
  if (shm_ptr_ == MAP_FAILED) {
    perror("mmap");
    throw std::runtime_error("shm_mmap failed");
  }
  state_ = ShmState::Mapped;
}

ShareMemory::~ShareMemory() {
  switch (state_) {
  case ShmState::Mapped:
    munmap(shm_ptr_, shm_size_);
  case ShmState::Truncated:
  case ShmState::Open:
    close(shm_fd_);
  case ShmState::Closed:
    break;
  default:
    break;
  }
  state_ = ShmState::Closed;
}

bool ShareMemory::Write(const void *data, uint32_t size) {
  // todo check write protection
  memcpy(shm_ptr_, data, size);
  return true;
}

bool ShareMemory::Read(void *data, uint32_t size) {
  int read_size = std::min(size, shm_size_);
  memcpy(data, shm_ptr_, read_size);
  return true;
}
} // namespace commu
} // namespace sd