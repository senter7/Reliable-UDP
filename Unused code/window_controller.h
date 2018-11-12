//
//  window_controller.h
//  New Reliable UDP
//
//  Created by Simone Minasola on 29/10/15.
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

#ifndef __New_Reliable_UDP__window_controller__
#define __New_Reliable_UDP__window_controller__

#include <stdio.h>

#include "window.h"
#include "time_controller.h"
#include "utils.h"

#include <netinet/in.h>

struct window_controller {
    struct tree_node *root;
    pthread_mutex_t MTX;
    pthread_cond_t full;
    pthread_cond_t empty;
    struct time_controller *tc;
    struct timeval dynamic_timeout;
    long double estimatedRTT;
    long double devRTT;
    long double sampleRTT;
    int sockfd;
    struct sockaddr_in addr;
    int output;
    int current_window_dimension;
};

struct window_controller *new_window_controller(struct time_controller *tc, int sockfd, struct sockaddr_in addr, int output);

void window_controller_add_packet(struct window_controller *wc, struct packet *pkt);

int window_controller_set_ack(struct window_controller *wc, long long int seq);

int window_controller_remaining_slots(struct window_controller *wc);

int window_controller_resend_packet(struct window_controller *wc, long long int seq);

long long int write_contiguous(struct window_controller *wc, long long int min);

void window_controller_dispose(struct window_controller *wc);

#endif /* defined(__New_Reliable_UDP__window_controller__) */
