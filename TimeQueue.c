//
//  TimeQueue.c
//  GitOS
//
//  Created by Wu Xin on 10/3/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#include <stdio.h>
#include "TimeQueue.h"
#include "global.h"
#include "syscalls.h"
/************************************************************************
 addToTimeQueue
 add the process to time queue by the delay time to wait to be awake
 ************************************************************************/
void addToTimeQueue(TimeQueue *queue, ProcessControlBlock *PCB) {
    
    if (queue == NULL) {
        printf("Time Queue not init yet");
        return;
    }
    
    if( DectectTimeQueue(queue, PCB)){
        return;
    }
    
    ProcessControlBlock *p = queue->QueueHead;
    ProcessControlBlock *previousPCB = NULL;
    
    if (queue->QueueHead == NULL) {
        queue->QueueHead = PCB;
    }
    else {
        while ( p!= NULL && p->timeOfDelay < PCB->timeOfDelay) {
            previousPCB = p;
            p = p->next_pcb;
        }
        
        if (previousPCB == NULL) {
            queue->QueueHead= PCB;
            PCB->next_pcb = p;
        }else {
            previousPCB->next_pcb = PCB;
            PCB->next_pcb = p;
        }
    }
    queue->length++;
}                                       // End of addToTimeQueue

/************************************************************************
 addToTimeQueue
 remove the process from time queue
 ************************************************************************/
BOOL removeFromTimeQueue(TimeQueue *queue, ProcessControlBlock *PCB) {
    ProcessControlBlock *p = queue->QueueHead;
    ProcessControlBlock *prePCB = NULL;
    
    if (queue->QueueHead == NULL) {
        printf("Empty Time Queue!\n");
        return -1;
    }
    
    while (p) {
        
        if (p == PCB) {
            if (queue->QueueHead == p) {
                queue->QueueHead = p->next_pcb;
                p->next_pcb = NULL;
            }
            else if (p->next_pcb == NULL) {
                prePCB->next_pcb = NULL;
            }
            else {
                prePCB->next_pcb = p->next_pcb;
                p->next_pcb = NULL;
            }
            
            queue->length--;
            return TRUE;
        }
        
        prePCB = p;
        p = p->next_pcb;
    }
    return FALSE;
}                                       // End of addToTimeQueue

/************************************************************************
 GetFirstPCBinTimeQueue
 find the first process in the time queue
 ************************************************************************/
void GetFirstPCBinTimeQueue( ProcessControlBlock **pcb, TimeQueue *queue ){
    
    ProcessControlBlock *p = queue->QueueHead;
    if( queue == NULL ){
        printf("Time Queue is not init yet!");
        return;
    }
    while( p->next_pcb != NULL )
        p = p->next_pcb;
    
    *pcb = p;
}                                       // End of GetFirstPCBinTimeQueue

/************************************************************************
 DectectTimeQueue
 find the specific process whether in or not in the time queue
 by process ID
 ************************************************************************/
BOOL DectectTimeQueue( TimeQueue *queue, ProcessControlBlock *pcb ){
    
    ProcessControlBlock *p = queue->QueueHead;
    
    while( p != NULL ){
        if( p->PID == pcb->PID )
            return TRUE;
        p = p->next_pcb;
    }
    return FALSE;
}                                       // End of DectectTimeQueue