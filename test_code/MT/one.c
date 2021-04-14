#define _OPEN_THREADS                                                           
                                                                                
#include <errno.h>                                                              
#include "thread.h"                                                           
#include <signal.h>                                                             
#include <stdio.h>                                                              
#include <unistd.h>  
#include <stdlib.h>                                                             
                                                                                
void            *threadfunc(void *parm)                                         
{                                                                               
 int        threadnum;                                                          
 int        *tnum;                                                              
 sigset_t   set;                                                                
                                                                                
 tnum = parm;                                                                   
 threadnum = *tnum;                                                             
                                                                                
 printf("Thread %d executing\n", threadnum);                                    
 sigemptyset(&set);                                                             
 if(sigaddset(&set, SIGUSR1) == -1) {                                           
    perror("Sigaddset error");                                                  
    thread_exit((void *)1);                                                    
 }                                                                              
                                                                                
 if(sigwait(&set,NULL) != SIGUSR1) {                                                 
    perror("Sigwait error");                                                    
    thread_exit((void *)2);                                                    
 }                                                                              
                                                                                
 thread_exit((void *)0);                                                       
}                                                                               
                                                                                
main() {                                                                        
 int          status;                                                           
 int          threadparm = 1;                                                   
 thread_t    threadid;                                                         
 int          thread_stat;                                                      
                                                                                
 status = thread_create( &threadid,                                               
                          threadfunc,                                           
                          (void *)&threadparm);                                 
 if ( status <  0) {                                                            
    perror("thread_create failed");                                            
    exit(1);                                                                    
 }                                                                              
                                                                                
 sleep(5);                                                                      
                                                                                
 status = thread_kill( threadid, SIGUSR1);                                     
 if ( status <  0)                                                              
    perror("thread_kill failed");                                              
                                                                                
 status = thread_join( threadid, (void *)&thread_stat);                        
 if ( status <  0)                                                              
    perror("thread_join failed");                                              
                                                                                
 exit(0);                                                                       
}              
