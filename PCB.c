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
#include "PCBChain.h"
#include "ReadyQueue.h"
#include "TimeQueue.h"
#include "SuspendQueue.h"
#include <stdlib.h>
#include <string.h>
/************************************************************************
 OSCreatProcess
 create a process and add it to ready queue
 ************************************************************************/
void OSCreatProcess(ProcessControlBlock **p, void *identifier, char *name, int priority, int mode) {

    ProcessControlBlock *pcb = (ProcessControlBlock *)calloc(1, sizeof(ProcessControlBlock));
    pcb->next_pcb= NULL;
    memset(pcb->name, '\0', 64);
    strcpy(pcb->name, name);
    pcb->PID = ++initPID;
    pcb->timeOfDelay = 0;
    pcb->mode = mode;
    pcb->priority = priority;
    pcb->status = Waiting;

    
    CALL(Z502MakeContext(&pcb->context, (void *)identifier, USER_MODE));
    
    CALL(JoinPCBChain(firstPCBChain, pcb));
    
    READ_MODIFY(ReadyQueue_LOCK, DO_LOCK, SUSPEND_UNTIL_LOCKED, &Result);
    
    CALL(addToReadyQueue(readyQueue, pcb, sortByPrority));
    
    READ_MODIFY(ReadyQueue_LOCK, DO_UNLOCK, SUSPEND_UNTIL_LOCKED, &Result);
    
    *p = pcb;
}                                           //  End of isInPCBChain

/************************************************************************
 OSTerminateProcess
 terminate a process ,remove it from queue and free the memory
 ************************************************************************/
void OSTerminateProcess(ProcessControlBlock *pcb) {
    
    READ_MODIFY(ReadyQueue_LOCK, DO_LOCK, SUSPEND_UNTIL_LOCKED, &Result);
    
    if (removeFromReadyQueue(readyQueue, pcb) != FALSE) {
        
        READ_MODIFY(TimeQueue_LOCK, DO_LOCK, SUSPEND_UNTIL_LOCKED, &Result);
        
        CALL(removeFromTimeQueue(timeQueue, pcb));
        
        READ_MODIFY(TimeQueue_LOCK, DO_UNLOCK, SUSPEND_UNTIL_LOCKED, &Result);
    }
    
    READ_MODIFY(ReadyQueue_LOCK, DO_UNLOCK, SUSPEND_UNTIL_LOCKED, &Result);
    
    
    Z502DestroyContext(&pcb->context);
    DeleteFromPCBChain(firstPCBChain, pcb);
    free(pcb);
}                                           //  End of isInPCBChain

/************************************************************************
 OSTerminateCurrentRunning
 terminate a process that is current running ,remove it from queue switch 
 to the next availble process in the system
 ************************************************************************/
void OSTerminateCurrentRunning(){
    ProcessControlBlock *pcb;

    DeleteFromPCBChain(firstPCBChain, current_running);
    
    if (timeQueue->QueueHead != NULL && readyQueue == NULL) {
        if (dispatchProcess() == FALSE) {
            CALL(Z502Idle());
            while ( dispatchProcess() == FALSE )
                CALL();
        }
    }
    
    else if( readyQueue->QueueHead == NULL && timeQueue->QueueHead == NULL){
        Z502Halt();
    }
    
    else{
        GetFirstPCBinReadyQueue(&pcb, readyQueue);
        
        READ_MODIFY(ReadyQueue_LOCK, DO_LOCK, SUSPEND_UNTIL_LOCKED, &Result);
        
        CALL(removeFromReadyQueue(readyQueue, pcb));
        
        READ_MODIFY(ReadyQueue_LOCK, DO_UNLOCK, SUSPEND_UNTIL_LOCKED, &Result);
        
        pcb->status = Running;
        current_running = pcb;
        
        CALL(Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE, &pcb->context));
    }
}                                           //  End of OSTerminateCurrentRunning

/************************************************************************
 OSSuspendProcess
 suspend a process and add it to suspend queue
 ************************************************************************/
void OSSuspendProcess(ProcessControlBlock *pcb) {
    INT32 time, time_delay;
    int Z502_MODE;
    
    print("SUSPEND",pcb->PID);
    if (pcb->PID == current_running->PID) {
        
        CALL(addToSuspendQueue(suspendQueue, pcb, sortByNature));
        current_running = NULL;
        dispatchProcess();
    }
    else if (DectectReadyQueue(readyQueue, pcb)) {
        READ_MODIFY(ReadyQueue_LOCK, DO_LOCK, SUSPEND_UNTIL_LOCKED, &Result);
        
        CALL(removeFromReadyQueue(readyQueue, pcb));
        
        READ_MODIFY(ReadyQueue_LOCK, DO_UNLOCK, SUSPEND_UNTIL_LOCKED, &Result);
        
        CALL(addToSuspendQueue(suspendQueue, pcb, sortByNature));
        
    }
    else if (DectectTimeQueue(timeQueue, pcb)) {
        
        READ_MODIFY(TimeQueue_LOCK, DO_LOCK, SUSPEND_UNTIL_LOCKED, &Result);
        
        CALL(removeFromTimeQueue(timeQueue, pcb));
        
        CALL(addToSuspendQueue(suspendQueue, pcb, sortByNature));
        
        if (timeQueue->QueueHead != NULL) {
            GetFirstPCBinTimeQueue(&pcb, timeQueue);
            GET_TIME_OF_DAY(&time);
            time_delay = pcb->timeOfDelay - time;
            MEM_WRITE(Z502TimerStart, &time_delay);
        }
        READ_MODIFY(TimeQueue_LOCK, DO_UNLOCK, SUSPEND_UNTIL_LOCKED, &Result);
    }
    pcb->status = Suspending;
}                                           //  End of OSSuspendProcess

/************************************************************************
 OSResumeProcess
 resume a process ,remove it from suspend queue and add it to ready queue
 ************************************************************************/
void OSResumeProcess(ProcessControlBlock *pcb) {
    
    if (DectectSuspendQueue(suspendQueue, pcb) == FALSE) {
        printf("Error: The Process can't resume beacuse it's not in suspend status");
        return;
    }else {
        
        print("RESUME",pcb->PID);
        CALL(removeFromSuspendQueue(suspendQueue, pcb));
        
        READ_MODIFY(ReadyQueue_LOCK, DO_LOCK, SUSPEND_UNTIL_LOCKED, &Result);
        
        CALL((addToReadyQueue(readyQueue, pcb, sortByPrority)));
        
        READ_MODIFY(ReadyQueue_LOCK, DO_UNLOCK, SUSPEND_UNTIL_LOCKED, &Result);
    }
    pcb->status = Waiting;
}                                           //  End of OSResumeProcess

/************************************************************************
 dispatchProcess
 responsible for dispatch the process in the system
 ************************************************************************/
int dispatchProcess() {
    ProcessControlBlock *pcb;
    
    if (readyQueue->QueueHead == NULL) {
        return FALSE;
    }else {
        READ_MODIFY(ReadyQueue_LOCK, DO_LOCK, SUSPEND_UNTIL_LOCKED, &Result);
        pcb = readyQueue->QueueHead;
        print("Dispatch", pcb->PID);
        
        if (current_running) {
            
            CALL(addToReadyQueue(readyQueue, current_running, sortByPrority));
            
            current_running->status = Waiting;
        }
        
        CALL(removeFromReadyQueue(readyQueue, pcb));
        
        pcb->status = Running;
        current_running = pcb;
        
        
        READ_MODIFY(ReadyQueue_LOCK, DO_UNLOCK, SUSPEND_UNTIL_LOCKED, &Result);
        
        CALL(Z502SwitchContext(SWITCH_CONTEXT_SAVE_MODE, &pcb->context));
        
        return TRUE;
    }
}                                           //  End of dispatchProcess


