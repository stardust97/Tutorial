#ifndef __LOCK_FREE_STACK_H_
#define __LOCK_FREE_STACK_H_

#include <atomic>
#include <cstddef>
#include <memory>

template <typename T> 
class LockFreeStack {
public:

LockFreeStack(): head_(nullptr) {

}

~LockFreeStack() {
  Node* h = head_.node;
  while (h) {
    Node *n = h->next;
    delete h;
    h = n;
  }
}

void push(T const &data) {
  RefNode new_node = new Node(data);
  new_node.node->next=head_.node;
  while (!head_.compare_exchange_weak(new_node->next, new_node));
}

std::shared_ptr<T> pop() {

  while (true) {
    RefNode old_head = head_.load();
    old_head.external_count++;
    RefNode new_head;
    do {
      new_head = old_head;
      new_head.external_count++;
    } while (head_.compare_exchange_weak(old_head, new_head));

    if (head_.compare_exchange_weak(new_head, new_head->next)) {
      // return value
    } else {
      //delete or retry
    }
  }


}

private:
  struct RefNode;
  struct Node {
    //todo 这里的data需要支持拷贝构造
    Node(T const &data): next(nullptr), data(std::make_shared(data)), internal_count(0){

    }
    RefNode next; // 这里为什么用值而非指针？
    std::shared_ptr<T> data;
    std::atomic<int> internal_count;
  };

  struct RefNode {
    RefNode() : node(nullptr), external_count(1) {}
    RefNode(T const &data) : node(new Node(data)), external_count(1) {}

   ~ RefNode(){
    //不管理node的生命周期
   }

    Node *node;
    int external_count;
  };

private:
  std::atomic<RefNode> head_;
};

#endif