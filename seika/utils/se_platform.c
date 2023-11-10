#include "se_platform.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#elif defined(__APPLE__)
#include <unistd.h>
#include <sys/ptrace.h>
#elif defined(__linux__)
#include <sys/ptrace.h>
#endif

bool se_platform_is_debugger_attached() {
#if defined(_WIN32) || defined(_WIN64)
    return IsDebuggerPresent();
#elif defined(__APPLE__)
    return (ptrace(PT_DENY_ATTACH, 0, 0, 0) == -1);
#elif defined(__linux__)
    return (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1);
#endif
    return false;
}
