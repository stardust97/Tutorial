#ifndef _SD_COMM_SHM_H_
#define _SD_COMM_SHM_H_

#include <cstdint>
#include <string_view>
#include <stdexcept>


namespace sd {
namespace commu {
class ShareMemory {
public:
  ShareMemory() = default;

  explicit ShareMemory(std::string_view shm_name, uint32_t size);
  ~ShareMemory();

  bool Write(const void *data, uint32_t size);
  bool Read(void *data, uint32_t size);

  template <typename T> 
  T Read() {
    if(sizeof(T)> shm_size_){
      throw std::runtime_error("too large data");
    }
    return *(reinterpret_cast<T*>(shm_ptr_));
  }

private:
  enum class ShmState {
    Default = 0,
    Open,
    Truncated,
    Mapped,
    Closed,
  };

private:
  int shm_fd_;
  uint32_t shm_size_;
  void *shm_ptr_ = nullptr;
  ShmState state_;
};

} // namespace commu
} // namespace sd

#endif