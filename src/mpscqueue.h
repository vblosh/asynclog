#pragma once
#include <atomic>
#include <mutex>
#include <stdexcept>
#include "logging.h"
#include "spinlock.h"

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
