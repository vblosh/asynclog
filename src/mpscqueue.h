#pragma once
#include <atomic>
#include <mutex>
#include <stdexcept>
#include "logging.h"
#include "spinlock.h"
#include "node.h"

namespace asynclog
{

class MpscQueue {
	Node* stub;
	std::atomic<Node*> head;
	std::atomic<Node*> tail;
public:
	MpscQueue(Node* astub) : stub(astub), head(stub), tail(stub) {
		stub->next.store(nullptr);
	}

	void push(Node* node) {
		node->next.store(nullptr, std::memory_order_relaxed);
		Node* prev = tail.exchange(node, std::memory_order_acq_rel);
		prev->next.store(node, std::memory_order_release);
	}

	Node* pop() {
		Node* head_copy = head.load(std::memory_order_relaxed);
		Node* next = head_copy->next.load(std::memory_order_acquire);

		if (next != nullptr) {
			head.store(next, std::memory_order_relaxed);
			head_copy->value = std::move(next->value);
			return head_copy;
		}
		return nullptr;
	}
};
}
