//
//  log_msg.h
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
//  This header file contains the 'log_msg' data structure, wich is used to print
//  info messages to the user on the screen, or write it on a file.
//  Messages are equipped with many options, like data and time of the message.
//  It is possible convert a 'log_msg' to a string, selecting a level of detail.
//  In addition, each message has a type ('msg_type' enumeration) that allows you
//  to differentiate between different messages.


#ifndef __Reliable_UDP__log_msg__
#define __Reliable_UDP__log_msg__

#define MAX_LOG_MSG_STRING_SIZE 1024

#include <stdio.h>
#include <time.h>

/*Enumeration for message type*/
enum msg_type {LOG_ERROR, LOG_WARNING, LOG_INFO, LOG_SUCCESS};


/* Enumeration used to identify the level of detail that output
 * message must have. I.E.:
 *
 * DETAIL_LOW:        [ OK ] Sending file...
 * DETAIL_MEDIUM:     [ OK ] (14:23:44) Sending file...
 * DETAIL_HIGH:       [ OK ] (28/07/2015) (14:23:44) Sending file...
 * DETAIL_EXTREME:    [ OK ] (28/07/2015) (14:23:44) Sending file...
 *                           Operation: Sending the file "temp.txt" from socket
 */
enum msg_detail {DETAIL_LOW, DETAIL_MEDIUM, DETAIL_HIGH, DETAIL_EXTREME};


struct log_msg {
    int day;                //local day (not always used)
    int month;              //local month (not always used)
    int year;               //local year (not always used)
    
    int hour;               //local hour
    int min;                //local minutes
    int sec;                //local seconds
    
    char msg[128];          //message
    char type[16];          //message type
    char operation[256];    //operation description (not often used)
};


/* Create new message.
 * This function creates a valid structure for the log message,
 * with date, time and text.
 *
 * Params:
 * - msg:               Complete message (128 max)
 * - type:              Message type from enumeration
 * - operation:         Optional specification for operation (it is often used NULL)
 *
 * Return:              Pointer to a 'log_msg' structure
 */
struct log_msg* new_log_msg(char* msg, int type, char* operation);


/* Delete memory used to store message.
 *
 * Params:
 * - msg:               Pointer to 'log_msg' structure
 *
 * Return:              Nothing
 */
void log_msg_delete(struct log_msg* msg);


/* Create raw message.
 * This function creates a string from a 'log_msg' whit the level
 * of detail specified in 'detail' parameter.
 *
 * Params:
 * - msg:               Pointer to a 'log_msg' structure
 * - detail:            Level of detail for the message from 'msg_detail' enumeration
 * 
 * Return:              String that includes data on the structure, according to
 *                      the level of detail chosen
 */
char* log_msg_to_string(struct log_msg* msg, int detail);


#endif /* defined(__Reliable_UDP__log_msg__) */
