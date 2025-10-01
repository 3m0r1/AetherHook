#pragma once

#include<windows.h>
#include<TlHelp32.h>
#include<functional>

#include "../shared/types.h"

namespace Utils {
	Drx GetTriggeredDr(PCONTEXT context);

	EngineResult SetDr7Bits(PCONTEXT context, int startPos, int width, DWORD64 value);
	EngineResult SetDebugRegister(PCONTEXT context, PVOID address, Drx drx);

	EngineResult ForEachThread(std::function<EngineResult(HANDLE)>);
	PCONTEXT GetThreadContext(HANDLE thread);

	void ContinueExecution(PCONTEXT context);
}