//
//  get.h
//  Reliable UDP
//
//  Created by Simone Minasola on 06/11/15.
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
//  This header function contains one of the most important function of the
//  project. It has been necessary to create an header file because this
//  function is used by server and client, to ensure maximum code reuse.
//  'receive_file' prepares the server or the client to receive a file from the
//  network. it is advisable to view the comments within the code to study the
//  behavior of this function, because it is very complex.


#ifndef __Reliable_UDP__get__
#define __Reliable_UDP__get__

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "utils.h"
#include "settings.h"
#include "window_controller.h"
#include "time_controller.h"
#include "strings.h"
#include "server_status.h"
#include "timer.h"
#include "print_messages.h"


/*  See the ABSTRACT for details
 *
 *  Params:
 *  - filename:         File name of the file to receive
 *  - childPort:        Communication port that has been selected previously
 *  - pktsNumber:       Number of packets to receive. This number is related to 
 *                      the size of the field 'data' of each package. See
 *                      MAX_BLOCK_SIZE in 'settings.h' for more details.
 *  - user:             ID of the caller: LS_CLIENT for client, LS_SERVER fors server
 *  - ip:               The IP of the caller. The server use NULL
 *  - old_sockfd:       Socket file descriptor previously opened
 *  - verbose_mode:     0 if verbose mode is not activated, otherwise 1
 *  - status:           Pointer to a valid 'server_status' structure. The structure
 *                      must be created by 'create_shared_memory()' function to
 *                      ensure mutual exclusion while printing on file or screen.
 *                      See 'server_status.h' for details. The client use NULL.
 *  - log:              Pointer to a log file previously opened. If the log
 *                      service is unavailable, 'log' is NULL.
 *
 *  Return:             Nothing
 *
 *  Effects:
 *  Prepares the server or the client to receive a file
 */
void receive_file(char *filename, int childPort, long int pktsNumber, int user, char *ip,
                  int old_sockfd, int verbose_mode,
                  struct server_status *status, FILE *log);

#endif /* defined(__Reliable_UDP__get__) */
