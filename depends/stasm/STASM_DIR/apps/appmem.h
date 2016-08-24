// appmem.h: miscellaneous functions for tracking memory use
//
// Copyright (C) 2005-2013, Stephen Milborrow

#ifndef STASM_APPMEM_H
#define STASM_APPMEM_H

namespace stasm
{
size_t PeakMemThisProcess(void); // peak memory used by the current process

int PercentPhysicalMemory(void); // percentage of memory currently used (all processes)

void CheckMem(void);             // warn when memory used (by all processes) is over 80%

} // namespace stasm
#endif // STASM_APPMEM_H
