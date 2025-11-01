#include "utils.h"

Drx Utils::GetTriggeredDr(PCONTEXT context) {
    for (int i = static_cast<int>(Drx::Dr0); i <= static_cast<int>(Drx::Dr3); ++i) {
        if (context->Dr6 & (1ULL << i))
            return static_cast<Drx>(i);
    }
    return Drx::Invalid;
}

EngineResult Utils::SetDr7Bits(PCONTEXT context, int startPos, int width, DWORD64 value) {
    if (width <= 0 || startPos < 0 || startPos + width > 64)
        return EngineResult::Failure;

    DWORD64 mask = ((1ULL << width) - 1) << startPos;

    context->Dr7 = (context->Dr7 & ~mask) | ((value << startPos) & mask);

    return EngineResult::Success;
}

EngineResult Utils::SetDebugRegister(PCONTEXT context, PVOID address, Drx drx) {
    switch (drx) {

    case Drx::Dr0:
        if (!context->Dr0) {
            context->Dr0 = (DWORD64)address;
        }
        break;

    case Drx::Dr1:
        if (!context->Dr1) {
            context->Dr1 = (DWORD64)address;
        }
        break;

    case Drx::Dr2:
        if (!context->Dr2) {
            context->Dr2 = (DWORD64)address;
        }
        break;

    case Drx::Dr3:
        if (!context->Dr3) {
            context->Dr3 = (DWORD64)address;
        }
        break;

    default:
        return EngineResult::Failure;
    }

    return EngineResult::Success;
}


EngineResult Utils::ForEachThread(std::function<EngineResult(HANDLE)> callback) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    THREADENTRY32 te32{ 0 };
    te32.dwSize = sizeof(te32);

    if (!Thread32First(snapshot, &te32)) {
        CloseHandle(snapshot);
        return EngineResult::Failure;
    }

    while (Thread32Next(snapshot, &te32)) {
        if (te32.th32OwnerProcessID == GetCurrentProcessId()) {

            HANDLE thread = OpenThread(
                THREAD_GET_CONTEXT | THREAD_SET_CONTEXT,
                FALSE,
                te32.th32ThreadID
            );

            if (thread) {
                auto result = callback(thread);

                CloseHandle(thread);

                if (result != EngineResult::Success) {
                    CloseHandle(snapshot);
                    return EngineResult::Failure;
                }
            }
        }
    }

    CloseHandle(snapshot);
    return EngineResult::Success;
}



EngineResult Utils::GetThreadContext(HANDLE thread, CONTEXT& context) {
    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    
    if (!GetThreadContext(thread, &context))
        return EngineResult::Failure;

    return EngineResult::Success;
}


void Utils::ClearDr6(PCONTEXT context) {
    context->Dr6 = 0;
}
void Utils::ContinueExecution(PCONTEXT context) {
    context->EFlags |= (1 << 16);
}