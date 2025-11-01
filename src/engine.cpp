#include "engine.h"
#include "utils.h"
#include "rule.h"
#include "hook.h"

#include "lock.h"

namespace Engine {
    std::shared_ptr<Hook> Hooks[MaxHooks] = { nullptr };
    std::atomic_flag Lock = ATOMIC_FLAG_INIT;
    PVOID Handler = nullptr;
}

LONG Engine::ExceptionHandler(PEXCEPTION_POINTERS exception) {
    if (exception->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {

        Drx drx = Utils::GetTriggeredDr(exception->ContextRecord);

        auto hook = Engine::GetHook(static_cast<int>(drx));

        if (hook) {

            if (hook->Active) {
                Context ctx = Context::BuildContext(exception, hook);

                for (auto& rule : hook->Rules) {
                    rule.Execute(ctx);
                }
            }
        }

        Utils::ClearDr6(exception->ContextRecord);
        Utils::ContinueExecution(exception->ContextRecord);

        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

std::shared_ptr<ExecutionHook> Engine::CreateExecutionHook(PVOID address, bool global) {
    auto guard = LockGuard(Engine::Lock);

    for (int i = 0; i < Engine::MaxHooks; i++)
    {
        if (!Engine::Hooks[i]) {
            auto hook = std::make_shared<ExecutionHook>(address, global, static_cast<int>(i));

            Engine::Hooks[i] = hook;

            hook->Activate();

            return hook;
        }

    }

    return nullptr;
}

std::shared_ptr<WatchpointHook> Engine::CreateWatchpoint(PVOID address, bool global, WatchpointLength length, WatchpointCondition condition) {
    auto guard = LockGuard(Engine::Lock);

    for (int i = 0; i < Engine::MaxHooks; i++)
    {
        if (!Engine::Hooks[i]) {
            auto hook = std::make_shared<WatchpointHook>(address, global, static_cast<int>(i), length, condition);

            Engine::Hooks[i] = hook;

            hook->Activate();

            return hook;
        }

    }

    return nullptr;
}

EngineResult Engine::RemoveHook(int index) {
    auto guard = LockGuard(Engine::Lock);

    if (index < 0 || index >= Engine::MaxHooks) {
        return EngineResult::Failure;
    }

    if (!Engine::Hooks[index]) {
        return EngineResult::Failure;
    }

    auto hook = Engine::Hooks[index];

    hook->Deactivate();

    Engine::Hooks[index] = nullptr;

    return EngineResult::Success;
}

std::shared_ptr<Hook> Engine::GetHook(int index) {
    auto guard = LockGuard(Engine::Lock);

    if (index < 0 || index >= Engine::MaxHooks)
        return nullptr;

    auto hook = Engine::Hooks[index];

    return hook;
}


EngineResult Engine::Start() {
    if (Engine::Handler)
        return EngineResult::Failure;

    PVOID handler = AddVectoredExceptionHandler(
        1,
        Engine::ExceptionHandler
    );

    Engine::Handler = handler;

    return Engine::Handler != nullptr ? EngineResult::Success : EngineResult::Failure;
}

EngineResult Engine::Stop() {
    if (!Engine::Handler)
        return EngineResult::Failure;

    for (int i = 0; i < Engine::MaxHooks; i++)
    {
        auto hook = Engine::GetHook(i);

        if (hook) {
            hook->Deactivate();
            Engine::RemoveHook(i);
        }
    }

    ULONG result = RemoveVectoredExceptionHandler(Engine::Handler);

    if (result) {
        Engine::Handler = nullptr;
        return EngineResult::Success;
    }

    return EngineResult::Failure;
}


EngineResult Engine::SetBreakPoint(HANDLE thread, PVOID address, Drx drx) {
    CONTEXT threadCtx { 0 };
    EngineResult success = Utils::GetThreadContext(thread, threadCtx);
    
    if (success != EngineResult::Success) {
        return EngineResult::Failure;
    }
    

    Utils::SetDebugRegister(
        &threadCtx,
        address,
        drx
    );

    Utils::SetDr7Bits(
        &threadCtx,
        static_cast<int>(drx) * 2,
        1,
        1
    );

    if (!SetThreadContext(thread, &threadCtx))
        return EngineResult::Failure;
    
    return EngineResult::Success;
}

EngineResult Engine::SetBreakPointGlobal(PVOID address, Drx drx) {
    return Utils::ForEachThread([drx, address](HANDLE thread) -> EngineResult {
        return Engine::SetBreakPoint(
            thread,
            address,
            drx
        );
    });
}

EngineResult Engine::RemoveBreakPointGlobal(Drx drx) {
    return Utils::ForEachThread([drx](HANDLE thread) -> EngineResult {
        return Engine::RemoveBreakPoint(
            thread,
            drx
        );
    });
}

EngineResult Engine::SetWatchPointGlobal(PVOID address, Drx drx, WatchpointCondition condition, WatchpointLength length) {
    return Utils::ForEachThread([drx, address, condition, length](HANDLE thread) -> EngineResult {
        return Engine::SetWatchPoint(
            thread,
            address,
            drx,
            condition,
            length
        );
    });
}

EngineResult Engine::RemoveWatchPointGlobal(Drx drx) {
    return Utils::ForEachThread([drx](HANDLE thread) -> EngineResult {
        return Engine::RemoveWatchPoint(
            thread,
            drx
        );
    });
}

EngineResult Engine::SetWatchPoint(HANDLE thread, PVOID address, Drx drx, WatchpointCondition condition, WatchpointLength length) {
    CONTEXT threadCtx { 0 };
    EngineResult success = Utils::GetThreadContext(thread, threadCtx);
    
    if (success != EngineResult::Success) {
        return EngineResult::Failure;
    }

    Utils::SetDebugRegister(
        &threadCtx,
        address,
        drx
    );

    Utils::SetDr7Bits(
        &threadCtx,
        static_cast<int>(drx) * 2,
        1,
        1
    );

    // set condition bits //
    Utils::SetDr7Bits(
        &threadCtx,
        ((static_cast<int>(drx) * 4) + 16),
        2,
        (DWORD64)condition
    );

    // set length bits //
    Utils::SetDr7Bits(
        &threadCtx,
        ((static_cast<int>(drx) * 4) + 18),
        2,
        static_cast<DWORD64>(length)
    );

    if (!SetThreadContext(thread, &threadCtx))
        return EngineResult::Failure;

    return EngineResult::Success;
}

EngineResult Engine::RemoveWatchPoint(HANDLE thread, Drx drx) {
    CONTEXT threadCtx { 0 };
    EngineResult success = Utils::GetThreadContext(thread, threadCtx);
    
    if (success != EngineResult::Success) {
        return EngineResult::Failure;
    }

    Utils::SetDebugRegister(
        &threadCtx,
        nullptr,
        drx
    );


    // set condition bits //
    Utils::SetDr7Bits(
        &threadCtx,
        ((static_cast<int>(drx) * 4) + 16),
        2,
        0
    );

    // set length bits //
    Utils::SetDr7Bits(
        &threadCtx,
        ((static_cast<int>(drx) * 4) + 18),
        2,
        0
    );

    if (!SetThreadContext(thread, &threadCtx))
        return EngineResult::Failure;

    return EngineResult::Success;
}

EngineResult Engine::RemoveBreakPoint(HANDLE thread, Drx drx) {
    CONTEXT threadCtx { 0 };
    EngineResult success = Utils::GetThreadContext(thread, threadCtx);
    
    if (success != EngineResult::Success) {
        return EngineResult::Failure;
    }

    Utils::SetDebugRegister(
        &threadCtx,
        nullptr,
        drx
    );

    Utils::SetDr7Bits(
        &threadCtx,
        static_cast<int>(drx) * 2,
        1,
        0
    );

    if (!SetThreadContext(thread, &threadCtx))
        return EngineResult::Failure;

    return EngineResult::Success;
}