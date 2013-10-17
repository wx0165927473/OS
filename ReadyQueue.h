//
//  ReadyQueue.h
//  GitOS
//
//  Created by Wu Xin on 10/3/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#ifndef GitOS_ReadyQueue_h
#define GitOS_ReadyQueue_h
#include "PCB.h"
#include "global.h"

typedef struct
{
    int length;
    ProcessControlBlock *QueueHead;
    
} ReadyQueue;

ReadyQueue           *readyQueue;

void addToReadyQueue(ReadyQueue *queue, ProcessControlBlock *PCB, int sortOrder);
BOOL removeFromReadyQueue(ReadyQueue *queue, ProcessControlBlock *PCB);
BOOL DectectReadyQueue(ReadyQueue *queue, ProcessControlBlock *pcb);
void GetFirstPCBinReadyQueue(ProcessControlBlock **pcb, ReadyQueue *queue);
#endif
