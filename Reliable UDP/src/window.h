//
//  window.h
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
//
//
//  ABSTRACT
//
//  This header file contains the 'window' data structure and the functions
//  that allows to manage it. The 'window' is basically a circular array,
//  like 'timer_wheel' data structure. Indexes 'E' and 'S' are respectively
//  the first position free, ready to be write, and the first position full,
//  ready to be read. In the architecture of the program (N-layer), 'window'
//  and its functions are placed in a layer below 'window_controller' data
//  structure. In fact, all this functions are used into functions of
//  'window_controller'. The data structure 'packet' is the basic unit with
//  which 'window' interacts. In fact 'packet' is in the lower layer of
//  the architecture as 'time_data'.


#ifndef __Reliable_UDP__window__
#define __Reliable_UDP__window__

#include "packet.h"

#include <stdio.h>
#include <stdlib.h>

struct window {
    int E;                  //Index: first position free
    int S;                  //Index: first position full
    int dim;                //Dimension of the array
    struct packet *buffer;  //Array of 'packet' structures
    int num;                //Number of 'packet' currently in the buffer
};


/*  This function creates a new initialized 'window' data structure
 *
 *  Parameters:
 *  - dim:      Dimension of circular array
 *
 *  Return:     Pointer to a new initializated 'window'
 */
struct window *new_window(int dim);


/*  This function adds a new 'packet' into a 'window'.
 *  This operation involves moving the index 'E' of one position.
 *
 *  Parameters:
 *  - w:        Pointer to 'window' through wich execute the operation
 *  - pkt:      'packet' structure to add into 'window'
 *
 *  Return:     1 if the 'window' is full and it is impossible to add
 *              another 'packet', 0 on success
 */
int window_add_pkt(struct window *w, struct packet *pkt);


/*  This function gets the first 'packet' avaible into circular array.
 *  This operation involves moving the index 'S' of one position.
 *
 *  Parameters:
 *  - w:        Pointer to 'window' through wich execute the operation
 *
 *  Return:     NULL if the 'window' is empty and it is impossible to get
 *              a 'packet', otherwise the pointer to first avaible 'packet'
 */
struct packet *window_get_pkt(struct window *w);


/*  This function allows to search a specific 'packet', identified by its
 *  sequence number, into the circular array. At the end of this operation,
 *  the indexes are reset to their previous values.
 *
 *  Parameters:
 *  - w:        Pointer to 'window' through wich execute the operation
 *  - seq:      Sequence number to search
 *
 *  Return:     Pointer to 'packet' with sequence number == seq. If there isn't
 *              a 'packet' with sequence number == seq, return NULL
 */
struct packet *window_search_by_seq(struct window *w, long long int seq);


/*  This function frees all memory occupied by a 'window'
 *
 *  Parameters:
 *  - w:        Pointer to 'window' through wich execute the operation
 *
 *  Return:     Nothing
 */
void window_delete(struct window *w);


/*  This function checks if the circular array is empty.
 *
 *  Parameters:
 *  - w:        Pointer to 'window' through wich execute the operation
 *
 *  Return:     1 if 'window' is empty, otherwise 0
 */
int window_is_empty(struct window *w);


/*  This function checks if the circular array is full.
 *
 *  Parameters:
 *  - w:        Pointer to 'window' through wich execute the operation
 *
 *  Return:     1 if 'window' is full, otherwise 0
 */
int window_is_full(struct window *w);


#endif /* defined(__Reliable_UDP__window__) */
