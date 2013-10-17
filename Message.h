//
//  Message.h
//  OS
//
//  Created by Wu Xin on 10/15/13.
//  Copyright (c) 2013 Wu Xin. All rights reserved.
//

#ifndef OS_Message_h
#define OS_Message_h
typedef struct Message
{
    int         MsgID;
    int         target_pid;
    int         source_pid;
    int         length;
    char        msg_buff[64];
} Message;

int initMsgID;

Message *CreateMessage(int source_pid, int target_pid, char *msg_buff, int length);
#endif
