//
//  MessageChain.c
//  OS
//
//  Created by Wu Xin on 10/15/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "Message.h"
#include "MessageChain.h"
/************************************************************************
 initMessageChain
 init the message Chain
 ************************************************************************/
MessageChain initMessageChain() {
    MessageChain messageChain = calloc(1,sizeof(MessageChain));
    messageChain->next = NULL;
    messageChain->msg = NULL;
    return messageChain;
}                                       //  End of initPCBChain

/************************************************************************
 getLastNode
 get the last message in the message Chain and return a message Node.
 ************************************************************************/
MessageChain getLastNode_MessageChain(MessageChain msgChain) {
    MessageChain chain = msgChain;
    if (chain == NULL) {
        printf("Message Chain not init yet");
    }
    
    while (chain) {
        if (chain->next == NULL) {
            break;
        }
        
        chain = chain->next;
    }
    
    return chain;
}                                       //  End of getLastNode_MessageChain

/************************************************************************
 get_length
 get the length of the MessageChain to find how many messages in the 
 MessageChain
 ************************************************************************/
int get_length_MessageChain(MessageChain msgChain) {
    
    int length =0;
    MessageChain chain;
    chain = msgChain;
    
    while (chain) {
        length++;
        chain = chain->next;
    }
    
    return length;
}                                       //  End of get_length_MessageChain

/************************************************************************
 JoinMessageChain
 add the message to message Chain after create it
 ************************************************************************/
void JoinMessageChain(MessageChain msgChain, Message *msg) {
    MessageChain InsertNode = calloc(1, sizeof(MsgNode));
    MessageChain chain = getLastNode_MessageChain(msgChain);
    
    InsertNode->msg = msg;
    InsertNode->next = NULL;
    
    
    chain->next = InsertNode;
    
    printf("Add '%s' to Chain. Length of Chain = %d----------\n", msg->msg_buff, get_length_MessageChain(msgChain));
}                                       //  End of JoinMessageChain

/************************************************************************
 DeleteFromMessageChain
 delete the message from Message Chain after the message has been received
 ************************************************************************/
void DeleteFromMessageChain(MessageChain msgChain, Message *msg) {
    MessageChain pre, chain;
    Message *m;
    chain = msgChain;
    
    while (chain!= NULL) {
        m = chain->msg;
        
        if (m!= NULL && m->MsgID == msg->MsgID) {
            pre->next = chain->next;
            free(chain);
            break;
        }
        
        pre = chain;
        chain = chain->next;
    }
    
    printf("Delete '%s' to Chain. Length of Chain = %d----------\n", msg->msg_buff, get_length_MessageChain(msgChain));
}                                       //  End of DeleteFromMessageChain

/************************************************************************
 isInMessageChain
 find a specific message by giving source message ID and target message ID
 in Message Chain, return a message
 ************************************************************************/
Message *isInMessageChain(MessageChain msgChain, int sourceMsgID, int targetMsgID) {
    Message *m;
    MessageChain chain = msgChain->next;
    
    while (chain != NULL) {
        m = chain->msg;
        
        if (sourceMsgID == m->source_pid || m->source_pid == -1 || sourceMsgID == -1) {
            if (targetMsgID == m->target_pid ||m->target_pid == -1) {
                return m;
            }
        }
        chain = chain->next;
    }
    return NULL;
}                                       //  End of isInMessageChain