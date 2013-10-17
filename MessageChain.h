//
//  MessageChain.h
//  OS
//
//  Created by Wu Xin on 10/15/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#ifndef OS_MessageChain_h
#define OS_MessageChain_h
#include "Message.h"

typedef struct MsgNode{
    Message *msg;
    struct MsgNode *next;
}MsgNode, *MessageChain;

MessageChain firstMessageChain;

MessageChain initMessageChain();
void JoinMessageChain(MessageChain msgChain, Message *msg);
void DeleteFromMessageChain(MessageChain msgChain, Message *msg);
Message *isInMessageChain(MessageChain msgChain, int sourceMsgID, int targetMsgID);
int get_length_MessageChain(MessageChain msgChain);
#endif
