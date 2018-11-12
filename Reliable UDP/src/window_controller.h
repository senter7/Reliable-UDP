//
//  window_controller.h
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
//  This header file contains the 'window_controller' data structure and its functions,
//  that allow to interact whit it. The 'window_controller', as the name suggest, allows
//  you to manipulate the sliding window represent by 'window' data structure.
//  The major operations that this data structure can execute are:
//  - add a new packet in window, set a timer for it and send it
//  - set a packet 'acked' and remove all acked packets in order from the window
//  - write contiguous packet, starting from a specific sequence number
//  - resend a packet already added in window, and update retries and timer
//  One of the most important feature is the automatic calculation of the timer for
//  each packet added in the sliding window by 'window_controller_add_packet'.
//  In fact, for each ACK arrived, a new timer is calculated for the next packets to be send.
//  The 'sampleRTT' is updated for each ACK arrived, so the timeout can be different
//  for every packet.
//  All operations are synchronized with semaphores and condition, to ensuring mutual
//  exclusion for each process/thread.


#ifndef __Reliable_UDP__window_controller__
#define __Reliable_UDP__window_controller__


#include "window.h"
#include "time_controller.h"
#include "utils.h"
#include "settings.h"


struct window_controller {
    struct window *w;              //Pointer to a 'window' data structure
    pthread_mutex_t m_zero;        //Mutex that can be locked whel last pkt is sent
    pthread_mutex_t MTX;           //Mutex that can be locked to ensuring mutual exclusion for each process/thread
    pthread_cond_t full;           //Used to wait until it is free at least one slot in the window
    pthread_cond_t empty;          //Used to wait until it is full at least one slot in the window
    pthread_cond_t zero;           //Used to wait until window is empty
    struct time_controller *tc;    //Pointer to a 'time_controller' data structure (can be NULL)
    struct timeval dynamicTimeout; //Represent the timeout updated for each ACK received (in secs and usecs)
    long double estimatedRTT;      //Exponential weighted moving average (EWMA) for RTT
    long double devRTT;            //EWMA for deviance between sampleRTT and estimatedRTT
    long double sampleRTT;         //Sample of RTT time, updated for each ACK received
    int sockfd;                    //Communication socket
    struct sockaddr_in addr;       //Valid address structure
    int output;                    //File descriptor to writing (can be -1 if you don't have to write file)
};


/*  This function creates a new 'window_controller' data structure already initialized.
 *  The 'window' included in 'window_controller' it'll be created by own function.
 *
 *  Parameters:
 *  - dim:      The dimension of the sliding window. In this program, the value passed
 *              in this function is WINDOW_DIMENSION, a macro in 'settings.h'
 *  - tc:       Pointer to a 'time_controller' data structure. It must be NULL if the
 *              'window_controller' is used for receive file and not for send. In fact
 *              only the sender process can manipulate the timer for each packet, and
 *              the receiving process has it's own inactivity timer.
 *  - sockfd:   Communication socket already opened
 *  - addr:     A valid address
 *  - output:   File descriptor to write file. It must be -1 if the sender process
 *              is using a 'window_controller', because only receiving process
 *              have to write an output file
 *
 *  Return:     A pointer to a valid 'window_controller' data structure
 */
struct window_controller *new_window_controller(int dim, struct time_controller *tc, int sockfd, struct sockaddr_in addr, int output);


/*  This function allows to add a packet in the sliding window. The packet will
 *  be added only if window is not full. If it is full, the process will wait
 *  on the condition 'is_full' until a signal will be lauched.
 *
 *  Parameters:
 *  - dim:      Pointer to 'window_controller' through wich execute the adding operation
 *  - pkt:      The packet that have to be added
 *
 *  Return:     Nothing
 */
void window_controller_add_packet(struct window_controller *wc, struct packet *pkt);


/*  This function can set a specific packet (already added in sliding window) as acked.
 *  Also, if this function is used by sender process, all contiguous packets
 *  already added will be deleted. Otherwise, if this function is used by
 *  receiving process, the dynamic timout will be updated.
 *
 *  Parameters:
 *  - wc:       Pointer to 'window_controller' through wich execute the operation
 *  - seq:      Sequence number of the package for which it was received an ACK
 *
 *  Return:     0 if the packet was not found into sliding window, otherwise 1
 */
int window_controller_set_ack(struct window_controller *wc, long long int seq);


/*  This function checks if the sliding window ('window') included in the
 *  'window_controller' data structure is full.
 *
 *  Parameters:
 *  - wc:       Pointer to 'window_controller' through wich execute the operation
 *
 *  Return:     1 if sliding window is full, otherwise 0
 */
int window_controller_is_full(struct window_controller *wc);


/*  This function checks if the sliding window ('window') included in the
 *  'window_controller' data structure is empty.
 *
 *  Parameters:
 *  - wc:       Pointer to 'window_controller' through wich execute the operation
 *
 *  Return:     1 if sliding window is empty, otherwise 0
 */
int window_controller_is_empty(struct window_controller *wc);


/*  This function can resend a packet already added into sliding window. The
 *  research is done by sequence's packet number. When a packet is re-sent,
 *  the 'time_send' of the packet is updated, as the 'retries' and the
 *  timer. In particular, if the 'retries' of a packet is greaten than
 *  MAX_RETRIES_SENDING_PKT (macro in settings.h), then this means that the line 
 *  can be very busy (loss percentage setted too high in settings.h), and so
 *  the communication is stopped. Also, when a packet is re-sent, its timeout
 *  is increased twice, in according to the fact that the line can be very busy.
 *  This function is used automatically by 'time_controller' when a timeout
 *  has expired.
 *
 *  Parameters:
 *  - wc:       Pointer to 'window_controller' through wich execute the operation
 *  - seq:      Sequence number of the packet that have to be resend
 *
 *  Return:     1 if the packet was not found into sliding window, otherwise 0
 */
int window_controller_resend_packet(struct window_controller *wc, long long int seq);


/*  This function writes all contiguous packets into sliding window in a file,
 *  starting from a specific sequence number. The file that have to be written
 *  is identified by file descriptor 'output' saved into 'window_controller'
 *  data structure.
 *
 *  Parameters:
 *  - wc:       Pointer to 'window_controller' through wich execute the operation
 *  - seq:      Sequence number that represent the starting point to writing
 *              all contiguous packets
 *
 *  Return:     The last sequence number that will have to be written
 */
long long int write_contiguous(struct window_controller *wc, long long int min);


/*  This function frees all the memory occupied by the data structure and its parameters
 *
 *  Parameters:
 *  - wc:       Pointer to 'window_controller' through wich execute the operation
 *
 *  Return:     Nothing
 */
void window_controller_dispose(struct window_controller *wc);

#endif /* defined(__Reliable_UDP__window_controller__) */
