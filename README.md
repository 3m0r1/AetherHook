## AetherHook

Advanced windows hooking library with the ability to filter based on context.

## Example

```cpp
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
```
