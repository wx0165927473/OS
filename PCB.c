//
//  PCB.c
//  Test0
//
//  Created by Wu Xin on 9/19/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#include <stdio.h>
#include "PCB.h"
#include "global.h"
#include "protos.h"
#include "syscalls.h"

ProcessControlBlock *OSCreatProcess(void *identifier, int priority, int mode) {
    int initPID = 0;

    ProcessControlBlock *pcb = (ProcessControlBlock *)calloc(1, sizeof(ProcessControlBlock));
    pcb->next_point = NULL;
    pcb->PID = initPID++;
    pcb->mode = mode; //0 == UserMode 1 == KernelMode
    pcb->proprity = priority;
    pcb->status = Waiting; //1 == running 0 == idle
    pcb->timeOfDelay = 100;

    Z502MakeContext(&pcb->context, (void *)identifier, USER_MODE);
    Z502SwitchContext(SWITCH_CONTEXT_KILL_MODE, &pcb->context);
    return pcb;
}
