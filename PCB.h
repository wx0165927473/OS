//
//  PCB.h
//  Test0
//
//  Created by Wu Xin on 9/19/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#ifndef Test0_PCB_h
#define Test0_PCB_h
#include "global.h"

typedef struct PCB
{
    int                         PID;
    char                        name[64];
    int                         status;
    int                         mode;
    int                         priority;
    int                         timeOfDelay;
    struct         PCB          *next_pcb;
    void                        *context;
}ProcessControlBlock;

ProcessControlBlock *current_running;
int initPID;
INT32 Result;

void OSCreatProcess(ProcessControlBlock **p,void *identifier, char *name, int priority, int mode);
void OSTerminateProcess(ProcessControlBlock *pcb);
void OSTerminateCurrentRunning();
void OSSuspendProcess(ProcessControlBlock *pcb);
void OSResumeProcess(ProcessControlBlock *pcb);
int dispatchProcess();
#endif
