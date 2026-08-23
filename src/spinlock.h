#pragma once
#include <atomic>

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

struct spinlock 
{
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

}
