#pragma once
#include "Typedefs.h"
#include "TextMode.h"
#include "Memory/PageFrameAllocator.h"
#include "Filesystem.h"

uint64 SyscallsHandler();

int syscall_open();
int syscall_close();
int syscall_remove();
int syscall_rmdir();
int syscall_mkdir();
int syscall_chdir();
int syscall_pwd();