//
//  time_controller.h
//  Reliable UDP
//
//  Created by Simone Minasola on 17/08/15.
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
//  This header file contains the 'time_controller' data structure and its functions,
//  that allow to interact whit it. This data structure allows to control a number
//  of 'time_data' data structures, and controls if the timeouts are expired.
//  A thread checks every TIME_CONTROLLER_GRANULARITY msecs all 'time_data' structures
//  into the 'timer_wheel' to verify which timeout has expired. For best results,
//  TIME_CONTROLLER_GRANULARITY in 'settings.h' should be set neither too small
//  (in order to prevent high cpu usage) nor too large (so as not to allow a expired
//  packet remain too long in the window).
//  The major operations that 'time_controller' can execute are:
//  - add a new timeout, putting a new 'time_data' into the 'timer_wheel'
//  - remove a specified 'time_data' from the 'timer_wheel'
//  - start and stop the thread responsible for monitoring all timeouts
//
//  The most important fact is that the sequence number in 'time_data' is closely related
//  to the sequence number in 'packet': every packet has a unique 'time_data' structure,
//  that owns the same sequence number of the packet itself. In this way, it is possible to
//  identify a packet to which corresponds a specific timeout, and resend this packet
//  if timeout has expired.
//  All the operation are process/thread safe.


#ifndef __Reliable_UDP__time_controller__
#define __Reliable_UDP__time_controller__

#include "timer_wheel.h"
#include "window_controller.h"
#include "window.h"
#include "utils.h"
#include "strings.h"
#include "server_status.h"

#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>

struct time_controller {
    unsigned int granularity;       //TIME_CONTROLLER_GRANULARITY in msecs
    pthread_t t;                    //Thread identifier
    pthread_mutex_t MTX;            //Mutex to ensuring mutual exclusion
    pthread_cond_t full;            //Used to wait until it is free at least one slot in the window
    pthread_cond_t empty;           //Used to wait until it is full at least one slot in the window
    struct timer_wheel *tw;         //Pointer to a initializated 'timer_wheel'
    struct window_controller *wc;   //Pointer to a initializated 'window_controller'
    int running;                    //1 if thread is running, 0 otherwise
    FILE *log;                      //Pointer to the log file (it can be NULL)
    int user;                       //ID of the user: LS_SERVER or LS_CLIENT
    struct server_status *status;   //Pointer to 'server_status' struct (it can be NULL)
};


/*  This function creates a new valid 'time_controller' data structure already
 *  initializated.
 *
 *  Parameters:
 *  - granularity:      This value represents the time during which the thread 
 *                      sleeps in msecs. When he awoke, the thread will check all 
 *                      the timeout into 'timer_wheel' before going back to sleep.
 *                      In this program, the value passed at this function is
 *                      TIME_CONTROLLER_GRANULARITY, a macro in 'settings.h'
 *  - dim:              The dimension of the sliding window. In this program, the value passed
 *                      in this function is WINDOW_DIMENSION, a macro in 'settings.h'
 *
 *  Return:             A pointer to a valid 'time_controller' data structure
 */
struct time_controller *new_time_controller(unsigned int granularity, int dim, FILE *log, int user,
                                            struct server_status *status);


/*  This function starts a new thread that is responsible for controlling timeouts.
 *  This thread execute throughout its life time the function 'thread_work'
 *  (see 'time_controller.c' for detail).
 *
 *  Parameters:
 *  - tc:               Pointer to 'time_controller' through wich execute the operation
 *  - wc:               Pointer to 'window_controller' through wich resend a packet
 *                      if a timeout has expired with 'window_controller_resend_packet'
 *
 *  Return:             Nothing
 */
void time_controller_start(struct time_controller *tc, struct window_controller *wc);


/*  This function eliminates a specific 'time_data' structure, identified by its
 *  sequence number (see ABSTRACT for detail).
 *
 *  Parameters:
 *  - tc:               Pointer to 'time_controller' through wich execute the operation
 *  - seq:              Sequence number of 'time_data' structure to delete
 *
 *  Return:             0 if the specific 'time_data' is not found, 1 otherwise.
 *                      If return value is 1, the specific 'time_data' is deleted.
 */
int time_controller_delete_timer(struct time_controller *tc, long long int seq);


/*  This function adds a new 'time_data' into the 'timer_wheel' data structure.
 *  The 'time_data' is added only if 'timer_wheel' is not full, otherwise
 *  the process/thread waits on condition 'full' until a signal is sent.
 *
 *  Parameters:
 *  - td:               The 'time_data' structure to be added
 *  - tc:               Pointer to 'time_controller' through wich execute the operation
 *
 *  Return:             Nothing
 */
void time_controller_add_new_timer(struct time_data *td, struct time_controller *tc);


/*  This function checks if the 'timer_wheel' included in the
 *  'time_controller' data structure is empty.
 *
 *  Parameters:
 *  - tc:               Pointer to 'time_controller' through wich execute the operation
 *
 *  Return:             1 if 'timer_wheel' is empty, otherwise 0
 */
int time_controller_is_empty(struct time_controller *tc);


/*  This function stops the thread safely, and kills it
 *
 *  Parameters:
 *  - tc:               Pointer to 'time_controller' through wich execute the operation
 *
 *  Return:             Nothing
 */
void time_controller_stop(struct time_controller *tc);


/*  This function frees all the memory occupied by the data structure and its parameters
 *
 *  Parameters:
 *  - tc:               Pointer to 'time_controller' through wich execute the operation
 *
 *  Return:             Nothing
 */
void time_controller_dispose(struct time_controller *tc);

#endif /* defined(__Reliable_UDP__time_controller__) */
