#include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include "thread.h"
    static threadlock spinlock;
    static int test_value = 0;
    //int UDP_first_thread();
    void * UDP_first_thread() 
    {
    	int i ,ret;
    	
    	printf("UDP_first_thread begin\n");
    	for(i =0 ;i<10 ;i++ ) {
    	thread_lock(&spinlock);
    		test_value++;
    		printf("test_value %d\n", test_value);
    		sleep(5);
    		thread_unlock(&spinlock);
    	}
    	printf("UDP_first_thread end\n");
    	return (void *) ret;
    }
     
    void * UDP_second_thread() 
    {
    	int i ,ret;
    	
    	printf("UDP_second_thread begin\n");
    	for(i =0 ;i<10 ;i++ ) {
    	    thread_lock(&spinlock);
    		test_value--;
    		printf("test_value %d\n", test_value);
    		sleep(3);
    		thread_unlock(&spinlock);
    	}
    	printf("UDP_second_thread end\n");
    	return (void *)ret;
    }
     
    int main(int argC, char* arg[]) 
    {
     
    	int err;
    	thread_t tid1, tid2;
    	
    	initlock(&spinlock);
    	//----------------Create UDP server thread ----------------
    	err = thread_create(&tid1, UDP_first_thread, NULL);
    	if (err != 0) {
    		perror(" fail to create thread ");
    		return -1;
    	}
    	sleep(1);
    	
    		err = thread_create(&tid1, UDP_second_thread, NULL);
    	if (err != 0) {
    		perror(" fail to create thread ");
    		return -1;
    	}
       
        thread_join(tid1, NULL);
    	thread_join(tid2, NULL);
    	
    	printf("main end\n");
     
    	return 0;
    }
