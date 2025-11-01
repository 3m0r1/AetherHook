#include <iostream>

#include "engine.h"
#include "rule.h"
#include "hook.h"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: OpenProcessExample.exe <PROCESS_ID>\n");
        return 1;
    }

    auto success = Engine::Start();

    if (success != EngineResult::Success) {
        printf("[-] Failed to start engine.\n");
        return 1;
    }

    auto filter = [](const Context& ctx) -> bool {
        auto access = ctx.GetArg<DWORD>(0);
        return (access & PROCESS_TERMINATE);
    };

    auto callback = [](Context& ctx) {
        auto access = ctx.GetArg<DWORD>(0);
        auto processId = ctx.GetArg<DWORD>(2);

        printf("[!] Detected an attempt to open a process (%ld) with the terminate flag.\n", processId);

        ctx.SetArg<DWORD>(0, access & ~PROCESS_TERMINATE);
    };

    auto hook = Engine::CreateExecutionHook(OpenProcess, false);

    hook->AddRule({ filter, callback });

    printf("[+] Press enter to trigger hook\n");

    getchar();

    DWORD processId = atoi(argv[1]);
    HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

    TerminateProcess(procHandle, 0);
    
    printf("[*] LastError: %ld\n", GetLastError());

    return 0;
}