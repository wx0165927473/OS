/************************************************************************

        This code forms the base of the operating system you will
        build.  It has only the barest rudiments of what you will
        eventually construct; yet it contains the interfaces that
        allow test.c and z502.c to be successfully built together.

        Revision History:
        1.0 August 1990
        1.1 December 1990: Portability attempted.
        1.3 July     1992: More Portability enhancements.
                           Add call to sample_code.
        1.4 December 1992: Limit (temporarily) printout in
                           interrupt handler.  More portability.
        2.0 January  2000: A number of small changes.
        2.1 May      2001: Bug fixes and clear STAT_VECTOR
        2.2 July     2002: Make code appropriate for undergrads.
                           Default program start is in test0.
        3.0 August   2004: Modified to support memory mapped IO
        3.1 August   2004: hardware interrupt runs on separate thread
        3.11 August  2004: Support for OS level locking
	4.0  July    2013: Major portions rewritten to support multiple threads
************************************************************************/

#include             "global.h"
#include             "syscalls.h"
#include             "protos.h"
#include             "string.h"
#include             "z502.h"
#include             "PCB.h"
#include             "TimeQueue.h"
#include             "ReadyQueue.h"
#include             "SuspendQueue.h"
#include             "PCBChain.h"
#include             "Message.h"
#include             "MessageChain.h"


// These loacations are global and define information about the page table
extern UINT16        *Z502_PAGE_TBL_ADDR;
extern INT16         Z502_PAGE_TBL_LENGTH;

extern void          *TO_VECTOR [];

char                 *call_names[] = { "mem_read ", "mem_write",
                            "read_mod ", "get_time ", "sleep    ",
                            "get_pid  ", "create   ", "term_proc",
                            "suspend  ", "resume   ", "ch_prior ",
                            "send     ", "receive  ", "disk_read",
                            "disk_wrt ", "def_sh_ar" };
int                  state_printer;

/************************************************************************
 initTestProcess
 Create a test process by which you entered name of test
 ************************************************************************/
void initTestProcess(char *name,ProcessControlBlock *pcb) {
    ProcessControlBlock *p;
    p = pcb;
    
    if (strcmp(name, "test0") == 0) {
        OSCreatProcess(&p, (void *)test0,"test0",0,USER_MODE);
        state_printer = FALSE;
    }else if(strcmp(name, "test1a") == 0) {
        OSCreatProcess(&p, (void *)test1a,"test1a",0,USER_MODE);
        state_printer = FALSE;
    }else if(strcmp(name, "test1b") == 0) {
        OSCreatProcess(&p, (void *)test1b,"test1b",0,USER_MODE);
        state_printer = FALSE;
    }else if(strcmp(name, "test1c") == 0) {
        OSCreatProcess(&p, (void *)test1c,"test1c",0,USER_MODE);
        state_printer = TRUE;
    }else if(strcmp(name, "test1d") == 0) {
        OSCreatProcess(&p, (void *)test1d,"test1d",0,USER_MODE);
        state_printer = TRUE;
    }else if(strcmp(name, "test1e") == 0) {
        OSCreatProcess(&p, (void *)test1e,"test1e",0,USER_MODE);
        state_printer = FALSE;
    }else if(strcmp(name, "test1f") == 0) {
        OSCreatProcess(&p, (void *)test1f,"test1f",0,USER_MODE);
        state_printer = TRUE;
    }else if(strcmp(name, "test1g") == 0) {
        OSCreatProcess(&p, (void *)test1g,"test1g",0,USER_MODE);
        state_printer = FALSE;
    }else if(strcmp(name, "test1h") == 0) {
        OSCreatProcess(&p, (void *)test1h,"test1h",0,USER_MODE);
        state_printer = TRUE;
    }else if(strcmp(name, "test1i") == 0) {
        OSCreatProcess(&p, (void *)test1i,"test1i",0,USER_MODE);
        state_printer = FALSE;
    }else if(strcmp(name, "test1j") == 0) {
        OSCreatProcess(&p, (void *)test1j,"test1j",0,USER_MODE);
        state_printer = TRUE;
    }else if(strcmp(name, "test1k") == 0) {
        OSCreatProcess(&p, (void *)test1k,"test1k",0,USER_MODE);
        state_printer = FALSE;
    }
}

/************************************************************************
 PRINT
 Print everything to observe how the system scheduling the process
 ************************************************************************/
void print(char *callname, int PID) {
    if (state_printer == FALSE) {
        return;
    }
    INT32                       time;
    int                         Z502_MODE;
    ProcessControlBlock         *pcb;
    
    GET_TIME_OF_DAY(&time);
    CALL(SP_setup(SP_TIME_MODE, time));
    
    CALL(SP_setup(SP_TARGET_MODE, PID));
    
    CALL(SP_setup_action(SP_ACTION_MODE, callname));
   
    if (current_running) {
        CALL(SP_setup(SP_RUNNING_MODE, current_running->PID));
    }
    
    pcb = readyQueue->QueueHead;
    while (pcb) {
        CALL(SP_setup(SP_READY_MODE, pcb->PID));
        pcb = pcb->next_pcb;
    }
    
    pcb = timeQueue->QueueHead;
    while (pcb) {
        CALL(SP_setup(SP_WAITING_MODE, pcb->PID));
        pcb = pcb->next_pcb;
    }
    
    pcb = suspendQueue->QueueHead;
    while (pcb) {
        CALL(SP_setup(SP_SUSPENDED_MODE, pcb->PID));
        pcb = pcb->next_pcb;
    }
   
	CALL(SP_print_header());
	CALL(SP_print_line());
}                                       /* End of interrupt_handler */
/************************************************************************
    INTERRUPT_HANDLER
        When the Z502 gets a hardware interrupt, it transfers control to
        this routine in the OS.
************************************************************************/
void    interrupt_handler( void ) {
    INT32              device_id;
    INT32              status;
    INT32              Index = 0;
    ProcessControlBlock *p;
    INT32              time, Result;
    short              Z502_MODE;
    INT32       time_delay;
    ProcessControlBlock *pcb;
    
    // Get cause of interrupt
    MEM_READ(Z502InterruptDevice, &device_id );
    // Set this device as target of our query
    MEM_WRITE(Z502InterruptDevice, &device_id );
    // Now read the status of this device
    MEM_READ(Z502InterruptStatus, &status );

    p = timeQueue->QueueHead;
    
    READ_MODIFY(TimeQueue_LOCK, DO_LOCK, SUSPEND_UNTIL_LOCKED, &Result);
    
    while (TRUE) {
        GET_TIME_OF_DAY(&time);
        if (p != NULL && time > p->timeOfDelay) {
            
            removeFromTimeQueue(timeQueue, p);
            
            READ_MODIFY(ReadyQueue_LOCK, DO_LOCK, SUSPEND_UNTIL_LOCKED, &Result);
            
            addToReadyQueue(readyQueue, p, sortByPrority);
            
            READ_MODIFY(ReadyQueue_LOCK, DO_UNLOCK, SUSPEND_UNTIL_LOCKED, &Result);
            
            p = timeQueue->QueueHead;
        }else
            break;
    }
    
    if (timeQueue->QueueHead != NULL) {
        GetFirstPCBinTimeQueue(&pcb, timeQueue);
        GET_TIME_OF_DAY(&time);
        time_delay = pcb->timeOfDelay - time;
        MEM_WRITE(Z502TimerStart, &time_delay);
    }
    
    READ_MODIFY(TimeQueue_LOCK, DO_UNLOCK, SUSPEND_UNTIL_LOCKED, &Result);
    
    // Clear out this device - we're done with it
    MEM_WRITE(Z502InterruptClear, &Index );
}                                       /* End of interrupt_handler */
/************************************************************************
    FAULT_HANDLER
        The beginning of the OS502.  Used to receive hardware faults.
************************************************************************/

void    fault_handler( void )
    {
    INT32       device_id;
    INT32       status;
    INT32       Index = 0;


    // Get cause of interrupt
    MEM_READ(Z502InterruptDevice, &device_id );
    // Set this device as target of our query
    MEM_WRITE(Z502InterruptDevice, &device_id );
    // Now read the status of this device
    MEM_READ(Z502InterruptStatus, &status );

    printf( "Fault_handler: Found vector type %d with value %d\n",
                        device_id, status );
    // Clear out this device - we're done with it
    MEM_WRITE(Z502InterruptClear, &Index );
}                                       /* End of fault_handler */
/************************************************************************
 START_TIMER
 When the system get a SYSNUM_SLEEP call, the start_timer will be called
 to put the process into time queue
 ************************************************************************/
void start_timer(SYSTEM_CALL_DATA *SystemCallData) {
    INT32 time, time_delay,Result;
    INT16 Z502_MODE;
    ProcessControlBlock *pcb;
    
    print("Sleep", current_running->PID);

    READ_MODIFY(TimeQueue_LOCK, DO_LOCK, SUSPEND_UNTIL_LOCKED, &Result);
    
    GET_TIME_OF_DAY(&time);
    time_delay = (INT32)SystemCallData->Argument[0];
    current_running->status = Waiting;
    current_running->timeOfDelay = time + time_delay;
	
    addToTimeQueue(timeQueue, current_running);
    
    current_running = NULL;
    
    if (timeQueue->QueueHead != NULL) {
        GetFirstPCBinTimeQueue(&pcb, timeQueue);
        GET_TIME_OF_DAY(&time);
        time_delay = pcb->timeOfDelay - time;
        MEM_WRITE(Z502TimerStart, &time_delay);
    }
    
    READ_MODIFY(TimeQueue_LOCK, DO_UNLOCK, SUSPEND_UNTIL_LOCKED, &Result);
    
    if (dispatchProcess() == FALSE) {
        CALL(Z502Idle());
        while ( dispatchProcess() == FALSE )
            CALL();
    }
}                                                // End of start_timer
/************************************************************************
 SVC
 The beginning of the OS502.  Used to receive software interrupts.
 All system calls come to this point in the code and are to be
 handled by the student written code here.
 The variable do_print is designed to print out the data for the
 incoming calls, but does so only for the first ten calls.  This
 allows the user to see what's happening, but doesn't overwhelm
 with the amount of data.
 ************************************************************************/
void    svc( SYSTEM_CALL_DATA *SystemCallData ) {
    short               call_type;
    static short        do_print = 100;
    short               i;
    INT32               Time;
    int                 priority, errorCode, PID, msg_buffer_size;
    char                *name, *msg;
    ProcessControlBlock *pcb = NULL;
    void                *address;
    PCBChain            temChain;
    Message             *message;

    call_type = (short)SystemCallData->SystemCallNumber;
    if ( do_print > 0 ) {
        printf( "SVC handler: %s\n", call_names[call_type]);
        for (i = 0; i < SystemCallData->NumberOfArguments - 1; i++ ){
//        	 Value = (long)*SystemCallData->Argument[i];
             printf( "Arg %d: Contents = (Decimal) %8ld,  (Hex) %8lX\n", i,
             (unsigned long )SystemCallData->Argument[i],
             (unsigned long )SystemCallData->Argument[i]);
        }
    }
    do_print--;

        switch (call_type) {
            case SYSNUM_GET_TIME_OF_DAY:
                Z502MemoryRead(Z502ClockStatus, &Time);
                *(int *)SystemCallData->Argument[0] = Time;
                break;

            case SYSNUM_TERMINATE_PROCESS:
                
                PID = (int)SystemCallData->Argument[0];
                
                if (PID == -1) {
                    OSTerminateCurrentRunning();
                }
                
                if (PID == -2) {
                    DeleteFromPCBChain(firstPCBChain, current_running);
                    Z502Halt();
                }
        
                pcb = isInPCBChain(firstPCBChain, PID);
                if (pcb == NULL) {
                    *(int *)SystemCallData->Argument[4] = ERR_PROCESS_NOT_EXIST;
                    break;
                }
                
                OSTerminateProcess(pcb);
                *(long *)SystemCallData->Argument[1] = ERR_SUCCESS;
                break;

            case SYSNUM_SLEEP:

                //svc hands control of the SLEEP request to start_timer, a routine YOU write.
                //start_timer, enqueues the PCB of the running process onto the timer_queue.
                
                CALL(start_timer(SystemCallData));
                break;
                
            case SYSNUM_CREATE_PROCESS:
                
                name = (char *)SystemCallData->Argument[0];
                address = SystemCallData->Argument[1];
                priority = (int)SystemCallData->Argument[2];
                
                if ((errorCode = checkError(name, priority)) == ERR_SUCCESS) {
                    CALL(OSCreatProcess(&pcb, address, name, priority, USER_MODE));
                    *(int *)SystemCallData->Argument[3] = pcb->PID;
                    print("Creat", pcb->PID);
                }
                
                *(int *)SystemCallData->Argument[4] = errorCode;
                break;
                
            case SYSNUM_GET_PROCESS_ID:
                
                name = (char *)SystemCallData->Argument[0];
                if( strcmp( name, "" ) == 0 ){
                    pcb = current_running;
                }
                else if ((temChain = compareName(firstPCBChain ,name))) {
                    pcb = temChain->pcb;
                }
                else {
                    *(int *)SystemCallData->Argument[2] = ERR_NO_SUCH_PID;
                    break;
                }
                
                *(int *)SystemCallData->Argument[1] = pcb->PID;
                *(long *)SystemCallData->Argument[2] = ERR_SUCCESS;
                break;
                
            case SYSNUM_SUSPEND_PROCESS:
                PID = (int )SystemCallData->Argument[0];
                pcb = isInPCBChain(firstPCBChain, PID);
                
                if (PID == 9999) {
                    *(long *)SystemCallData->Argument[1] = ERR_ILLEGAL_PRIORITY;
                    break;
                }else if (DectectSuspendQueue(suspendQueue, pcb)) {
                    *(long *)SystemCallData->Argument[1] = ERR_DUPLICATE_SUSPEND;
                    break;
                }else if(PID == -1){
                    *(long *)SystemCallData->Argument[1] = ERR_SUSPEND_OURSELF;
                    break;
                }
                else {
                    OSSuspendProcess(pcb);
                    *(long *)SystemCallData->Argument[1] = ERR_SUCCESS;
                    break;
                }
                
            case SYSNUM_RESUME_PROCESS:
                PID = (int)SystemCallData->Argument[0];
                pcb = isInPCBChain(firstPCBChain, PID);
                
                if (PID == 9999) {
                    *(long *)SystemCallData->Argument[1] = ERR_ILLEGAL_PRIORITY;
                    break;
                }else if (DectectSuspendQueue(suspendQueue, pcb) == FALSE) {
                    *(long *)SystemCallData->Argument[1] = ERR_RESUME_NOT_SUSPEND_PROCESS;
                    break;
                }
                else {
                    OSResumeProcess(pcb);
                    *(long *)SystemCallData->Argument[1] = ERR_SUCCESS;
                    print("Resume", pcb->PID);
                    break;
                }
                
            case SYSNUM_CHANGE_PRIORITY:
                PID = (int)SystemCallData->Argument[0];
                priority = (int)SystemCallData->Argument[1];
                if (PID == 9999) {
                    *(long *)SystemCallData->Argument[2] = ERR_NO_SUCH_PID;
                    break;
                }
                
                if (priority == 999) {
                    *(long *)SystemCallData->Argument[2] = ERR_ILLEGAL_PRIORITY;
                    break;
                }
                
                if (PID == -1) {
                    current_running->priority= priority;
                    *(long *)SystemCallData->Argument[2] = ERR_SUCCESS;
                    break;
                }
                
                pcb = isInPCBChain(firstPCBChain, PID);
                if (pcb == NULL) {
                    *(long *)SystemCallData->Argument[2] = ERR_PROCESS_NOT_EXIST;
                    break;
                }else {
                    
                    print("ch_prior", pcb->PID);
                    pcb->priority = priority;
                    
                    if (DectectReadyQueue(readyQueue, pcb)) {
                        READ_MODIFY( ReadyQueue_LOCK, DO_LOCK, SUSPEND_UNTIL_LOCKED,
                                    &Result);
                        removeFromReadyQueue(readyQueue, pcb);
                        addToReadyQueue(readyQueue, pcb, sortByPrority);
                        READ_MODIFY( ReadyQueue_LOCK, DO_UNLOCK, SUSPEND_UNTIL_LOCKED,
                                    &Result);
                    }
                }
                *(long *)SystemCallData->Argument[2] = ERR_SUCCESS;
                break;
                
            case SYSNUM_SEND_MESSAGE:
                PID = (int)SystemCallData->Argument[0];
                msg = (char *)SystemCallData->Argument[1];
                msg_buffer_size = (int)SystemCallData->Argument[2];
                
                if (PID == 9999) {
                    *(long *)SystemCallData->Argument[3] = ERR_NO_SUCH_PID;
                    break;
                }
                //message_send_length is the size of the send buffer
                //it must be larger than or equal in size to the string of data that is actually sent
                if (msg_buffer_size <= 0 || msg_buffer_size > 64 || msg_buffer_size < strlen(msg)) {
                    *(long *)SystemCallData->Argument[3] = ERR_ILLEGAL_MESSAGE_LENGTH;
                    break;
                }
                
                if( get_length_MessageChain(firstMessageChain) > 20 ){
                    *(long*)SystemCallData->Argument[3] = ERR_OVERFLOW_MESSAGE;
                    break;
                }
                
                //pid = -1
                if (PID != -1) {
                    pcb = isInPCBChain(firstPCBChain, PID);
                }
                
                Message *message = CreateMessage(current_running->PID, PID, msg, msg_buffer_size);
                JoinMessageChain(firstMessageChain, message);
                
                if (pcb) {
                    if (DectectSuspendQueue(suspendQueue, pcb)) {
                        OSResumeProcess(pcb);
                    }
                }else {
                    GetFirstPCBinSuspendQueue(&pcb, suspendQueue);
                    if (pcb) {
                        OSResumeProcess(pcb);
                    }
                }
                
                *(long *)SystemCallData->Argument[3] = ERR_SUCCESS;
                break;
                
            case SYSNUM_RECEIVE_MESSAGE:
                PID = (int)SystemCallData->Argument[0];
                msg = (char *)SystemCallData->Argument[1];
                msg_buffer_size = (int)SystemCallData->Argument[2];
                
                if (PID == 9999) {
                    *(long *)SystemCallData->Argument[5] = ERR_NO_SUCH_PID;
                    break;
                }
                
                if (msg_buffer_size <= 0 || msg_buffer_size > 64) {
                    *(long *)SystemCallData->Argument[5] = ERR_ILLEGAL_MESSAGE_LENGTH;
                    break;
                }
                
                message = isInMessageChain(firstMessageChain, PID, current_running->PID);
                
                while (message == NULL) {
                    OSSuspendProcess(current_running);
                }
                
                if (msg_buffer_size < message->length) {
                    *(long *)SystemCallData->Argument[5] = ERR_ILLEGAL_MESSAGE_LENGTH;
                    break;
                }
                
                *(int*)SystemCallData->Argument[3] = message->length;
                *(int*)SystemCallData->Argument[4] = message->source_pid;
                strcpy( msg, message->msg_buff );
                DeleteFromMessageChain(firstMessageChain, message);
                
                *(long*)SystemCallData->Argument[5] = ERR_SUCCESS;
                break; 

            default:
                printf("ERROR! call_type not recognized!\n");
                printf("Call_type is - %i\n",call_type);
        }
}                                               // End of svc

/************************************************************************
    osInit
        This is the first routine called after the simulation begins.  This
        is equivalent to boot code.  All the initial OS components can be
        defined and initialized here.
************************************************************************/

void    osInit( int argc, char *argv[]  ) {
    void                *next_context;
    INT32               i;
    initPID = 0;
    ProcessControlBlock *pcb = NULL;
    
    //init every global Variable here
    firstPCBChain = initPCBChain();
    firstMessageChain = initMessageChain();
    timeQueue = (TimeQueue *) calloc(1, sizeof(TimeQueue));
    timeQueue->length = 0;
    timeQueue->QueueHead = NULL;
    readyQueue = (ReadyQueue *) calloc(1, sizeof(ReadyQueue));
    readyQueue->length = 0;
    readyQueue->QueueHead = NULL;
    suspendQueue = (SuspendQueue *) calloc(1, sizeof(SuspendQueue));
    suspendQueue->length = 0;
    suspendQueue->QueueHead = NULL;
    current_running = NULL;

    /* Demonstrates how calling arguments are passed thru to here       */

    printf( "Program called with %d arguments:", argc );
    for ( i = 0; i < argc; i++ )
        printf( " %s", argv[i] ); //argv[1]
    printf( "\n" );
//    printf( "Calling with argument 'sample' executes the sample program.\n" );

    /*          Setup so handlers will come to code in base.c           */

    TO_VECTOR[TO_VECTOR_INT_HANDLER_ADDR]   = (void *)interrupt_handler;
    TO_VECTOR[TO_VECTOR_FAULT_HANDLER_ADDR] = (void *)fault_handler;
    TO_VECTOR[TO_VECTOR_TRAP_HANDLER_ADDR]  = (void *)svc;

    /*  Determine if the switch was set, and if so go to demo routine.  */

    if (( argc > 1 ) && ( strcmp( argv[1], "sample" ) == 0 ) ) {
        Z502MakeContext( &next_context, (void *)sample_code, KERNEL_MODE );
        Z502SwitchContext( SWITCH_CONTEXT_SAVE_MODE, &next_context );
    }                   /* This routine should never return!!           */

    /*  This should be done by a "os_make_process" routine, so that
        test0 runs on a process recognized by the operating system.    */
//    Z502MakeContext( &next_context, (void *)test1a, USER_MODE );
//    Z502SwitchContext( SWITCH_CONTEXT_KILL_MODE, &next_context );
    
    initTestProcess(argv[1], pcb);
//    OSCreatProcess(&pcb, (void *)test1j,"test1j",0,USER_MODE);
    print("Creat", pcb->PID);
    while (TRUE) {
        if (dispatchProcess() == FALSE) {
            break;
        }
    }
}                                               // End of osInit
