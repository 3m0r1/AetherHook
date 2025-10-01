#include <iostream>

#include "include/engine.h"
#include "include/rule.h"
#include "include/hook.h"

int main()
{
    auto success = Engine::Start();

    if (success != EngineResult::Success) {
        printf("[-] Failed to start engine.\n");
    }

    auto filter = [](const Context& ctx) -> bool {
        auto access = ctx.GetArg<DWORD>(0);
        return (access & PROCESS_TERMINATE);
    };

    auto callback = [](Context& ctx) {
        printf("[!] Detected an attempt to open a process with the terminate flag.\n");

        auto access = ctx.GetArg<DWORD>(0);

        ctx.SetArg<DWORD>(0, access & ~PROCESS_TERMINATE);
    };

    auto hook = Engine::CreateExecutionHook(OpenProcess, false);

    hook->AddRule({ filter, callback });

    printf("[+] Press to trigger hook\n");

    getchar();

    DWORD pid = 67956; // place your pid here
    HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    TerminateProcess(procHandle, 0);
    
    printf("LastError: %ld\n", GetLastError());

    getchar();
}