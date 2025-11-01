#pragma once

#include<windows.h>
#include<TlHelp32.h>
#include<functional>

#include "types.h"

namespace Utils {
	Drx GetTriggeredDr(PCONTEXT context);

	EngineResult SetDr7Bits(PCONTEXT context, int startPos, int width, DWORD64 value);
	EngineResult SetDebugRegister(PCONTEXT context, PVOID address, Drx drx);

	EngineResult ForEachThread(std::function<EngineResult(HANDLE)>);

	EngineResult GetThreadContext(HANDLE thread, CONTEXT& context);

	void ClearDr6(PCONTEXT context);
	void ContinueExecution(PCONTEXT context);
}