//
//  SuspendQueue.h
//  OS
//
//  Created by Wu Xin on 10/13/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#ifndef OS_SuspendQueue_h
#define OS_SuspendQueue_h
#include "global.h"
#include "PCB.h"

typedef struct
{
    int length;
    ProcessControlBlock *QueueHead;
} SuspendQueue;

SuspendQueue           *suspendQueue;

void addToSuspendQueue(SuspendQueue *queue, ProcessControlBlock *PCB, int sortOrder);
BOOL removeFromSuspendQueue(SuspendQueue *queue, ProcessControlBlock *PCB);
BOOL DectectSuspendQueue(SuspendQueue *queue, ProcessControlBlock *pcb);
void GetFirstPCBinSuspendQueue(ProcessControlBlock **pcb, SuspendQueue *queue);


#endif
