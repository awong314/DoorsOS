// kernel_types.h, 159

#ifndef _KERNEL_TYPES_H_
#define _KERNEL_TYPES_H_

#include "kernel_constants.h"

typedef void (*func_p_t)(); // void-return function pointer type

// SLEEP state added in Phase2
// WAITCHILD and ZOMBIE added in Phase 8
typedef enum {AVAIL, READY, RUN, SLEEP, WAIT, WAITCHILD, ZOMBIE} state_t;   
//typedef enum {TT, IT, ST, IP, SP} pages_t;   

typedef struct {
   //Top 4 regs Phase 6
   unsigned int edi,
               esi,
               ebp,
               esp,     
               ebx,
		         edx,
		         ecx,
		         eax,
		         intr_num,
		         eip,
		         cs,
		         efl;
} trapframe_t;

typedef struct {
   // Phase 1
   state_t state;             // state of process
   int runtime;               // runtime of this run
   int totaltime;             // total runtime
   trapframe_t *trapframe_p;  // points to saved trapframe
   // Phase 2
   int wake_time;	            // wake time of proc in centi-secs = cur OS time + sleep time 
   // Phase 6
   int ppid;                  // Parent pid         
   // Phase A 
   int page[5];               // To record page # used by the proc
   int TT;                    // Address of proc translation table
} pcb_t;                     

typedef struct {              // generic queue type
   int q[Q_SIZE];             // integers are queued in q[] array
   int size;                  // size is also where the tail is for new data
   int indexOfFirst;          // index of the first value
} pid_q_t;


// Phase 3
typedef struct {
   int val;
   pid_q_t wait_q;
} semaphore_t;


// Phase 4
typedef struct {              // phase4 Dev Drv I
   char dsp[BUFF_SIZE];       // buffer for term output
   int port;                  // port data register
   pid_q_t dsp_wait_q;        // PID await for term output 

   // Phase 5
   char kb[BUFF_SIZE];        // Keyboard buffer
   pid_q_t kb_wait_q;         
   int status;   
} term_t;

#endif // _KERNEL_TYPES_H_
