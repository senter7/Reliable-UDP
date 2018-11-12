//
//  timer.h
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
//
//
//  ABSTRACT
//
//  This header file contains a simple data structure that allows you to calculate
//  the execution time of an operation. The execution time is calculated in seconds.


#ifndef __Reliable_UDP__timer__
#define __Reliable_UDP__timer__

#include <sys/time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/*  This enumeration contains the three possible operations:
 *
 *  TIMER_START:    start the timer by capturing the current time
 *  TIMER_LAP:      capturing the time since 'TIMER_START' without stopping the
 *                  timer and updating 'total_time_catched' value.
 *  TIMER_RESET:    reset the timer to 0
 */
enum timer_operation {TIMER_START, TIMER_LAP, TIMER_RESET};

struct timer {
    double initial_time;        //Initial time: is calculated for every 'lap'
    double final_time;          //Final time: is calculated for ever 'lap'
    double last_time_catched;   //Last time catched between each lap
    double total_time_catched;  //Total time catched since the timer was started
    struct timeval t;           //Where i save the current time with 'gettimeofday'
};

/*  This function creates a new timer.
 *
 *  Parameters:     Nothing
 *
 *  Return:         Pointer to a 'timer' data structure
 */
struct timer *new_timer();


/*  This is the main function which is used to set the timer. This function
 *  allows you to start and stop the timer, to capture the total and partial
 *  time elapsed and reset the timer.
 *
 *  Parameters:
 *  - t:            Pointer to a timer
 *  - MODE:         One of the three options avaible in 'timer_operation' enumeration
 *
 *  Return:         Noting
 */
void set_timer(struct timer *t, int MODE);


/* Return the last time catched after a TIMER_LAP */
double get_last_time_catched(struct timer *t);


/* Return the total time elapsed after one or more TIMER_LAP */
double get_total_time_catched(struct timer *t);

#endif /* defined(__Reliable_UDP__timer__) */
