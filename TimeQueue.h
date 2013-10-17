//
//  TimeQueue.h
//  GitOS
//
//  Created by Wu Xin on 10/3/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#ifndef GitOS_TimeQueue_h
#define GitOS_TimeQueue_h
#include "PCB.h"
#include "global.h"

typedef struct
{
    ProcessControlBlock *QueueHead;
    int length;
    
} TimeQueue;

TimeQueue            *timeQueue;

void addToTimeQueue(TimeQueue *queue, ProcessControlBlock *PCB);
BOOL removeFromTimeQueue(TimeQueue *queue, ProcessControlBlock *PCB);
void GetFirstPCBinTimeQueue(ProcessControlBlock **pcb, TimeQueue *queue);
BOOL DectectTimeQueue(TimeQueue *queue, ProcessControlBlock *pcb);

#endif
