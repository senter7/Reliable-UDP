//
//  server_status.h
//  Reliable UDP
//
//  Created by Simone Minasola on 16/11/15.
//  Copyright (c) 2015 Simone Minasola. All rights reserved.
//  <simone.minasola@gmail.com>
//
//  GPLV3
//  This file is part of Reliable UDP.
//
//  Reliable UDP is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Reliabe UDP is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Reliabe UDP.  If not, see <http://www.gnu.org/licenses/>.
//
//
//  ABSTRACT
//
//  This header file contains support data structures for the server.
//  In particular, 'server_status' is a data structure that should be shared among
//  all the processess created by the server. So, it has to be created an area of
//  shared memory. 'server_status' lets you know the status of the server
//  (used ports, free ports, active processes) and guarantees mutual exclusion
//  between processes for writing to files or on the screen.
//  'free_p' data structure is used to manage the available ports. In fact,
//  as soon as the program starts, it is created and initialized an array of
//  'free_p' with values ​​of available ports (see 'settings.h' and FIRST_AVAILABLE_PORT
//  for details).
//  All these functions are synchronized.
//  The client uses a fake 'server_status' structure. In fact, it has a 'server_status'
//  variable that is setted to NULL.


#ifndef __Reliable_UDP__server_status__
#define __Reliable_UDP__server_status__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "utils.h"

/*  'free_p' is used to search, get and release a port that will be used to send/receive
 *  packets. The number of ports is determined by MAX_PROCESSES_NUMBER
 *  in 'settings.h'. In fact, every process can get a port. The port numbers are
 *  contiguous, and starts from the FIRST_AVAILABLE_PORT in 'settings.h'. See
 *  'settings.h' for details.
 */
struct free_p {
    int used;   //0 if the port is unused, otherwise 1
    int port;   //Number of the avaible port
    pid_t user; //0 if port is unused, otherwise process ID of the process
                //that is using the port
};


/*  This data structure identifies the server status. Whit 'server_status', it is
 *  possibile to kwon how many processes are running on the server and how many
 *  ports are involved. In addition, using a zone of shared memory between processes,
 *  you can assign unique ports for each request, and write to files or on the
 *  screen with synchronization, with the use of a semaphore that guarantees mutual 
 *  exclusion.
 */
struct server_status {
    struct free_p v[MAX_PROCESSES_NUMBER];  //Array of avaible ports
    int processes;                          //Number of processes currently in running
    sem_t MTX;                              //Semaphore to sync processes (server)
};


/*  This function creates a shared memory area, shareable between all server 
 *  processes. Furthermore, this functions initializes and returns a
 *  shareable 'server_status' data structure.
 *
 *  Parameters:     Nothing
 *
 *  Return:         Pointer to shareable 'server_status'
 */
struct server_status *create_shared_memory(void);


/*  This function is used to get the number of running processes on the server
 *
 *  Parameters:
 *  - status:       Pointer to the 'server_status' data structure
 *
 *  Return:         Number of the running processes (integer)
 */
int get_processes(struct server_status *status);


/*  Increments by 1 the number of running processes
 *
 *  Parameters:
 *  - status:       Pointer to the 'server_status' data structure
 *
 *  Return:         Nothing
 */
void increase_processes(struct server_status *status);


/*  Decrements by 1 the number of running processes
 *
 *  Parameters:
 *  - status:       Pointer to the 'server_status' data structure
 *
 *  Return:         Nothing
 */
void decrease_processes(struct server_status *status);


/*  This function returns the first avaible port in 'free_p' array
 *  into 'status. See g'et_first_free_port()' in 'server_status.c' for
 *  more details.
 *
 *  Parameters:
 *  - status:       Pointer to the 'server_status' data structure
 *
 *  Return:         Pointer to 'free_p' struct with the avaible port
 */
struct free_p *first_available_port(struct server_status *status);


/*  This function set a port as unused. See 'release_port' in
 *  'server_status.c' for more details.
 *
 *  Parameters:
 *  - status:       Pointer to the 'server_status' data structure
 *  - port:         port number to search into array
 *
 *  Return:         Nothing
 */
void close_port(struct server_status *status, int port);


/*  This function implements an another way to set a port as unused.
 *  The function research the port to be relased by pid.
 *  In fact, each 'free_p' struct stores the process ID of the process
 *  that will use it.
 *
 *  Parameters:
 *  - status:       Pointer to the 'server_status' data structure
 *  - pid           Process ID af the user
 *
 *  Return:         Nothing
 */
void close_port1(struct server_status *status, pid_t pid);


/*  This function locks a semaphore to allow mutual exclusion
 *
 *  Parameters:
 *  - status:       Pointer to the 'server_status' data structure wich contains
 *                  the semaphore
 *
 *  Return:         Nothing
 */
void get_sem(struct server_status *status);


/*  This function unlocks a semaphore to allow mutual exclusion
 *
 *  Parameters:
 *  - status:       Pointer to the 'server_status' data structure wich contains
 *                  the semaphore
 *
 *  Return:         Nothing
 */
void release_sem(struct server_status *status);


#endif /* defined(__Reliable_UDP__server_status__) */
