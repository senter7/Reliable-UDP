//
//  time_data.h
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
//  This header file contains the data structure 'time_data' that makes up,
//  with 'packet', the first level of N-layer architecture in the project.
//  This data structure is used to keep track of time sending and receipt of
//  each packet to which it is connected (see ABSTRACT in 'packet.h' for details).
//  In fact, 'packet' and 'time_data' are closely linked: each packet has a pointer
//  to a structure 'time_data', even if they are handled in a completely different
//  way. Both data structures share the sequence number.


#ifndef __Reliable_UDP__time_data__
#define __Reliable_UDP__time_data__

//SE time.h NON FUNZIONA PROVARE sys/time.h
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>


struct time_data {
    struct timeval time_send;   //Send time
    struct timeval time_recv;   //Time of receipt
    struct timeval timeout;     //Timeout
    struct timeval time_stamp;  //Not used in this release
    long long int seq;          //Sequence number
    
};


/*  This function creates a new and initializated 'time_data' data structure
 *
 *  Parameters:
 *  - seq:          Sequence number
 *
 *  Return:         Pointer to a 'time_data' data structure
 */
struct time_data *new_time_data(long long int seq);


#endif /* defined(__Reliable_UDP__time_data__) */
