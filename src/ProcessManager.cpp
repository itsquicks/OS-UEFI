#include "ProcessManager.h"

Process* processes;
int processCount;

void InitializeProcessManager()
{
    processes = (Process*)RequestPage();
    LockPage(processes);
    memset(processes, 0, 4096);

    processCount = 0;
}

Process* BuildProcess(void* address)
{
    Process* process = processes + processCount;
    process->PID = processCount + 1;
    process->state = PAUSED;
    process->address = address;

    FILE stdout;
    stdout.flags = 0;
    stdout.fileno = 1;
    stdout.buffer_base = (uint8*)RequestPage();
    LockPage(stdout.buffer_base);
    memset(stdout.buffer_base, 0, 4096);
    stdout.buffer_end = 0;
    stdout.read_ptr = stdout.buffer_base;
    stdout.write_ptr = stdout.buffer_base;

    process->stdout = stdout;

    FILE stderr;
    stderr.flags = 0;
    stderr.fileno = 2;
    stderr.buffer_base = (uint8*)RequestPage();
    LockPage(stderr.buffer_base);
    memset(stderr.buffer_base, 0, 4096);
    stderr.buffer_end = 0;
    stderr.read_ptr = stderr.buffer_base;
    stderr.write_ptr = stderr.buffer_base;

    process->stderr = stderr;

    for (uint64 i = (uint64)address;i < (uint64)address + 64;i++)
    {
        if (*(uint64*)i == 0xAAAAAAAAAAAAAAAA)
        {
            *(uint64*)i = (uint64)&process->stdin;
        }
        else if (*(uint64*)i == 0xBBBBBBBBBBBBBBBB)
        {
            *(uint64*)i = (uint64)&process->stdout;
        }
        else if (*(uint64*)i == 0xCCCCCCCCCCCCCCCC)
        {
            *(uint64*)i = (uint64)&process->stderr;
            break;
        }
    }

    return process;
}

void SetProcessInput(int PID, uint8* buffer)
{
    Process* process = processes + PID - 1;

    FILE stdin;
    stdin.flags = 0;
    stdin.fileno = 0;
    stdin.buffer_base = buffer;
    stdin.buffer_end = 0;
    stdin.read_ptr = stdin.buffer_base;
    stdin.write_ptr = stdin.buffer_base;

    process->stdin = stdin;
}

char* GetProcessOutput(int PID)
{
    Process* process = processes + PID - 1;
    return (char*)process->stdout.buffer_base;
}

char* GetProcessError(int PID)
{
    Process* process = processes + PID - 1;
    return (char*)process->stderr.buffer_base;
}

void StartProcess(int PID, int argc, char** argv)
{
    Process* process = processes + PID - 1;

    int (*_pstart) (int, char**);
    _pstart = (int (*)(int, char**))(uint64)process->address;

    process->state = RUNNING;

    asm("pushq %rax");
    asm("pushq %rcx");
    asm("pushq %rdx");
    asm("pushq %rbx");
    asm("pushq %rsi");
    asm("pushq %rdi");

    _pstart(argc, argv);

    asm("popq %rdi");
    asm("popq %rsi");
    asm("popq %rbx");
    asm("popq %rdx");
    asm("popq %rcx");
    asm("popq %rax");
}

void PauseProcess(int PID)
{

}

void ResumeProcess(int PID)
{

}

void EndProcess(int PID)
{
    Process* process = processes + PID - 1;

    process->state = DONE;

    if (process->stdin.buffer_base != 0)
        FreePage(process->stdin.buffer_base);
    FreePage(process->stdout.buffer_base);
    FreePage(process->stderr.buffer_base);
    FreePage(process->address);

    memset(process, 0, sizeof(Process));
    processCount--;
}