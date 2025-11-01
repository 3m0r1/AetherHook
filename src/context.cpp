#include "context.h"
#include "engine.h"
#include "utils.h"
#include "hook.h"

Context Context::BuildContext(PEXCEPTION_POINTERS exception, std::shared_ptr<Hook> hook) {
    Context ctx{ 
        .ContextRecord = exception->ContextRecord,
        .ExceptionRecord = exception->ExceptionRecord,
        .EngineHook = hook
    };

    return ctx;
}


void Context::Return() {
    this->ContextRecord->Rip = *reinterpret_cast<uintptr_t*>(this->ContextRecord->Rsp);
    this->ContextRecord->Rsp += sizeof(uintptr_t);
}

DWORD64 Context::GetIP() const {
    return this->ContextRecord->Rip;
}

bool Context::CalledFrom(HMODULE moduleHandle) const {
    MODULEINFO modInfo{ 0 };

    if (!GetModuleInformation(
        GetCurrentProcess(),
        moduleHandle,
        &modInfo,
        sizeof(modInfo)
    )) {
        return false;
    }

    auto ip = this->GetIP();

    auto startImage = (DWORD64)modInfo.lpBaseOfDll;
    auto endImage = (DWORD64)((uintptr_t)modInfo.lpBaseOfDll + modInfo.SizeOfImage);

    if (ip > startImage && ip < endImage) {
        return true;
    }

    return false;
}

void Context::ActivateHook() {
    this->EngineHook->Activate();
}

void Context::DeactivateHook() {
    this->EngineHook->Deactivate();
}