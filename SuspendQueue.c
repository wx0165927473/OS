//
//  SuspendQueue.c
//  OS
//
//  Created by Wu Xin on 10/13/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#include <stdio.h>
#include "SuspendQueue.h"
#include "global.h"
/************************************************************************
 addToSuspendQueue
 add the process to suspend queue after the process is suspended
 ************************************************************************/
void addToSuspendQueue(SuspendQueue *queue, ProcessControlBlock *PCB, int sortOrder) {
    
    if (DectectSuspendQueue (queue, PCB)) {
        return;
    }
    
    if (queue == NULL) {
        printf("Suspend Queue not init yet");
        return;
    }
    
    ProcessControlBlock *previousPCB = NULL;
    ProcessControlBlock *p = queue->QueueHead;
    
    if (queue->QueueHead == NULL) {
        queue->QueueHead = PCB;
    }
    else {
        if (sortOrder == sortByPrority) {
            while ( p != NULL && p->priority <= PCB->priority) {
                previousPCB = p;
                p = p->next_pcb;
            }
            previousPCB->next_pcb = PCB;
            PCB->next_pcb = p;
        }
        
        else {
            while (p) {
                previousPCB = p;
                p = p->next_pcb;
            }
        }
        
        if (previousPCB == NULL) {
            queue->QueueHead = PCB;
            PCB->next_pcb = p;
        }else {
            previousPCB->next_pcb = PCB;
            PCB->next_pcb = p;
        }
    }
    queue->length++;
}                                       // End of addToTimeQueue

/************************************************************************
 removeFromSuspendQueue
 remove the process from suspend queue after the process is resumed
 ************************************************************************/
BOOL removeFromSuspendQueue(SuspendQueue *queue, ProcessControlBlock *PCB) {
    
    ProcessControlBlock *p = queue->QueueHead;
    ProcessControlBlock *prePCB = NULL;
    
    if (queue->QueueHead == NULL || queue->length == 0) {
        printf("Empty Suspend Queue!\n");
        return -1;
    }
    
    while (p) {
        
        if (p == PCB) {
            if (queue->QueueHead == PCB) {
                queue->QueueHead = PCB->next_pcb;
                PCB->next_pcb = NULL;
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
}                                       // End of removeFromSuspendQueue

/************************************************************************
 DectectSuspendQueue
 find the specific process whether in or not in the suspend queue
 by process ID
 ************************************************************************/
BOOL DectectSuspendQueue(SuspendQueue *queue, ProcessControlBlock *pcb){
    
    ProcessControlBlock *p = queue->QueueHead;
    
    while( p != NULL ){
        if( p->PID == pcb->PID )
            return TRUE;
        p = p->next_pcb;
    }
    return FALSE;
}                                       // End of DectectSuspendQueue

/************************************************************************
 GetFirstPCBinSuspendQueue
 find the first process in the suspend queue
 ************************************************************************/
void GetFirstPCBinSuspendQueue( ProcessControlBlock **pcb, SuspendQueue *queue ){
    
    ProcessControlBlock *p = queue->QueueHead;
    if( queue == NULL ){
        printf("Suspend Queue is not init yet!");
        return;
    }
    while( p->next_pcb != NULL )
        p = p->next_pcb;
    
    *pcb = p;
}                                       // End of GetFirstPCBinSuspendQueue