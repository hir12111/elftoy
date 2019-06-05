
#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include <stdint.h>
#include <sys/ptrace.h>
#include <sys/types.h>

void read_memory(pid_t pid, uint64_t addr, void *buffer, size_t len);

#endif