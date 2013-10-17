//
//  ReadyQueue.c
//  GitOS
//
//  Created by Wu Xin on 10/3/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#include <stdio.h>
#include "ReadyQueue.h"
#include "global.h"
/************************************************************************
 getLastPCB
 get the last process in the ready queue
 ************************************************************************/
ProcessControlBlock *getLastPCB(ReadyQueue *queue) {
    ProcessControlBlock *p = queue->QueueHead;
    if (queue == NULL) {
        printf("Ready Queue not init yet");
    }
    
    
    while (p->next_pcb) {
        p = p->next_pcb;
    }
    
    return p;
}                                       // End of getLastPCB

/************************************************************************
 addToReadyQueue
 add the process to ready queue by priority when the process is ready to go
 ************************************************************************/
void addToReadyQueue(ReadyQueue *queue, ProcessControlBlock *PCB, int sortOrder) {

    if (DectectReadyQueue(queue, PCB)) {
        return;
    }
    
    if (queue == NULL) {
        printf("Ready Queue not init yet");
        return;
    }
    
    ProcessControlBlock *previousPCB = NULL;
    ProcessControlBlock *p = queue->QueueHead;
    
    if (queue->QueueHead == NULL) {
        queue->QueueHead = PCB;
    }
    else {
        if (sortOrder == sortByPrority) {
            if (p->priority > PCB->priority) {
                queue->QueueHead = PCB;
                PCB->next_pcb = p;
            }else {
            
            while ( p != NULL && p->priority <= PCB->priority) {
                previousPCB = p;
                p = p->next_pcb;
            }
            previousPCB->next_pcb = PCB;
            PCB->next_pcb = p;
            
            }
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
}                                       // End of addToReadyQueue

/************************************************************************
 removeFromReadyQueue
 remove the process from ready queue when it ready to run
 ************************************************************************/
BOOL removeFromReadyQueue(ReadyQueue *queue, ProcessControlBlock *PCB) {
    
    ProcessControlBlock *p = queue->QueueHead;
    ProcessControlBlock *prePCB = NULL;
    
    if (queue->QueueHead == NULL || queue->length == 0) {
        printf("Empty Ready Queue!\n");
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
}                                       // End of removeFromReadyQueue

/************************************************************************
 DectectReadyQueue
 find the specific process whether in or not in the ready queue 
 by process ID
 ************************************************************************/
BOOL DectectReadyQueue(ReadyQueue *queue, ProcessControlBlock *pcb){
    
    ProcessControlBlock *p = queue->QueueHead;
    
    while( p != NULL ){
        if( p->PID == pcb->PID )
            return TRUE;
        p = p->next_pcb;
    }
    return FALSE;
}                                       // End of DectectReadyQueue

/************************************************************************
 GetFirstPCBinReadyQueue
 find the first process in the ready queue
 ************************************************************************/
void GetFirstPCBinReadyQueue( ProcessControlBlock **pcb, ReadyQueue *queue ){
    
    ProcessControlBlock *p = queue->QueueHead;
    if( queue == NULL ){
        printf("Time Queue is not init yet!");
        return;
    }
    while( p->next_pcb != NULL )
        p = p->next_pcb;
    
    *pcb = p;
}