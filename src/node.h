#pragma once
#include <deque>
#include <vector>
#include "logdata.h"
#include "spinlock.h"

namespace asynclog
{
	struct Node {
		std::atomic<Node*> next;
		Logdata value;

		explicit Node(const Logdata& logdata) : next(nullptr), value(logdata) {}
		explicit Node(Logdata&& logdata) : next(nullptr), value(std::move(logdata)) {}

		Node(Node&&) = default;
		Node& operator=(Node&&) = default;

		Node(const Node& other) {
			next.store(other.next);
			value = other.value;
		}

		Node& operator=(const Node& other) {
			next.store(other.next);
			value = other.value;
			return *this;
		}
	};

	class NodeAllocator
	{
		std::deque<std::vector<Node>> data;
		size_t chunkSize;
		Node* freeTail;
		spinlock allocLock;
	public:
 		NodeAllocator(size_t capacity) : chunkSize(capacity), freeTail(nullptr) {
			addChunk();
		}

		~NodeAllocator() {
		}

		void addChunk() {
			data.resize(data.size() + 1);
			data.back().reserve(chunkSize);
		}

		template<typename T>
		Node* allocate(T&& node) {
			std::scoped_lock lock(allocLock);
			Node* ret;
			if (freeTail) { // there are deallocated nodes in free queue
				ret = freeTail;
				*ret = std::forward<T>(node);
				freeTail = freeTail->next;
			}
			else { // there are no deallocated nodes in free queue
				if (data.back().size() == chunkSize) {
					// allocate new chunk
					addChunk();
				}
				auto& current = data.back();
				current.push_back(std::forward<T>(node));
				ret = &current.back();
			}

			return ret;
		}

		void deallocate(Node* node) {
			std::scoped_lock lock(allocLock);
			node->next = freeTail;
			freeTail = node;
		}

		void truncateBuffer() {
			std::scoped_lock lock(allocLock);
			data.resize(1);
		}
	};

}
