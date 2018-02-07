// tools.c, 159

#include "spede.h"
#include "kernel_types.h"
#include "kernel_data.h"

// clear DRAM by setting each byte to zero
void MyBzero(char *p, int size) {
      for(int i=0; i<size; i++) {
            //set where p points to to (char *)0
            &p = 0;
	    //Increment p
	    p++;
      }
}

// dequeue, return 1st element in array, and move all forward
// if queue empty, return -1
int DeQ(pid_q_t *p) {
   int i,  element = -1;

   if the size of the queue p points to is zero, return element (-1)
   (otherwise, continue)
   copy the 1st in the array that p points to to element
   decrement the size of the queue p points to by 1
   move all elements in the array to the front by one position

   return element;
}

// enqueue element to next available position in array, 'size' is array index
void EnQ(int element, pid_q_t *p) {
   if the size of the queue that p points to equals Q_SIZE {
      show on target PC: "Kernel Panic: queue is full, cannot EnQ!\n"
      return;       // alternative: breakpoint() into GDB
   }
   copy element into the array indexed by 'size'
   increment 'size' of the queue p points to by 1
}

