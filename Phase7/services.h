// services.h, 159

#ifndef _SERVICES_H_
#define _SERVICES_H_

#include "kernel_types.h"   // need definition of 'func_p_t' below

void NewProcService(func_p_t);
void TimerService(void);
void SyscallService(trapframe_t*);
void GetpidService(int*);
void SleepService(int);
void WriteService(int, char*, int);
void SemwaitService(int);
void SempostService(int);
void TermService(int);
// Phase 5
void DspService(int);
void KbService(int);
void ReadService(int, char*, int); 
// Phase 6
void ForkService(int *ebx_p);
// Phase 7
void SignalService(int signal, func_p_t p);
void GetppidService(int *p);
void WrapperService(int pid, func_p_t p);

#endif
