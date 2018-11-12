/*
 *  p_thraddsem2.c
 *  compile:
 *  gcc p_thraddsem2.c -o p_thraddsem2 -lpthread 
 *
 *  Same program like p_thraddsem.c but using conditional compilation for platform independent code
 *
 * The Mach kernel specifies the following functions for using semaphores, 
 * as defined by <mach/semaphore.h> and <mach/task.h> 
 * (Note: only listed the important ones that have POSIX relatives):
 *
 * kern_return_t semaphore_create(task_t task, semaphore_t *semaphore,int policy, int value)
 * kern_return_t semaphore_signal(semaphore_t semaphore)
 * kern_return_t semaphore_signal_all(semaphore_t semaphore)
 * kern_return_t semaphore_wait(semaphore_t semaphore)
 * kern_return_t semaphore_destroy(task_t task, semaphore_t semaphore)
 * kern_return_t semaphore_signal_thread(semaphore_t semaphore, thread_act_t thread_act)
 *  
 *  Created by G. Boccignone on 20/05/09.
 *  Copyright 2009 Dipartimento di Scienze dell'Informazione, Università di Milano. All rights reserved.
 *
 */


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define __APPLE__ 1  //set to 0 on a Linux platform

//_______________ compiles a platform independent set of semaphore functions
#ifdef __APPLE__
#include <mach/semaphore.h>
#include <mach/task.h>
#else
#include <semaphore.h>
#endif

//sem_t mymutex; //Declare  the ( pointer to) semaphore global (outside of any function)
#ifdef __APPLE__
semaphore_t mymutex ;
#else
sem_t mymutex;
#endif
//_______________

#define NITER 1000000


int count = 0;


void _sem_create(void * semStructure, int initialValue);
void _sem_signal(void * semStructure);
void _sem_wait(void * semStructure);
void _sem_destroy(void * semStructure);

void * ThreadAdd(void *); //function executed by threads


int main(int argc, char * argv[])
{
    pthread_t tid1, tid2;
	
	//Initialize the unnamed semaphore in the main function: initial value is set to 1.
    // Note the second argument: passing zero denotes
    // that the semaphore is shared between threads (and
    // not processes).
	_sem_create(&mymutex, 1);
	
	//mymutex=sem_open("pippo", S_IRUSR |S_IWUSR , O_CREAT, 1);
	
    if(pthread_create(&tid1, NULL, ThreadAdd, NULL)){
		printf("\n ERROR creating thread 1");
		exit(1);
    }
	
    if(pthread_create(&tid2, NULL, ThreadAdd, NULL)){
		printf("\n ERROR creating thread 2");
		exit(1);
    }
	
    if(pthread_join(tid1, NULL))	/* wait for the thread 1 to finish */{
		printf("\n ERROR joining thread");
		exit(1);
    }
	
    if(pthread_join(tid2, NULL))        /* wait for the thread 2 to finish */{
		printf("\n ERROR joining thread");
		exit(1);
    }
	
    if (count < 2 * NITER) 
        printf("\n BOOM! count is [%d], should be %d\n", count, 2*NITER);
    else
        printf("\n OK! count is [%d]\n", count);
	
	// destroys the semaphore; no threads should be waiting on the semaphore if its destruction is to succeed
	_sem_destroy(&mymutex);
	
	pthread_exit(NULL);
}

void * ThreadAdd(void * a)
{
    int i, tmp;
	int value;
	
    for(i = 0; i < NITER; i++){			
		//entering the critical section: wait on semaphore
		_sem_wait(&mymutex);
        tmp = count;      /* copy the global count locally */
        tmp = tmp+1;      /* increment the local copy */
        count = tmp;      /* store the local value into the global count */ 
		//exiting the critical section: increments the value of the semaphore and wakes up a blocked process waiting on the semaphore, if any.
		_sem_signal(&mymutex);
	}
}

void _sem_create(void * semStructure, int initialValue)
{
#ifdef __APPLE__
    semaphore_create(mach_task_self(), (semaphore_t *)semStructure, SYNC_POLICY_FIFO, initialValue);
#else
    int pshared = 0;
    sem_init((sem_t *)semStructure, pshared, initialValue);
#endif
}

void _sem_destroy(void * semStructure)
{
#ifdef __APPLE__
    semaphore_destroy(mach_task_self(), (semaphore_t)semStructure);
#else
    sem_destroy((sem_t *)semStructure);
#endif
}



void _sem_signal(void * semStructure)
{
#ifdef __APPLE__
    semaphore_signal(*((semaphore_t *)semStructure));
#else
    sem_post((sem_t *)semStructure);
#endif
}


void _sem_wait(void * semStructure)
{
#ifdef __APPLE__
    semaphore_wait(*((semaphore_t *)semStructure));
#else
    sem_wait((sem_t *)semStructure);
#endif
}
