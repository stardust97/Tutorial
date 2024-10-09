
#include <bits/c++config.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std::chrono_literals;

struct Buffer{
  char* buf;
  int len;
  uint32_t cnt = 0;

  // todo 移动构造与移动赋值

  // 判断当前缓冲区是否被填满
  bool full() {
    if (cnt++ > 500)
      return true;
    return false;
  }

  void append(char* text, int len){
    // 将text加入缓冲区
  }
};


std::mutex mutex_;
std::condition_variable cv_;
std::unique_ptr<Buffer> cur_buf_; //当前缓冲
std::unique_ptr<Buffer> next_buf_; // 预备缓冲,如果需要，这里也可以用数组保存多个预备缓冲区
std::vector<std::unique_ptr<Buffer>> buffers_; // 待写入文件的已填满缓冲

bool running_ = true;

// 日志前端向缓冲区写数据
void append(char *text, int len) {
  std::lock_guard<std::mutex> lock(mutex_);
  if(!cur_buf_->full()){
    cur_buf_->append(text, len); // 发生的概率高
  } else {                       // 当前缓冲区已满
    buffers_.push_back(std::move(cur_buf_));
    if (next_buf_) {
      cur_buf_ = std::move(next_buf_); // 更新当前缓冲区为备用缓冲区
    } else {
      cur_buf_.reset(new Buffer()); // 重新申请一个缓冲区（发生的概率低）
    }
    cur_buf_->append(text, len);
    cv_.notify_all(); // 通知写文件线程处理待写入的buffers
  }
  return;
}

// 日志后端向文件写数据
void write_thread_task(){
  std::unique_ptr<Buffer> buf1 = std::make_unique<Buffer>();
  std::unique_ptr<Buffer> buf2 = std::make_unique<Buffer>();
  std::vector<std::unique_ptr<Buffer>> buffers2write_;

  while (running_) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (buffers_.size() == 0) { //这里没有用while
        cv_.wait_for(lock, 1s); // 增加超时时间等，避免一直等待
      }

      buffers_.push_back(std::move(cur_buf_));
      cur_buf_ = std::move(buf1);
      buffers2write_.swap(buffers_);
      if (!next_buf_) {
        next_buf_ = std::move(buf2);
      }
    }
    // 在临界区外将buffers2write_写入文件
    // todo
    // 将写完的buffers2write_中的空闲缓冲区重新赋值给buf1和buf2
    // 这样后续的buf1和buf2还可以用于替换cur_buf_和next_buf_,
    // 减少日志前端的内存分配
  }
}

// 当前端日志大量堆积，需要丢掉多余的buffer，腾出内存，避免OOM

// Java ConcurrentHashMap中用多个bucket，前端写日志时按线程id 哈希到不同的bucket中，
// 减少contention