// proc.c, 159
// all processes are coded here
// processes do not use kernel data or code, must ask via service calls

#include "spede.h"       // cons_xxx below needs
#include "kernel_data.h" // run_pid needed below
#include "syscalls.h"    // prototypes of the system calls
#include "proc.h"        // prototypes of processes
#include "tools.h"

void IdleProc(void) {
   int i;
   unsigned short *p = (unsigned short *)0xb8000 + 79;//upper right corner of display 
   while(1) {
      *p = '0' + 0x0f00; // show '0' at the upper right corner
      for(i=0; i<LOOP/2; i++) asm("inb $0x80"); // delay .5 sec
      *p = ' ' + 0x0f00; // show ' ' at the upper right corner
      for(i=0; i<LOOP/2; i++) asm("inb $0x80"); // delay 
   }
}

// Phase 6 
void ChildStuff(int which) {  // which terminal to display msg
   int i, my_pid, centi_sec;
   char str[] = "   ";

   // Get my PID
   my_pid = sys_getpid();

   // Calcalute sleep period (multiple of .5 seconds times my PID)
   centi_sec = 50 * my_pid;

   // Build a string based on my PID
   str[0] = '0' + my_pid/10;
   str[1] = '0' + my_pid%10;

   // Phase 8
   for(i=0; i<3; i++) {
      sys_write(which, "\n\r", 2);
      sys_write(which, "I'm the child, PID ", 19);
      sys_write(which, str, 3);
      sys_sleep(centi_sec);
   }

   sys_exit(100 - my_pid);
}

void UserProc(void) {
   int my_pid, cpid, centi_sec, which;
   char str[] = "   ";
   char cmd[BUFF_SIZE];

   my_pid = sys_getpid();
   centi_sec = 50 * my_pid;
   str[0] = '0' + my_pid/10;
   str[1] = '0' + my_pid%10;

   which = (my_pid % 2)? TERM1 : TERM2;   //Determine which terminal to go to
   
   // Phase 7  
   sys_signal(SIGINT, &Ouch);  // Register handler routine for SIGINT

   while(1) {
      sys_write(which, "\n\r", 2);        // get a new line
      sys_write(which, str, 3);           // to show my PID
      sys_write(which, "enter ", 6);      // and other msgs
      sys_write(which, "shell ", 6);
      sys_write(which, "command: ", 9);
      sys_read(which, cmd, BUFF_SIZE);    // here we read term KB
      sys_write(which, "You've entered: ", 16);
      sys_write(which, cmd, BUFF_SIZE);   // verify what's read
      if(MyStrcmp(cmd, "fork")) {
         cpid = sys_fork();
         switch(cpid) {
            case -1:
               // Show error message (OS failed to fork)
               sys_write(which, "\n\rUserProc: cannot fork!\n\r", 28);
               break;
            case 0:
               // Phase 9 
               // Child forked, call sys_exec to run ChildStuff in separate DRAM space (outside of MyOS.dli)
               sys_exec(ChildStuff, which);
               break;
            default:
               // Phase 8 Go waitchild immediately
               ChildHandler();
               break;
         }
      } else if(MyStrcmp(cmd, "fork&") || MyStrcmp(cmd, "fork &")) {
         sys_signal(SIGCHILD, ChildHandler);
         cpid = sys_fork();
         switch(cpid) {
            case -1:
               sys_write(which, "\n\rUserProc: cannot fork!\n\r", 28);
               sys_signal(SIGCHILD, NULL);
               break;
            case 0:
               sys_exec(ChildStuff, which);
               break;
            default:
               // NOTHING
               break;
         }
      }
      sys_sleep(centi_sec);               // sleep for .5 sec x PID
   }
}

// Phase 7
void Wrapper(func_p_t p) {             // arg implanted in stack
   asm("pusha");                       // save regs
   p();                                // call user's signal handler
   asm("popa");                        // pop back regs
   asm("mov %%ebp, %%esp; pop %%ebp; ret $4"::);
}

void Ouch(void) {                      // signal handler
   int ppid, which;

   ppid = sys_getppid();               // follow parent
   if (ppid == 0) ppid = sys_getpid(); // no parent, use own PID

   which = ppid % 2 ? TERM1 : TERM2;
   sys_write(which, "Ouch, don't touch that! ", 24);
}

// Phase 8
void ChildHandler(void) {
   int which, child_pid, exit_code;
   char str[] = "   ";

   child_pid = sys_waitchild(&exit_code);
   // Determine which terminal to use
   which = (sys_getpid() % 2) ? TERM1 : TERM2;
   // Build str from child_pid
   str[0] = '0' + child_pid/10;
   str[1] = '0' + child_pid%10;
   // Show the message
   sys_write(which, "The child PID ", 14);
   sys_write(which, str, 3);
   sys_write(which, " exited, ", 9);
   // Build str from exit_code
   str[0] = '0' + exit_code/10;
   str[1] = '0' + exit_code%10;
   // Show the message
   sys_write(which, "exit code = ", 12);
   sys_write(which, str, 3);
}

