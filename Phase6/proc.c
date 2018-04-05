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

/*
// Phase 5 Userproc
void UserProc(void) {
   int my_pid, centi_sec, which;
   char str[] = "   ";
   char cmd[BUFF_SIZE];

   my_pid = sys_getpid();
   centi_sec = 50 * my_pid;
   str[0] = '0' + my_pid/10;
   str[1] = '0' + my_pid%10;

   which = (my_pid % 2)? TERM1 : TERM2; //Determine which terminal to go to

   while(1) {
      sys_write(which, "\n\r", 2);      // get a new line
      sys_write(which, str, 3);         // to show my PID
      sys_write(which, "enter ", 6);    // and other msgs
      sys_write(which, "shell ", 6);
      sys_write(which, "command: ", 9);
      sys_read(which, cmd, BUFF_SIZE);  // here we read term KB
      sys_write(which, "You've entered: ", 16);
      sys_write(which, cmd, BUFF_SIZE); // verify what's read
      sys_sleep(centi_sec);             // sleep for .5 sec x PID
   }
}
*/ 

// Phase 6 
void ChildStuff(int which) {  // which terminal to display msg
   int my_pid, centi_sec;
   char str[] = "   ";

   // Get my PID
   my_pid = sys_getpid();

   // Calcalute sleep period (multiple of .5 seconds times my PID)
   centi_sec = 50 * my_pid;

   // Build a string based on my PID
   str[0] = '0' + my_pid/10;
   str[1] = '0' + my_pid%10;

   // Loop forever: show msg need multiple sys_write() calls
   while(1) {
      sys_write(which, "\n\r", 2);        // get a new line
      sys_write(which, str, 3);           // to show my PID
      sys_write(which, "I ", 6);          // and other msgs
      sys_write(which, "shell ", 6);
      sys_write(which, "command: ", 9);
      sys_sleep(centi_sec);               // sleep for .5 sec x PID
   }
   
}

void UserProc(void) {
   int my_pid, cpid, centi_sec, which;
   char str[] = "   ";
   char str1[] = "   ";
   char cmd[BUFF_SIZE];

   my_pid = sys_getpid();
   centi_sec = 50 * my_pid;
   str[0] = '0' + my_pid/10;
   str[1] = '0' + my_pid%10;

   which = (my_pid % 2)? TERM1 : TERM2;   //Determine which terminal to go to

   while(1) {
      sys_write(which, "\n\r", 2);        // get a new line
      sys_write(which, str, 3);           // to show my PID
      sys_write(which, "enter ", 6);      // and other msgs
      sys_write(which, "shell ", 6);
      sys_write(which, "command: ", 9);
      sys_read(which, cmd, BUFF_SIZE);    // here we read term KB
      sys_write(which, "You've entered: ", 16);
      sys_write(which, cmd, BUFF_SIZE);   // verify what's read
      sys_sleep(centi_sec);               // sleep for .5 sec x PID
      if(MyStrcmp(cmd, "fork")) {
         cpid = sys_fork();
         switch(cpid) {
            case -1:
               // Show error message (OS failed to fork)
               cons_printf("Fork failed try chopsticks...\n");
               break;
            case 0:
               // Child process created, let it do ChildStuff()
               ChildStuff(which);
               break;
            default:
               // Build a str from pid and show it (see demo for exact content)
               str1[0] = '0' + cpid/10;
               str1[1] = '0' + cpid%10;
               sys_write(which, "\n\r", 2);        // get a new line
               sys_write(which, "UserProc ", 9);     
               sys_write(which, "forked ", 7);
               sys_write(which, "a child, ", 9);     
               sys_write(which, "PID ", 4);
               sys_write(which, str1, 3);
         }
      }
   }
}
