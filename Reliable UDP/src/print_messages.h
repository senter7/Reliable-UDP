//
//  print_messages.h
//  Reliable UDP
//
//  Created by Simone Minasola on 18/11/15.
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
//  This header file contains many functions that have the task of writing messages
//  for the user, writing them on the screen and/or on the log file. It is preferred to divide
//  the user messages from the code for a more readable code. Each message, according
//  to appropriate criteria, is written only if the 'verbose mode' is active .
//  In addition, some messages are not written to the log file, so it is simpler
//  and immediate to read. For messages written by client, which has a unique
//  running process, it is used a semaphore type 'pthread_mutex_t' to synchronize
//  the writing between different threads. The messages written by the server,
//  however, are synchronized by a semaphore POSIX ANONYMOUS 'sem_t', to synchronize
//  various processes and threads.
//  These functions perform a series of simple 'printf()' with formatted text.
//  So it is useless to write detailed comments for each function.



#ifndef __Reliable_UDP__print_messages__
#define __Reliable_UDP__print_messages__

#include <stdio.h>

#include "server_status.h"
#include "strings.h"

/*  Example:
    |==================================|
    |  Welcome to Reliable UDP Client  |
    | Date 19.11.2015  Time [23:11:25] |
    | Server started:                  |
    | - Log service [ON]               |
    | - Verbose service [OFF]          |
    |==================================|  */
void print_server_welcome_msg(FILE *log, int verbose);



/*  Example:
    |==================================|
    |  Welcome to Reliable UDP Client  |
    | Date 19.11.2015  Time [23:11:25] |
    | Client started:                  |
    | - Log service [OFF]              |
    | - Verbose service [OFF]          |
    |==================================|  */
void print_client_welcome_msg(FILE *log, int verbose);

/* msg: max processes number reached. Request not accepted */
void print_max_processes_msg(FILE *log, struct server_status *status);

/* msg: request accepted */
void print_request_accepted(FILE *log, struct server_status *status);

/* msg: file not found */
void print_file_not_found_msg(FILE *log, struct server_status *status, char *data);

/* msg: selected port ('port') */
void print_port_msg(FILE *log, struct server_status *status, int verbose_mode, int port);

/* msg: completition: 'x'% */
void print_completition_msg(FILE *log, struct server_status *status, int user, int percentage);

/* msg: PKT_FIN just received */
void print_fin_arrived_msg(struct server_status *status, int user, int verbose_mode);

/* msg: PKT_FINACK just received */
void print_finack_arrived_msg(struct server_status *status, int user, int verbose_mode);

/* msg: operation interrupted */
void print_interrupted_operation_msg(FILE *log, struct server_status *status, int user);

/* msg: number of pkts to receive -->('pkt_number') */
void print_pkt_to_receive(FILE *log, struct server_status *status, int user, long int pkts_number);

/* msg: operation started */
void print_operation_started_msg(FILE *log, struct server_status *status, int user, const char *type);

/* msg: packet received */
void print_pkt_arrived_msg(struct server_status *status, int user, int verbose, long long int seq);

/* msg: ack received */
void print_ack_arrived_msg(struct server_status *status, int user, int verbose, long long int seq);

/* msg: PKT_ERR received */
void print_err_arrived_msg(FILE *log, struct server_status *status, int user, char *data);

/* msg: packet sent */
void print_pkt_sent_msg(struct server_status *status, int user, int verbose, long long int seq);

/* msg: waiting for request... */
void print_waiting_msg(FILE *log, struct server_status *status);

void print_close_connection(FILE *log, struct server_status *status, int user);

#endif /* defined(__Reliable_UDP__print_messages__) */
