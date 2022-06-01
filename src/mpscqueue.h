#pragma once
#include <atomic>
#include <mutex>
#include <stdexcept>
#include "logging.h"

using namespace std;

#if defined(_MSC_VER)

	/* prefer using intrinsics directly instead of winnt.h macro */
	/* http://software.intel.com/en-us/forums/topic/296168 */
	#include <intrin.h>
	#if defined(_M_AMD64) || defined(_M_IX86)
	#pragma intrinsic(_mm_pause)
	#define spin_pause()  _mm_pause()
	#elif defined(_M_IA64)
	#pragma intrinsic(__yield)
	#define spin_pause()  __yield()
	#else
	#define spin_pause()  YieldProcessor()
	#endif
#elif defined(__x86_64__) || defined(__i386__)

/* http://software.intel.com/sites/products/documentation/studio/composer/en-us/2011Update/compiler_c/intref_cls/common/intref_sse2_pause.htm
 * http://software.intel.com/sites/default/files/m/2/c/d/3/9/25602-17689_w_spinlock.pdf
 * http://software.intel.com/en-us/forums/topic/309231
 * http://siyobik.info.gf/main/reference/instruction/PAUSE
 * http://stackoverflow.com/questions/7086220/what-does-rep-nop-mean-in-x86-assembly
 * http://stackoverflow.com/questions/7371869/minimum-time-a-thread-can-pause-in-linux
 */
 /* http://gcc.gnu.org/onlinedocs/gcc/X86-Built_002din-Functions.html
  * GCC provides __builtin_ia32_pause() */
 #define spin_pause()  __builtin_ia32_pause()

#endif


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
				spin_pause();
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
