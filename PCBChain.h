//
//  PCBChain.h
//  OS
//
//  Created by Wu Xin on 10/6/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#ifndef OS_PCBChain_h
#define OS_PCBChain_h
#include "PCB.h"

typedef struct PCBNode{
    ProcessControlBlock *pcb;
    struct PCBNode *next;
}PCBNode, *PCBChain;

PCBChain firstPCBChain;

PCBChain initPCBChain();
int checkError(char *name, int priority);
int get_length(PCBChain pcbChain);
PCBChain compareName(PCBChain pcbChain, char *name);
void JoinPCBChain(PCBChain pcbChain, ProcessControlBlock *pcb);
void DeleteFromPCBChain(PCBChain pcbChain, ProcessControlBlock *pcb);
ProcessControlBlock *isInPCBChain(PCBChain pcbChain, int PID);
#endif
