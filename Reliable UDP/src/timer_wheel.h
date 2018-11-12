//
//  timer_wheel.h
//  Reliable UDP
//
//  Created by Simone Minasola on 13/08/15.
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
//  This header file contains the 'timer_wheel' data structure and the functions
//  that allows to manage it. The 'timer_wheel' is basically a circular array,
//  like 'window' data structure. Indexes 'E' and 'S' are respectively
//  the first position free, ready to be write, and the first position full,
//  ready to be read. In the architecture of the program (N-layer), 'timer_wheel'
//  and its functions are placed in a layer below 'time_controller' data
//  structure. In fact, all this functions are used into functions of
//  'time_controller'. The data structure 'time_data' is the basic unit with
//  which 'timer_wheel' interacts. In fact 'time_data' is in the lower layer of
//  the architecture as 'packet'.


#ifndef __Reliable_UDP__timer_wheel__
#define __Reliable_UDP__timer_wheel__

#include "time_data.h"

#include <stdio.h>
#include <stdlib.h>

struct timer_wheel {
    int E;                      //Index: first position free
    int S;                      //Index: first position full
    int dim;                    //Dimension of the array
    struct time_data *buffer;   //Array of 'time_data' structures
};


/*  This function creates a new and initializated 'timer_wheel' with 'dim' 
 *  slots avaible.
 *
 *  Parameters:
 *  - dim:      dimension of the circular array
 *
 *  Return:     Pointer to an initializated 'timer_wheel' data structure
 */
struct timer_wheel *new_timer_wheel(int dim);


/*  This function adds a new 'time_data' into 'timer_wheel'.
 *  This operation involves moving the index 'E' of one position.
 *
 *  Parameters:
 *  - tw:       Pointer to 'timer_wheel' through wich execute the operation
 *  - td:       'time_data' structure to add into 'timer_wheel'
 *
 *  Return:     1 if the 'timer_wheel' is full and it is impossible to add
 *              another 'time_data', 0 on success
 */
int timer_wheel_add(struct timer_wheel *tw, struct time_data *td);


/*  This function gets the first 'time_data' avaible into circular array.
 *  This operation involves moving the index 'S' of one position.
 *
 *  Parameters:
 *  - tw:       Pointer to 'timer_wheel' through wich execute the operation
 *
 *  Return:     NULL if the 'timer_wheel' is empty and it is impossible to get
 *              a 'time_data', otherwise the pointer to first avaible 'time_data'
 */
struct time_data *timer_wheel_get_timer(struct timer_wheel *tw);


/*  This function allows to search a specific 'time_data', identified by its
 *  sequence number, into the circular array. At the end of this operation, 
 *  the indexes are reset to their previous values.
 *
 *  Parameters:
 *  - tw:       Pointer to 'timer_wheel' through wich execute the operation
 *  - seq:      Sequence number to search
 *
 *  Return:     Pointer to 'time_data' with sequence number == seq. If there isn't
 *              a 'time_data' with sequence number == seq, return NULL
 */
struct time_data *timer_wheel_search_by_seq(struct timer_wheel *tw, long long int seq);


/*  This function has the same behavior of 'timer_wheel_search_by_seq', but
 *  the indexes 'E' and 'S' are NOT restored to their previous values.
 *
 *  Parameters:
 *  - tw:       Pointer to 'timer_wheel' through wich execute the operation
 *  - seq:      Sequence number to search
 *
 *  Return:     Pointer to 'time_data' with sequence number == seq. If there isn't
 *              a 'time_data' with sequence number == seq, return NULL
 */
struct time_data *timer_wheel_search_by_seq1(struct timer_wheel *tw, long long int seq);


/*  This funcrion frees all memory occupied by a 'timer_wheel'
 *
 *  Parameters:
 *  - tw:       Pointer to 'timer_wheel' through wich execute the operation
 *
 *  Return:     Nothing
 */
void timer_wheel_delete(struct timer_wheel *tw);


/*  This function checks if the circular array is empty.
 *
 *  Parameters:
 *  - tw:       Pointer to 'timer_wheel' through wich execute the operation
 *
 *  Return:     1 if 'timer_wheel' is empty, otherwise 0
 */
int timer_wheel_is_empty(struct timer_wheel *tw);


/*  This function checks if the circular array is full.
 *
 *  Parameters:
 *  - tw:       Pointer to 'timer_wheel' through wich execute the operation
 *
 *  Return:     1 if 'timer_wheel' is full, otherwise 0
 */
int timer_wheel_is_full(struct timer_wheel *tw);


/*  This function deletes the first position ready to be read in the
 *  circular array. To exeute this operation, the function switches the first
 *  position ready to be write with the first position ready to be read.
 *  Then, the indexes are update, so the 'time_data' can be excluded.
 *  This function must be called only after a 'timer_wheel_get_timer'. In this way,
 *  the indexes are positioned in the correct manner.
 *  In example:
 *
 *------------------------------------------------------------------------------
 *  At the beginning:
 *   -----------------------
 *  |   | 4 | 5 | 6 |   |   |
 *   -----------------------
 *        S           E
 *
 *------------------------------------------------------------------------------
 *
 *  Call 'timer_wheel_get_timer':
 *
 *  struct time_data *td = timer_wheel_get_timer(tw)
 *  td = 4
 *
 *   -----------------------
 *  |   | 4 | 5 | 6 |   |   |
 *   -----------------------
 *            S       E
 *
 *------------------------------------------------------------------------------
 *
 *  Call 'timer_wheel_delete_timer' to delete '4'
 *
 *   -----------------------
 *  |   | 6 | 5 | 4 |   |   |
 *   -----------------------
 *        S       E
 *
 *------------------------------------------------------------------------------
 *
 *  From now, the next call to 'timer_wheel_add' will write a new 'time_data' in E,
 *  and the next call to 'timer_wheel_get_timer' will read from S.
 *
 *  td = 7
 *  Call timer_wheel_add(tw, td)
 *
 *   -----------------------
 *  |   | 6 | 5 | 7 |   |   |
 *   -----------------------
 *        S           E
 *
 *------------------------------------------------------------------------------
 *
 *  Parameters:
 *  - tw:       Pointer to 'timer_wheel' through wich execute the operation
 *
 *  Return:     Nothing
 */
void timer_wheel_delete_timer(struct timer_wheel *tw);



#endif /* defined(__Reliable_UDP__timer_wheel__) */
