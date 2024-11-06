#ifndef __LOCK_FREE_STACK_H_
#define __LOCK_FREE_STACK_H_

#include <atomic>
#include <cstddef>
#include <memory>

template <typename T> 
class LockFreeStack {
public:
LockFreeStack() {
  RefNode dummy;
  dummy.node = nullptr;
  dummy.external_count = 0;
  head_.store(dummy);
}

~LockFreeStack() {
  while (pop());
}

void push(T const &data) {
  RefNode new_node ;
  new_node.node = new Node(data);
  new_node.external_count = 1;
  new_node.node->next=head_.load();
  while (!head_.compare_exchange_weak(new_node.node->next, new_node));
}

std::shared_ptr<T> pop() {
  while (true) {
    RefNode old_head = head_.load();
    RefNode new_head;
    do {
      new_head = old_head;
      new_head.external_count++;
    } while (!head_.compare_exchange_weak(old_head, new_head));
    // 在多线程的情况下，此时head_的引用计数是cas成功的那个线程的引用计数

    old_head = new_head;
    Node *node = old_head.node;
    if (!node) {
      return nullptr;
    }
    if (head_.compare_exchange_strong(old_head, node->next)) { // pop头结点
      std::shared_ptr<T> res;
      res.swap(node->data);
      int increase_cnt = old_head.external_count - 2;
      if (node->internal_count.fetch_add(increase_cnt) == -increase_cnt) {
        delete node;
      }
      return res;
    } else {
      if (node->internal_count.fetch_sub(1) == 1) {
        delete node;
      }
    }
  }
}

private:
  struct Node;
  struct RefNode {
    Node *node;
    int external_count;
  };

  struct Node {
    //todo 这里的data需要支持拷贝构造
    Node(T const &data) : data(std::make_shared<T>(data)), internal_count(0) {}

    std::shared_ptr<T> data;
    std::atomic<int> internal_count;
    RefNode next; // 这里为什么用值而非指针？
  };

private:
  std::atomic<RefNode> head_;
};

#endif