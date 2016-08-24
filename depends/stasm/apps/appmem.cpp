// appmem.cpp: miscellaneous functions for tracking memory use
//
// Copyright (C) 2005-2013, Stephen Milborrow

#include "stasm.h"
#include "appmisc.h"
#if _MSC_VER // microsoft compiler
#include <psapi.h> // for OpenProcess and GetProcessMemoryInfo
#endif

namespace stasm
{
size_t PeakMemThisProcess(void) // peak memory used by the current process
{
#if _MSC_VER // microsoft compiler
    const HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                 FALSE, GetCurrentProcessId());
    if (!h)
        Err("OpenProcess failed");
    PROCESS_MEMORY_COUNTERS pmc;
    if (!GetProcessMemoryInfo(h, &pmc, sizeof(pmc)))
        Err("GetProcessMemoryInfo failed");
    CloseHandle(h);
    return pmc.PeakWorkingSetSize;
#else
    return 0;
#endif
}

int PercentPhysicalMemory(void) // percentage of memory currently used (all processes)
{
#if _MSC_VER // microsoft compiler
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    return int(statex.dwMemoryLoad);
#else
    return 0;
#endif
}

void CheckMem(void) // warn when memory used (by all processes) is over 80%
{
    static int oldpercent = 80 - 2;        // -2 so first warning is at 80%
    int percent = PercentPhysicalMemory(); // percent mem used by all processes
    if (percent >= oldpercent + 2)
    {
        oldpercent = percent;
        lprintf("\n%d%% physical mem all processes, %.0fMB peak this process\n",
                percent, double(PeakMemThisProcess()) / 1e6);
    }
}
} // namespace stasm
