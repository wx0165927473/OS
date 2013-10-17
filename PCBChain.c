//
//  PCBChain.c
//  OS
//
//  Created by Wu Xin on 10/6/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#include <stdio.h>
#include "PCBChain.h"
#include "global.h"
#include <string.h>
#include <stdlib.h>
/************************************************************************
 initPCBChain
 init the PCB Chain
 ************************************************************************/
PCBChain initPCBChain() {
    PCBChain pcbChain = calloc(1,sizeof(PCBChain));
    pcbChain->next = NULL;
    pcbChain->pcb = NULL;
    return pcbChain;
}                                           //  End of initPCBChain

/************************************************************************
 checkError
 check if there is a error before process creat
 ************************************************************************/
int checkError(char *name, int priority) {
    int length;
    if (priority < 0) {
        return ERR_ILLEGAL_PRIORITY;
    }
    
    if ((length = get_length(firstPCBChain)) >= 20) {
        return ERR_OVERFLOW_THREAD;
    }
    
    if (compareName(firstPCBChain, name) != NULL) {
        return ERR_DUPLICATE_NAME;
    }
    
    return ERR_SUCCESS;
}                                            //  End of initPCBChain

/************************************************************************
 get_length
 get the length of the PCBChain to find how many processes in the system
 ************************************************************************/
int get_length(PCBChain pcbChain) {
    
    int length =0;
    PCBChain chain;
    chain = pcbChain;
    
    while (chain) {
        length++;
        chain = chain->next;
    }
    
    return length;
}                                           //  End of get_length

/************************************************************************
 getLastNode
 get the last PCB in the PCB Chain and return a PCB Node.
 ************************************************************************/
PCBChain getLastNode(PCBChain pcbChain) {
    PCBChain chain = pcbChain;
    if (chain == NULL) {
        printf("PCB Chain not init yet");
    }

    while (chain) {
        if (chain->next == NULL) {
            break;
        }
        
        chain = chain->next;
    }
    
    return chain;
}                                           //  End of getLastNode

/************************************************************************
 compareName
 find a specific PCB by giving PCB name in PCB Chain
 ************************************************************************/
PCBChain compareName(PCBChain pcbChain, char *name) {
    PCBChain chain;
    ProcessControlBlock *p;
    chain = pcbChain;
    
    while (chain!= NULL) {
        p = chain->pcb;
        
        if (p!= NULL && (strcmp(p->name, name) == 0)) {
            return chain;
        }
        chain = chain->next;
    }
    return NULL;
}                                           //  End of compareName

/************************************************************************
 JoinPCBChain
 add the PCB to PCB Chain after create it
 ************************************************************************/
void JoinPCBChain(PCBChain pcbChain, ProcessControlBlock *pcb) {
    PCBChain InsertNode = calloc(1, sizeof(PCBNode));
    PCBChain chain = getLastNode(pcbChain);
    
    InsertNode->pcb = pcb;
    InsertNode->next = NULL;
    
    
    chain->next = InsertNode;
    
    printf("Add '%s' to Chain. Length of Chain = %d----------\n", pcb->name, get_length(pcbChain));
}                                           //  End of JoinPCBChain

/************************************************************************
 DeleteFromPCBChain
 delete the PCB from PCB Chain after the process is terminated
 ************************************************************************/
void DeleteFromPCBChain(PCBChain pcbChain, ProcessControlBlock *pcb) {
    PCBChain pre, chain;
    ProcessControlBlock *p;
    chain = pcbChain;
    
    while (chain!= NULL) {
        p = chain->pcb;
        
        if (p!= NULL && p->PID == pcb->PID) {
            pre->next = chain->next;
            free(chain);
            break;
        }
        
        pre = chain;
        chain = chain->next;
    }
    printf("Delete '%s' to Chain. Length of Chain = %d----------\n", pcb->name, get_length(pcbChain));
}                                           //  End of DeleteFromPCBChain

/************************************************************************
 isInPCBChain
 find a specific PCB by giving PCB ID in PCB Chain, return a PCB
 ************************************************************************/
ProcessControlBlock *isInPCBChain(PCBChain pcbChain, int PID) {
    ProcessControlBlock *p;
    PCBChain chain = pcbChain->next;
    
    
    while (chain != NULL) {
        p = chain->pcb;
        
        if (p->PID == PID) {
            return p;
        }
        chain = chain->next;
    }
    return NULL;
}                                           //  End of isInPCBChain