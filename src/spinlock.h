#pragma once
#include <atomic>

#if defined(_MSC_VER)
	#include <intrin.h>
	#if defined(_M_AMD64) || defined(_M_IX86)
	#pragma intrinsic(_mm_pause)
	#define spin_pause()  _mm_pause()
	#elif defined(_M_ARM64) || defined(_M_ARM)
	#define spin_pause()  __yield()
	#else
	#define spin_pause()  YieldProcessor()
	#endif
#elif defined(__x86_64__) || defined(__i386__)
	#define spin_pause()  __builtin_ia32_pause()
#elif defined(__aarch64__)
	#define spin_pause()  __asm__ __volatile__("yield")
#elif defined(__arm__)
	#if defined(__ARM_ARCH) && __ARM_ARCH >= 7
	#define spin_pause()  __asm__ __volatile__("yield")
	#else
	#define spin_pause()  ((void)0)
	#endif
#else
	#define spin_pause()  ((void)0)
#endif

namespace asynclog
{

class spinlock
{
	std::atomic<bool> lock_{false};

public:
	spinlock() = default;
	spinlock(const spinlock&) = delete;
	spinlock& operator=(const spinlock&) = delete;

	void lock() noexcept {
		for (;;) {
			// Optimistically assume the lock is free on the first try
			if (!lock_.exchange(true, std::memory_order_acquire)) {
				return;
			}
			// Wait for lock to be released without generating cache misses
			while (lock_.load(std::memory_order_relaxed)) {
				// Issue X86 PAUSE or ARM YIELD to reduce hyper-thread contention
				spin_pause();
			}
		}
	}

	bool try_lock() noexcept {
		// Relaxed load first to avoid extra RMW traffic in a try-lock spin
		return !lock_.load(std::memory_order_relaxed) &&
			!lock_.exchange(true, std::memory_order_acquire);
	}

	void unlock() noexcept {
		lock_.store(false, std::memory_order_release);
	}
};

}
