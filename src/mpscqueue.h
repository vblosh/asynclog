#pragma once
#include <atomic>
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

class NodeAllocator
{
	std::vector<Node> data;
	std::atomic<size_t> current;
public:
	NodeAllocator(size_t capacity) : current(0) {
		data.resize(capacity);
	}

	~NodeAllocator() {
	}

	Node* Allocate() {
		return &data[current++];
	}

	void Deallocate(Node*) {

	}
};

class MpscQueue {
	NodeAllocator alloc;
	Node* stub;
	std::atomic<Node*> head;
	std::atomic<Node*> tail;
public:
	MpscQueue(size_t capacity = 100000) : alloc(capacity), stub(CreateNode()), head(stub), tail(stub) {
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
