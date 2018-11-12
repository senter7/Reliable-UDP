//
//  timer.c
//  Reliable UDP
//
//  Created by Simone Minasola on 15/11/15.
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

#include "timer.h"


struct timer *new_timer() {
    struct timer *t = malloc(sizeof(struct timer));
    
    if (t == NULL) {
        perror("malloc() in new_time()");
        exit(EXIT_FAILURE);
    }
    
    t->final_time = 0.0;            //Initialize all the values
    t->initial_time = 0.0;          //
    t->last_time_catched = 0.0;     //
    t->total_time_catched = 0.0;    //
    
    return t;
}

void set_timer(struct timer *t, int MODE) {
    //In this function the local time is converted in seconds
    switch (MODE) {
        case TIMER_START:
            gettimeofday(&t->t, NULL);
            t->initial_time = pow(10,-6)*(t->t.tv_usec) + t->t.tv_sec;
            break;
        case TIMER_LAP:
            gettimeofday(&t->t, NULL);
            t->final_time = pow(10,-6)*(t->t.tv_usec) + t->t.tv_sec;
            t->last_time_catched = t->final_time - t->initial_time;
            t->total_time_catched += t->last_time_catched;
            t->initial_time = pow(10,-6)*(t->t.tv_usec) + t->t.tv_sec;
            break;
        case TIMER_RESET:
            t->final_time = 0.0;
            t->initial_time = 0.0;
            t->last_time_catched = 0.0;
            t->total_time_catched = 0.0;
            break;
    }
}

double get_last_time_catched(struct timer *t) {
    return t->last_time_catched;
}

double get_total_time_catched(struct timer *t) {
    return t->total_time_catched;
}