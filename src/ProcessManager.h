#pragma once
#include "Typedefs.h"
#include "Memory/PageFrameAllocator.h"
#include "kstring.h"
#include "TextMode.h"
#include "kconvert.h"

#define NOT_RESPONDING -1
#define DONE 0 
#define PAUSED 1
#define RUNNING 2

struct Process
{
    int PID;
    int state;
    void* address;

    FILE stdin;
    FILE stdout;
    FILE stderr;
};

extern Process* processes;
extern int processCount;

void InitializeProcessManager();

Process* BuildProcess(void* address);

void SetProcessInput(int PID, uint8* buffer);
char* GetProcessOutput(int PID);
char* GetProcessError(int PID);

void StartProcess(int PID, int argc, char** argv);
void PauseProcess(int PID);
void ResumeProcess(int PID);
void EndProcess(int PID);