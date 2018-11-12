//
//  window_controller.c
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


#include "window_controller.h"

struct window_controller *new_window_controller(int dim, struct time_controller *tc, int sockfd, struct sockaddr_in addr, int output) {
    struct window_controller *wc;
    
    wc = malloc(sizeof(struct window_controller));
    if(wc == NULL) {
        fprintf(stderr, "Error in new_window_controller(): cannot allocate memory for window_controller\n");
        exit(EXIT_FAILURE);
    }
    //Allocate memory for sliding window
    wc->w = new_window(dim);
    //Initialize mutex
    if(pthread_mutex_init(&wc->MTX, NULL) != 0 || pthread_mutex_init(&wc->m_zero, NULL) != 0) {
        fprintf(stderr, "Error in newwindow_controller(): cannot initialize mutex\n");
        exit(EXIT_FAILURE);
    }
    //Initialize conditions
    if (pthread_cond_init((&wc->empty), NULL) != 0 || pthread_cond_init(&wc->full, NULL) != 0 ||
        pthread_cond_init((&wc->zero), NULL)) {
        fprintf(stderr, "Error in new_window_controller(): cannot create conditions\n");
        exit(EXIT_FAILURE);
    }

    wc->tc = tc;
    wc->dynamicTimeout.tv_sec = 0;  //Timeout is 0 because it is setted in...
    wc->dynamicTimeout.tv_usec = 0; //...window_controller_add_packet()
    wc->addr = addr;
    wc->sockfd = sockfd;
    wc->output = output;
    
    return wc;
}


int window_controller_is_full(struct window_controller *wc) {
    get_mutex(&wc->MTX);
    int res = window_is_full(wc->w);    //res = 1 if window is full, otherwise 0
    release_mutex(&wc->MTX);
    return res;
}

int window_controller_is_empty(struct window_controller *wc) {
    get_mutex(&wc->MTX);
    int res = window_is_empty(wc->w);   //res = 1 if window is empty, otherwise 0
    release_mutex(&wc->MTX);
    return res;
}

void window_controller_add_packet(struct window_controller *wc, struct packet *pkt) {
    
    get_mutex(&wc->MTX);    //get mutex

    //Wait until at least one slot is free into sliding window
    while (window_is_full(wc->w) == 1)
        pthread_cond_wait(&wc->full, &wc->MTX);
    
    
    //If this function is used by sender process, then tc != NULL
    //The sender process needs a 'time_controller' data structure
    if (wc->tc != NULL) {
        send_pkt(wc->sockfd, pkt, wc->addr); //send the pkt
        window_add_pkt(wc->w, pkt);          //add the pkt into sliding window
        pthread_cond_signal(&wc->empty);     //send a signal to all pending processes
                                             //suspended on condition 'empty'
        release_mutex(&wc->MTX);             //release mutex
    
        //If it did not receive any ACK, the timeout is configured with its
        //default value: DEFAULT_TIMEOUT_SEC and DEFAULT_TIMEOUT_USEC in 'settings.h'
        if (wc->dynamicTimeout.tv_sec == 0 && wc->dynamicTimeout.tv_usec == 0) {
            pkt->td->timeout.tv_sec = DEFAULT_TIMEOUT_SEC;
            pkt->td->timeout.tv_usec = DEFAULT_TIMEOUT_USEC;
        }
        //If it did receive at least one ACK, use the dynamic timeout value
        else {
            pkt->td->timeout.tv_sec = wc->dynamicTimeout.tv_sec;
            pkt->td->timeout.tv_usec = wc->dynamicTimeout.tv_usec;
        }
        //Add the 'time_data' data structure included in the pkt just added into
        //the 'timer_wheel' using this function from 'time_controller'
        time_controller_add_new_timer(pkt->td, wc->tc);
    }
    
    //If tc == NULL, then this function is used by the receiver process.
    //The receiver process doesn't need a 'time_controller' data structure
    else {
        window_add_pkt(wc->w, pkt);         //add the pkt into sliding window
        if (wc->sockfd != -5)               //for debug only!
            sort_window(wc->w);             //sort the sliding window
        release_mutex(&wc->MTX);            //release mutex
    }
}


long long int write_contiguous(struct window_controller *wc, long long int min) {
    long long int last = min;
    int deleted = 0;                    //This variables identifies if a pkt was written. It is used
                                        //to send signals to the condition 'full'
    get_mutex(&wc->MTX);
    
    struct packet *pkt;
    ssize_t m;
    int nE = wc->w->E;                  //Save the indexes to restore them at the end
    int nS = wc->w->S;                  //
    
    //Continuously until the window is empty
    while (window_is_empty(wc->w) == 0) {
        pkt = window_get_pkt(wc->w);    //get first pkt avaible
        
        //If seq number is equal to seq that i need...
        if (pkt->seq == last) {
            nE = wc->w->E;              //update the indexes...
            nS = wc->w->S;              //...and write data
            
            m = write(wc->output, (void *) pkt->data, pkt->dimension);
            if (m == -1 || (size_t) m != pkt->dimension) {
                fprintf(stderr, "Error in writeContiguous(): cannot write on output file\n");
                exit(EXIT_FAILURE);
            }
            last++;                     //update 'last'
            deleted = 1;                //set 'deleted' = 1
            //free(pkt);
        }
        //If seq number is not equal to seq that i need...
        else {
            wc->w->E = nE;              //restore indexes...
            wc->w->S = nS;              //
            break;                      //...and break
        }
        
        //If at least 1 pkt was written, send a signal to all processes pending
        //in condition 'full'
        if (deleted == 1)
            pthread_cond_signal(&wc->full);
    }
    wc->w->E = nE;
    wc->w->S = nS;
    
    release_mutex(&wc->MTX);            //release mutex
    
    //At the end, if the sliding window is empty, send a signal to all processes
    //pending on condition 'full'
    if (window_controller_is_empty(wc) == 1)
        pthread_cond_signal(&wc->zero);
    
    return last;                        //return last seq. number
}


void delete_contiguous(struct window_controller *wc) {
    get_mutex(&wc->MTX);                //get mutex
    
    //If window is empty, wait for condition 'empty'
    while (window_is_empty(wc->w) == 1)
        pthread_cond_wait(&wc->empty, &wc->MTX);
    
    struct packet *pkt = NULL;
    int nE = wc->w->E;                  //Save the indexes to restore them at the end
    int nS = wc->w->S;                  //
    
    //Continuously until the window is empty
    while (window_is_empty(wc->w) == 0) {
        pkt = window_get_pkt(wc->w);    //get first pkt avaible
        //A further check on the sliding window
        if (pkt != NULL) {
            if (pkt->acked == 0) {      //if pkt wasn't acked...
                wc->w->E = nE;          //...restore indexes...
                wc->w->S = nS;          //
                break;                  //...break
            }
            else {                      //if pkt was acked...
                nE = wc->w->E;          //...update indexes
                nS = wc->w->S;          //
                //free(pkt);            //send a signal
                pthread_cond_signal(&wc->full);
            }
            //pthread_cond_signal(&wc->full);
        }
        //free(pkt);
    }
    wc->w->E = nE;
    wc->w->S = nS;
    
    release_mutex(&wc->MTX);
    
    //At the end, if the sliding window is empty, send a signal to all processes
    //pending on condition 'full'
    if (window_controller_is_empty(wc) == 1)
        pthread_cond_signal(&wc->zero);
}

void calculate_dynamic_timeout(struct window_controller *wc) {
    //estimatedRTT = (1 - alpha) x estimatedRTT + alpha x sampleRTT
    wc->estimatedRTT = 0.875 * wc->estimatedRTT + 0.125 * wc->sampleRTT;
    
    //devRTT = (1 - beta) x devRTT + beta x |sampleRTT - estimatedRTT|
    wc->devRTT = 0.75 * wc->devRTT + 0.25 * fabsl(wc->sampleRTT - wc->estimatedRTT);
    
    //timeout interval = estimatedRTT + 4 x devRTT
    double timeout = wc->estimatedRTT + 4 * wc->devRTT;
    
    long int sec = timeout / 1000000;
    
    wc->dynamicTimeout.tv_sec = sec;
    wc->dynamicTimeout.tv_usec = timeout - sec * 1000000;
}


int window_controller_set_ack(struct window_controller *wc, long long int seq) {
    get_mutex(&wc->MTX);
    //Search the pkt with sequence number == seq
    struct packet *pkt = window_search_by_seq(wc->w, seq);
    
    if (pkt == NULL) {                                      //If the pkt doesn't exists...
        release_mutex(&wc->MTX);                            //...release mutex.
        return 0;                                           //...return 0
    }
    
    //If pkt != NULL, the the packet with sequence number == seq was found
    else {
        pkt->acked = 1;                                     //set this packet as acked
        gettimeofday(&pkt->td->time_recv, NULL);

        wc->sampleRTT = (pkt->td->time_recv.tv_sec -        //set the new sampleRTT
                         pkt->td->time_send.tv_sec) *       //using  the arrival time
                        1000000 +                           //and the send time of
                        (pkt->td->time_recv.tv_usec -       //the packet
                        pkt->td->time_send.tv_usec);        //
        
        //If this function is used by sender process, then wc->output must be -1
        if (wc->output == -1) {
            release_mutex(&wc->MTX);
            delete_contiguous(wc);                          //delete all contiguous pkt
            get_mutex(&wc->MTX);
        }
    }

    //A new ACK is arrived, so the dynamic timeout must be updated
    calculate_dynamic_timeout(wc);
    
    release_mutex(&wc->MTX);
    return 1;
}


int window_controller_resend_packet(struct window_controller *wc, long long int seq) {
    
    get_mutex(&wc->MTX);
    
    struct packet *pkt;
    //Search the pkt with sequence number == seq into sliding window
    pkt = window_search_by_seq(wc->w, seq);
    
    //If packet doesn't exists, return 1...
    if (pkt == NULL) {
        release_mutex(&wc->MTX);
        return 1;
    }
    
    //...otherwise
    else {
        pkt->retries = (pkt->retries) + 1;                  //increase retries
        if (pkt->retries == MAX_RETRIES_SENDING_PKT) {      //control max retries
            fprintf(stderr, "Generic error: lost connection or line too busy\n");
            exit(EXIT_FAILURE);
        }
        
        send_pkt(wc->sockfd, pkt, wc->addr);                //resend pkt
        
        gettimeofday(&pkt->td->time_send, NULL);            //update send time for pkt
        
        //Increase the timeout twice
        pkt->td->timeout.tv_sec = pkt->td->timeout.tv_sec * 2;
        pkt->td->timeout.tv_usec = pkt->td->timeout.tv_usec * 2;
        
        release_mutex(&wc->MTX);
        
        return 0;
    }
}

void window_controller_dispose(struct window_controller *wc) {
    struct packet *pkt = NULL;
    //Delete all pkt in the window
    while (window_is_empty(wc->w) == 0) {
        pkt = window_get_pkt(wc->w);
        free(pkt);
    }
    
    free(wc->w->buffer);        //delete buffer memory
    free(wc->w);                //delete window memory
    free(wc);                   //delete window_controller memory
}