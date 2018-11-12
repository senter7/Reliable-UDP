//
//  packet.c
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

#include "packet.h"

struct packet *new_packet(int type, long long int seq, char *data, size_t dimension) {
    //Allocate memory for data structure
    struct packet *new = malloc(sizeof(struct packet));
    if(new == NULL) {
        fprintf(stderr, "Error in new_packet(): cannot allocate memory for packet\n");
        exit(EXIT_FAILURE);
    }
    //If 'data' is NULL, fill the 'data' field with '\0'
    if (data != NULL)
        memcpy(new->data, data, MAX_BLOCK_SIZE);
    else
        memcpy(new->data, "\0", MAX_BLOCK_SIZE);
    //Initialize all the parameters
    new->acked = 0;
    new->seq = seq;
    new->type = type;
    new->dimension = dimension;
    new->td = new_time_data(seq);
    new->retries = 0;
    
    return new;
}

void packet_delete(struct packet *pkt) {
    free(pkt);
}
