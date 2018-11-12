//
//  time_controller.c
//  Reliable UDP
//
//  Created by Simone Minasola on 16/08/15.
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


#include "time_controller.h"

/*  This function represent the main work of the thread that is responsible for 
 *  controlling timeouts.
 *
 *  Parameters:
 *  - arg:      Represent a casting to 'stuct time_controller *'
 *
 *  Return:     Nothing
 */
void *threadWork(void *arg) {
    struct time_controller *tc = (struct time_controller *) arg;
    
    //Running until 'tc->running' == 1
    while (tc->running == 1) {
        struct timeval tv;
        struct time_data *td;
        
        get_mutex(&tc->MTX);
        
        //If 'timer_wheel' is empty, wait on condition 'empty'
        while (timer_wheel_is_empty(tc->tw) == 1)
            pthread_cond_wait(&tc->empty, &tc->MTX);
        
        int deleted = 0;        //If deleted != 0, at least one 'time_data' was deleted
        int nE = tc->tw->E;     //Save the indexes to restore them at the end
        int nS = tc->tw->S;     //
        
        //Continue until 'timer_wheel' is empty
        while (timer_wheel_is_empty(tc->tw) == 0) {
            
            gettimeofday(&tv, NULL);                    //Reference time to control the others
            
            td = timer_wheel_get_timer(tc->tw);         //Get first 'time_data' avaible
            
            //Control if timeout has expired, comparing this with the reference time.
            //If timeout has expired, seach for packet with sequence number == seq
            if ((tv.tv_sec - td->time_send.tv_sec) * 1000000 + (tv.tv_usec - td->time_send.tv_usec) > td->timeout.tv_sec * 1000000 + td->timeout.tv_usec) {
                //If the packet exists, resend it and update its 'time_send'
                if(window_controller_resend_packet(tc->wc, td->seq) == 0) {
                    gettimeofday(&td->time_send, NULL);
                }
                //If the packet doesn't exists, delete the timer. In fact, if packet doesn't
                //exists, it means that it was acked and deleted before
                else {
                    timer_wheel_delete_timer(tc->tw);   //Delete the timer
                    nE = (nE - 1) % tc->tw->dim;        //Set the index
                    deleted++;                          //Increase 'deleted' to send signal
                }
            }
        }
        
        tc->tw->E = nE;             //Restore the indexes
        tc->tw->S = nS;             //
        
        //If at least one 'time_data' was deleted, send a signal to all processes
        //pending on condition 'full'
        if (deleted > 0)
            pthread_cond_signal(&tc->full);
        
        release_mutex(&tc->MTX);    //release mutex
        
        msleep(tc->granularity);    //sleeps
        
    }
    
    //When 'running' is setted to 0 by 'time_controller_stop', the thread die
    pthread_exit(0);
}

struct time_controller *new_time_controller(unsigned int granularity, int dim, FILE *log, int user, struct server_status *status) {
    struct time_controller *tc;
    
    tc = malloc(sizeof(struct time_controller));   //Allocate memory for data structure
    if (tc == NULL) {
        fprintf(stderr, "Error in newtime_controller(): cannot allocate memory for time_controller\n");
        exit(EXIT_FAILURE);
    }
    
    tc->tw = new_timer_wheel(dim);  //Create a new 'timer_wheel'
    //Initialize mutex
    if(pthread_mutex_init(&tc->MTX, NULL) != 0) {
        fprintf(stderr, "Error in newtime_controller(): cannot initialize mutex\n");
        exit(EXIT_FAILURE);
    }
    //Initialize conditions
    if (pthread_cond_init((&tc->empty), NULL) != 0 || pthread_cond_init(&tc->full, NULL) != 0) {
        fprintf(stderr, "Error in newtime_controller(): cannot create conditions\n");
        exit(EXIT_FAILURE);
    }
    
    tc->granularity = granularity;  //set granularity
    tc->running = 0;                //at the beginning, the thread does not work
    tc->log = log;
    tc->user = user;
    tc->status = status;
    
    return tc;
}

void time_controller_start(struct time_controller *tc, struct window_controller *wc) {
    tc->wc = wc;
    tc->running = 1;    //set 'running' = 1 to start the thread
    
    //Create a new thread to execute the function 'thread_work'
    if(pthread_create(&tc->t, NULL, threadWork, tc) != 0) {
        fprintf(stderr, "Error in time_ontroller_tart(): cannot create new thread\n");
        exit(EXIT_FAILURE);
    }
}

int time_controller_delete_timer(struct time_controller *tc, long long int seq) {
    struct time_data *td = NULL;
    int res;
    
    get_mutex(&tc->MTX);
    
    int nE = tc->tw->E;     //Save the indexes to restore them at the end
    int nS = tc->tw->S;     //
    
    //Search a specified 'time_data' without restore the indexes
    td = timer_wheel_search_by_seq1(tc->tw, seq);
    //If the specified 'time_data' doesn't exists, return 0...
    if (td == NULL)
        res = 0;
    //...otherwise
    else {
        timer_wheel_delete_timer(tc->tw);   //delete the 'time_data' just found
        //nE--;                               //decrease index;
        nE = (nE - 1) % tc->tw->dim;
        res = 1;
        //pthread_cond_signal(&tc->full);     //send a signal for condition 'full'
    }
    
    tc->tw->E = nE;                         //restore the indexes
    tc->tw->S = nS;                         //
    
    release_mutex(&tc->MTX);
    
    if (res == 1)
        pthread_cond_signal(&tc->full);
    
    return res;
}

int time_controller_is_empty(struct time_controller *tc) {
    int res;
    get_mutex(&tc->MTX);
    res = timer_wheel_is_empty(tc->tw);
    release_mutex(&tc->MTX);
    return res;
}

void time_controller_add_new_timer(struct time_data *td, struct time_controller *tc) {
    get_mutex(&tc->MTX);

    //If 'timer_wheel' is full, wait on condition 'full'
    while (timer_wheel_is_full(tc->tw) == 1)
        pthread_cond_wait(&tc->full, &tc->MTX);
    
    //If 'timer_wheel' is not full...
    gettimeofday(&td->time_send, NULL);    //enter the current time
    timer_wheel_add(tc->tw, td);            //insert td into 'timer_wheel'
    
    //send a signal for condition 'empty'
    pthread_cond_signal(&tc->empty);
    
    release_mutex(&tc->MTX);
}

void time_controller_stop(struct time_controller *tc) {
    get_mutex(&tc->MTX);
    tc->running = 0;
    release_mutex(&tc->MTX);
}

void time_controller_dispose(struct time_controller *tc) {
    struct time_data *td = NULL;
    while (timer_wheel_is_empty(tc->tw) == 0) {
        td = timer_wheel_get_timer(tc->tw);
        free(td);
    }
    
    free(tc->tw->buffer);
    free(tc->tw);
    free(tc);
}