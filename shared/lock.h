#pragma once

#include<atomic>

class LockGuard {
public:
	LockGuard(std::atomic_flag& flag) : Flag(flag) {
		while (Flag.test_and_set(std::memory_order_acquire)) {
			_mm_pause();
		}
	}

	~LockGuard() {
		Flag.clear(std::memory_order_release);
	}

private:
	std::atomic_flag& Flag;
};