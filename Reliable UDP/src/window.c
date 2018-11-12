//
//  window.c
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

#include "window.h"


struct window *new_window(int dim) {
    struct window *w;
    
    w = malloc(sizeof(struct window));                  //allocate memory for 'window'
    if (w == NULL) {
        fprintf(stderr, "Error in new_window(): cannot allocate memory for window\n");
        exit(EXIT_FAILURE);
    }
    
    w->buffer = malloc(sizeof(struct packet) * dim);    //allocate memory for circular array
    if (w->buffer == NULL) {
        fprintf(stderr, "Error in new_window(): cannot allocate memory for buffer\n");
        exit(EXIT_FAILURE);
    }
    
    w->E = 0;       //Initialize the indexes
    w->S = 0;       //
    w->dim = dim;   //
    w->num = 0;     //Now, there isn't any packets into 'window'
    
    return w;
}

int window_is_empty(struct window *w) {
    //Circular array is empty if 'E' and 'S' has the same value
    if (w->S == w->E)
        return 1;
    else
        return 0;
}

int window_is_full(struct window *w) {
    //Circular array is full if between 'E' and 'S' there is only one free slot
    int nE = (w->E + 1) % w->dim;
    if (nE == w->S)
        return 1;
    else
        return 0;
}

int window_add_pkt(struct window *w, struct packet *pkt) {
    if (window_is_full(w) == 1)
        return 1;
    else {
        w->buffer[w->E] = *pkt;         //Add a packet in position 'E'
        w->E = (w->E + 1) % w->dim;     //Move 'E' of one position
        w->num++;                       //Update numbers of pkts in 'window'
        return 0;
    }
}

struct packet *window_get_pkt(struct window *w) {
    struct packet *pkt;
    
    if (window_is_empty(w) == 1)
        return NULL;
    else {
        pkt = &w->buffer[w->S];         //Get packet from position 'S'
        w->S = (w->S + 1) % w->dim;     //Move 'S' of one position
        w->num--;                       //Update numbers of pkts in 'window'
        return pkt;
    }
}

struct packet *window_search_by_seq(struct window *w, long long int seq) {
    struct packet *pkt = NULL;
    int nE;
    int nS;
    
    if (window_is_empty(w) == 1)
        return NULL;
    else {
        nE = w->E;                       //Save the indexes value to restore it at the end
        nS = w->S;                       //
        
        while (window_is_empty(w) == 0) {//Search in the circular buffer
            pkt = window_get_pkt(w);     //the pkt with sequence number == seq
            if (pkt->seq == seq)         //
                break;
        }
        
        w->E = nE;                       //Restore old values
        w->S = nS;                       //

        if (pkt != NULL && pkt->seq != seq)
            return NULL;
        
        return pkt;
    }
}

void window_delete(struct window *w) {
    free(w->buffer);                    //delete buffer memory
    free(w);                            //delete struct memory
}