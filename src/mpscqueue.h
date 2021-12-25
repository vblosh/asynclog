#pragma once
#include <atomic>
#include <mutex>
#include <stdexcept>
#include "logging.h"

using namespace std;

namespace asynclog
{

struct Node {
	std::atomic<Node*> next;
	Logdata value;

	Node() : next(nullptr) {}

	Node(Node&&) = default;
	Node& operator=(Node&&) = default;

	Node(const Node& other) {
		next.store(other.next);
		value = other.value;
	}
	Node& operator=(const Node&) = delete;
};

struct spinlock {
	std::atomic<bool> lock_ = { 0 };

	void lock() noexcept {
		for (;;) {
			// Optimistically assume the lock is free on the first try
			if (!lock_.exchange(true, std::memory_order_acquire)) {
				return;
			}
			// Wait for lock to be released without generating cache misses
			while (lock_.load(std::memory_order_relaxed)) {
				// Issue X86 PAUSE or ARM YIELD instruction to reduce contention between
				// hyper-threads
				__builtin_ia32_pause();
			}
		}
	}

	bool try_lock() noexcept {
		// First do a relaxed load to check if lock is free in order to prevent
		// unnecessary cache misses if someone does while(!try_lock())
		return !lock_.load(std::memory_order_relaxed) &&
			!lock_.exchange(true, std::memory_order_acquire);
	}

	void unlock() noexcept {
		lock_.store(false, std::memory_order_release);
	}
};

class NodeAllocator
{
	std::vector<Node> data;
	size_t current;
	Node* freeTail;		// tail of free queue of deallocated nodes
	spinlock allocLock;
public:
	NodeAllocator(size_t capacity) : current(0), freeTail(nullptr) {
		data.resize(capacity);
	}

	~NodeAllocator() {
	}

	Node* Allocate() {
		std::lock_guard<spinlock> lock(allocLock);
		Node* ret;
		if (freeTail) { // there are deallocated nodes in free queue
			ret = freeTail;
			freeTail = freeTail->next;
		}
		else { // there are no deallocated nodes in free queue
			ret = &data[current++];
			// TODO: expand data
			if (current == data.size())
				throw std::runtime_error("overflow");
		}

		return ret;
	}

	void Deallocate(Node* node) {
		std::lock_guard<spinlock> lock(allocLock);
		node->next = freeTail;
		freeTail = node;
	}
};

class MpscQueue {
	NodeAllocator alloc;
	Node* stub;
	std::atomic<Node*> head;
	std::atomic<Node*> tail;
public:
	MpscQueue(size_t capacity) : alloc(capacity), stub(CreateNode()), head(stub), tail(stub) {
		stub->next.store(nullptr);
	}

	Node* CreateNode() {
		return alloc.Allocate();
	}

	void DeleteNode(Node* node) {
		alloc.Deallocate(node);
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
			head_copy->value = std::move(next->value);
			return head_copy;
		}
		return nullptr;
	}
};
}
