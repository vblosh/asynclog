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
	NodeAllocator alloc;
	Node* stub;
	std::atomic<Node*> head;
	std::atomic<Node*> tail;
public:
	MpscQueue(size_t capacity) : alloc(capacity), stub(CreateNode(Node(Logdata()))), head(stub), tail(stub) {
		stub->next.store(nullptr);
	}

	template<typename T>
	Node* CreateNode(T&& node) {
		return alloc.allocate(std::forward<T>(node));
	}

	void DeleteNode(Node* node) {
		alloc.deallocate(node);
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
