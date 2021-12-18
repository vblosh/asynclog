#pragma once
#include <atomic>
#include "logging.h"

using namespace std;

namespace asynclog
{

struct Node {
  std::atomic<Node*> next;
  Logdata value; 
};

class MpscQueue {
public:
  MpscQueue() : stub(new Node()), head(stub), tail(stub) {
    stub->next.store(nullptr);
  }

  void push(Node* node) {
    node->next.store(nullptr, memory_order_relaxed);
    Node* prev = tail.exchange(node, memory_order_acq_rel);  
    prev->next.store(node, memory_order_release);           
  }

  Node* pop() {
    Node* head_copy = head.load(memory_order_relaxed);
    Node* next = head_copy->next.load(memory_order_acquire);

    if (next != nullptr) {
      head.store(next, memory_order_relaxed);
      head_copy->value = next->value;
      return head_copy;
    }
    return nullptr;
  }

private:
  Node* stub;
  std::atomic<Node*> head;
  std::atomic<Node*> tail;
};
}