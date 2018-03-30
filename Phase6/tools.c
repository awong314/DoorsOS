// tools.c, 159

#include "spede.h"
#include "kernel_types.h"
#include "kernel_data.h"

// clear DRAM by setting each byte to zero
void MyBzero(char *p, int size) {
   int i;
   for(i=0; i<size; i++) {          //loop for 'size' number of times:
      *p = (char)0;                 //set where p points to to (char *)0
      p++;                          //increment p (by 1)
   }
}

// dequeue, return 1st element in array, and move all forward
// if queue empty, return -1
int DeQ(pid_q_t *p) {
   int element = -1;

   if(p->size == 0) return element;             //if the size of the queue p to is zero, return element (-1) (else, continue)
   
   element = p->q[p->indexOfFirst];             //copy the 1st in the array that p points to to element
   p->size -= 1;                                //decrement the size of the queue p points to by 1
   p->indexOfFirst = (p->indexOfFirst + 1) % Q_SIZE;  //move all elements in the array to the front by one position

   return element;
}

// enqueue element to next available position in array, 'size' is array index
void EnQ(int element, pid_q_t *p) {
   if(p->size == Q_SIZE) {                            //if the size of the queue that p points to equals Q_SIZE 
      cons_printf("Kernel Panic: Queue is full...");  //show on target PC: "Kernel Panic: queue is full, cannot EnQ!\n"
      return;                                         //alternative: breakpoint() into GDB
   }
   p->q[(p->indexOfFirst + p->size) % Q_SIZE] = element;  //copy element into the array indexed by 'size'
   p->size += 1;                                   //increment 'size' of the queue p points to by 1
}

//Phase 4 String Copy Function
void MyStrcpy(char *dst, char *src) {
   while(*src) {
      *dst = *src;
      src++;
      dst++;
   }
   *(dst) = '\0';
}

//Phase 5 
void MyStrShift(char *str) {
   while(*str) {    
      if(*str == '\0') return;  
      *str = *(str+1);
      str++;
   }
}

void MyStrAppend(char *str, char c) {   
   int cnt = 0;
   while(*str && cnt<BUFF_SIZE-1) {
      str++;
      cnt++;
   }
   *str = c;
   *(str+1) = '\0';
}

//Phase 6
int MyStrcmp(char *s1, char *s2) {
   while(*s1 || *s2) {
      if(*s1 != *s2) return 0;   //Return FALSE
      s1++;
      s2++;
   }
   return 1;   //Return TRUE
}

void MyMemcpy(char *dst, char *src, int bytes) {
   int i;   
   for(i=0; i<bytes; i++) {
      *dst = *src;
   }
}


