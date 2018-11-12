//
//  server_status.c
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

#include "server_status.h"

void get_sem(struct server_status *status) {
    if (status == NULL) //Client uses NULL.
        return;         //See ABSTRACT in 'server_status.h" for important details
    
    if (sem_wait(&status->MTX) == -1) {
        perror("sem_wait() in get_sem()");
        exit(EXIT_FAILURE);
    }
}

void release_sem(struct server_status *status) {
    if (status == NULL) //Client uses NULL.
        return;         //See ABSTRACT in 'server_status.h" for important details
    
    if (sem_post(&status->MTX) == -1) {
        perror("sem_post() in release_sem()");
        exit(EXIT_FAILURE);
    }
}


/*  This function initializes all avaible ports as unused
 *
 *  Parameters:
 *  - v:        Pointer to the array of 'free_p' that contains the available ports
 *
 *  Return:     1 on success. If FIRST_AVAILABLE_PORT in 'settings.h' is set
 *              incorrectly, the function return 0. See 'seetings.h'
 *              for details.
 */
int init_free_port(struct free_p *v) {
    int i, j = FIRST_AVAILABLE_PORT;
    for (i=0; i<MAX_PROCESSES_NUMBER; ++i) {
        v[i].port = j+i;
        v[i].used = 0;
        v[i].user = 0;
        if (j+i == SERV_PORT)   //Change the settings!
            return 0;
    }
    return 1;
}


/*  This function searches for the first available port between all available
 *  ports that the server can use. If there is a free port, then it is set as
 *  used.
 *
 *  Parameters:
 *  - v:        Pointer to the array of 'free_p' that contains the available ports
 *  - n:        Max number of port available
 *
 *  Return:     Pointer to first available 'free_p' structure. If no ports are 
 *              available, it returns NULL
 */
struct free_p *get_first_free_port(struct free_p *v, int n) {
    int i;
    for (i=0; i<n; ++i) {
        if (v[i].used == 0) {
            v[i].used = 1;    //Set as used and return it
            return &v[i];
        }
    }
    return NULL;
}


/*  This function search the port number 'port' into 'free_p' array,
 *  and sets it to unused again.
 *
 *  Parameters:
 *  - v:        Pointer to the array of 'free_p' that contains the available ports
 *  - port:     The port number to search
 *  - n:        Max number of port available
 *
 *  Return:     Nothing
 */
void release_port(struct free_p *v, int port, int n) {
    int i;
    for (i=0; i<n; ++i) {
        if (v[i].port == port) {
            v[i].used = 0;  //set as unused
            v[i].user = 0;  //remove the process id
            break;
        }
    }
}

/*  This function search the 'free_p' structure into 'free_p' array that contains
 *  the port used by process ID 'pid', and sets it to unused again.
 *
 *  Parameters:
 *  - v:        Pointer to the array of 'free_p' that contains the available ports
 *  - pid:      The process ID to search
 *  - n:        Max number of port available
 *
 *  Return:     Nothing
 */
void release_port1(struct free_p *v, pid_t pid, int n) {
    int i;
    for (i=0; i<n; ++i) {
        if (v[i].user == pid) {
            v[i].used = 0;  //set as unused
            v[i].user = 0;  //remove the process id
            break;
        }
    }
}


/*  This function initializes a 'server_status' data structure with
 *  default values. Semaphore is initialized too, as the array of
 *  'free_p' included in 'server_status'.
 *
 *  Parameters:
 *  - status:       Pointer to the 'server_status' data structure
 *
 *  Return:         1 on success, otherwise 0. See 'init_free_port()' in
 *                  'server_status.c' for more and important details.
 */
int server_status_init(struct server_status *status) {
    //Initialize 'free_p' array
    if (init_free_port(status->v) == 0)
        return 0;
    
    //At the beginning, there isn't any running process
    status->processes = 0;
    
    //Initialize the unnamed semaphore
    if (sem_init(&status->MTX, 1, 1) == -1) {
        perror("sem_init() in server_status_init()");
        exit(EXIT_FAILURE);
    }
    
    return 1;
}


struct server_status *create_shared_memory(void) {
    struct server_status *status;
    key_t key;
    int fd;
    
    //Create a new key, hoping not used already :)
    //This method is standard in C POSIX, but not entirely safe
    key = ftok(".", '3');
    if(key == -1) {
        perror("ftok() in create_shared_memory()");
        exit(EXIT_FAILURE);
    }
    
    //Create a new shared memory area with the key 'key'.
    fd = shmget(key, sizeof(struct server_status), IPC_CREAT|0666);
    if(fd == -1) {
        perror("shmget() in create_shared_memory()");
        exit(EXIT_FAILURE);
    }
    
    //Attach the shared memory zone at the process
    status = shmat(fd, NULL, 0);
    if(status == NULL) {
        perror("shmat() in create_shared_memory()");
        exit(EXIT_FAILURE);
    }
    
    //Initialize 'server_status' just created
    if (server_status_init(status) == 0)
        return NULL;
    
    return status;
}


int get_processes(struct server_status *status) {
    get_sem(status);
    int i = status->processes;
    release_sem(status);
    return i;
}


void increase_processes(struct server_status *status) {
    get_sem(status);
    status->processes++;
    release_sem(status);
}


void decrease_processes(struct server_status *status) {
    get_sem(status);            
    status->processes--;
    release_sem(status);
}


struct free_p *first_available_port(struct server_status *status) {
    struct free_p *p = NULL;
    
    get_sem(status);
    p = get_first_free_port(status->v, MAX_PROCESSES_NUMBER);
    release_sem(status);
    
    return p;
}


void close_port(struct server_status *status, int port) {
    get_sem(status);
    release_port(status->v, port, MAX_PROCESSES_NUMBER);
    release_sem(status);
}


void close_port1(struct server_status *status, pid_t pid) {
    get_sem(status);
    release_port1(status->v, pid, MAX_PROCESSES_NUMBER);
    release_sem(status);
}

