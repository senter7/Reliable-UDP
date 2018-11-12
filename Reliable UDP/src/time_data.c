//
//  time_data.c
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

#include "time_data.h"

struct time_data *new_time_data(long long int seq) {
    struct time_data *td;
    
    td = malloc(sizeof(struct time_data));
    if(td == NULL) {
        fprintf(stderr, "Error in new_time_data(): cannote allocate memory for new time_data structure\n");
        exit(EXIT_FAILURE);
    }
    
    td->seq = seq;
    //Set the time of sending
    gettimeofday(&td->time_send, NULL);
    
    return td;
}

