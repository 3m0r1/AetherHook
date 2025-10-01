#pragma once

#include<windows.h>
#include<vector>
#include<atomic>
#include<memory>

#include<TlHelp32.h>

#include "../shared/types.h"

class Hook;
class ExecutionHook;
class WatchpointHook;

class LockGuard {
public:
	explicit LockGuard(std::atomic_flag& flag) : Flag(flag) {
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

namespace Engine {
	constexpr int MaxHooks = 4;

	extern std::shared_ptr<Hook> Hooks[MaxHooks];
	extern std::atomic_flag Lock;
	extern PVOID Handler;

	static LONG ExceptionHandler(PEXCEPTION_POINTERS exception);
	
	void SetLockAndWait();
	void ClearLock();

	EngineResult SetBreakPoint(HANDLE thread, PVOID address, Drx drx);
	EngineResult RemoveBreakPoint(HANDLE thread, Drx drx);

	EngineResult SetBreakPointGlobal(PVOID address, Drx drx);
	EngineResult RemoveBreakPointGlobal(Drx drx);

	EngineResult SetWatchPoint(HANDLE thread, PVOID address, Drx drx, WatchpointCondition condition, WatchpointLength length);
	EngineResult RemoveWatchPoint(HANDLE thread, Drx drx);

	EngineResult SetWatchPointGlobal(PVOID address, Drx drx, WatchpointCondition condition, WatchpointLength length);
	EngineResult RemoveWatchPointGlobal(Drx drx);

	std::shared_ptr<ExecutionHook> CreateExecutionHook(PVOID address, bool global);
	std::shared_ptr<WatchpointHook> CreateWatchpoint(PVOID address, bool global, WatchpointLength length, WatchpointCondition condition);

	EngineResult AddHook(std::shared_ptr<Hook> hook);
	EngineResult RemoveHook(int index);
	std::shared_ptr<Hook> GetHook(int index);

	EngineResult Start();
	EngineResult Stop();
}