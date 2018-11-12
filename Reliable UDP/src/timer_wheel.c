//
//  timer_wheel.c
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

#include "timer_wheel.h"

struct timer_wheel *new_timer_wheel(int dim) {
    struct timer_wheel *tw;
    //Allocate memory for data structure
    tw = malloc(sizeof(struct timer_wheel));
    if (tw == NULL) {
        fprintf(stderr, "Error in new_timer_wheel(): cannot allocate memory for timer_wheel\n");
        exit(EXIT_FAILURE);
    }
    //Allocate memory for array of 'dim' 'time_data' data structures
    tw->buffer = malloc(sizeof(struct time_data) * dim);
    if (tw->buffer == NULL) {
        fprintf(stderr, "Error in new_timer_wheel(): cannot allocate memory for buffer\n");
        exit(EXIT_FAILURE);
    }
    
    tw->E = 0;      //initialize the indexes
    tw->S = 0;      //
    tw->dim = dim;
    
    return tw;
}

int timer_wheel_is_empty(struct timer_wheel *tw) {
    //Circular array is empty if 'E' and 'S' has the same value
    if (tw->S == tw->E)
        return 1;
    else
        return 0;
}

int timer_wheel_is_full(struct timer_wheel *tw) {
    //Circular array is full if between 'E' and 'S' there is only one free slot
    int nE = (tw->E + 1) % tw->dim;
    if (nE == tw->S)
        return 1;
    else
        return 0;
}

int timer_wheel_add(struct timer_wheel *tw, struct time_data *td) {
    if (timer_wheel_is_full(tw) == 1)
        return 1;
    else {
        tw->buffer[tw->E] = *td;        //Add 'time_data' into position 'E'
        tw->E = (tw->E + 1) % tw->dim;  //Move 'E' of one position

        return 0;
    }
}

struct time_data *timer_wheel_get_timer(struct timer_wheel *tw) {
    struct time_data *td;
    
    if (timer_wheel_is_empty(tw) == 1)
        return NULL;
    else {
        td = &tw->buffer[tw->S];        //Read 'time_data' in position 'S'
        tw->S = (tw->S + 1) % tw->dim;  //Move 'S' of one position
        return td;
    }
}

struct time_data *timer_wheel_search_by_seq(struct timer_wheel *tw, long long int seq) {
    struct time_data *td = NULL;
    int nE;
    int nS;
    
    if (timer_wheel_is_empty(tw) == 1)
        return NULL;
    else {
        nE = tw->E;         //Save the indexes value to restore it at the end
        nS = tw->S;         //
        
        //Slide all the circular array, because the values ​​can not be ordered
        while (timer_wheel_is_empty(tw) == 0) {
            td = timer_wheel_get_timer(tw);
            if (td->seq == seq)     //If this is the value i'm looking for...
                break;              //...stop (sequence number are unique)
        }
        
        tw->E = nE;                 //Restore the prevoius values
        tw->S = nS;                 //
        
        //Last control before 'return'
        if (td != NULL && td->seq != seq)
            return NULL;
        
        return td;
    }
}

struct time_data *timer_wheel_search_by_seq1(struct timer_wheel *tw, long long int seq) {
    struct time_data *td = NULL;
    
    if (timer_wheel_is_empty(tw) == 1)
        return NULL;
    else {
        //Slide all the circular array, because the values ​​can not be ordered
        while (timer_wheel_is_empty(tw) == 0) {
            td = timer_wheel_get_timer(tw);
            if (td->seq == seq)     //If this is the value i'm looking for...
                break;              //...stop (sequence number are unique)
        }
        //Last control before 'return'
        if (td != NULL && td->seq != seq)
            return NULL;
        
        return td;
    }
}

void timer_wheel_delete_timer(struct timer_wheel *tw) {
    struct time_data *temp = &tw->buffer[(tw->S-1) % tw->dim];
    
    tw->buffer[tw->S-1] = tw->buffer[(tw->E-1)];    //Switch pointers
    tw->buffer[(tw->E-1)] = *temp;                  //
    tw->E = (tw->E - 1) % tw->dim;    //Update the indexes
    tw->S = (tw->S - 1) % tw->dim;    //
    
}

void timer_wheel_delete(struct timer_wheel *tw) {
    free(tw->buffer);   //Delete buffer memory
    free(tw);           //Delete struct memory
}