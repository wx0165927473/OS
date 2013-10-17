//
//  Message.c
//  OS
//
//  Created by Wu Xin on 10/15/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Message.h"
/************************************************************************
 CreateMessage
 create a message that contain all necessary information
 ************************************************************************/
Message *CreateMessage(int source_pid, int target_pid, char *msg_buff, int length) {
    Message *msg = (Message *)calloc(1, sizeof(Message));
    
    memset(msg->msg_buff, 0, 64);
    strcpy(msg->msg_buff, msg_buff);
    msg->MsgID = ++initMsgID;
    msg->source_pid = source_pid;
    msg->target_pid = target_pid;
    msg->length = length;
    
    return msg;
}                                   // End of CreateMessage