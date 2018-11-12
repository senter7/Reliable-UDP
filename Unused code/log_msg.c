//
//  log_msg.c
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

#include "log_msg.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


struct log_msg* new_log_msg(char* msg, int type, char* operation) {
    struct log_msg *message;
    struct tm *newtime;
    time_t t;
    //Allocate memory
    message = malloc(sizeof(struct log_msg));
    if(message == NULL) {
        fprintf(stderr, "Error in newlog_msg(): malloc() has failed\n");
        exit(EXIT_FAILURE); //I have to exit??
    }
    //Get the time since epoch
    time(&t);
    if (t == (time_t) -1) {
        fprintf(stderr, "Error in newlog_msg(): cannot retrieve time since epoch-time\n");
        exit(EXIT_FAILURE);
    }
    //Get local time and date
    errno = 0;
    newtime = localtime(&t);
    if (newtime == NULL || errno != 0) {
        fprintf(stderr, "Error in newlog_msg():  cannot retrieve local time and date\n");
        exit(EXIT_FAILURE);
    }
    
    message->day = newtime->tm_mday;            //Save the current day
    message->month = newtime->tm_mon + 1;       //Save the month
    message->year = 1900 + newtime->tm_year;    //Convert year
    
    message->hour = newtime->tm_hour;           //Save hours
    message->min = newtime->tm_min;             //Save  minutes
    message->sec = newtime->tm_sec;             //Save  seconds
    
    switch (type) {                             //Select the correct msg
        case LOG_ERROR:
            strcpy(message->type, "ERR ");
            break;
        case LOG_WARNING:
            strcpy(message->type, "WARN");
            break;
        case LOG_INFO:
            strcpy(message->type, "INFO");
            break;
        case LOG_SUCCESS:
            strcpy(message->type, " OK ");
            break;
    }
    
    if (msg != NULL)                            //Copy msg
        strcpy(message->msg, msg);
    
    if (operation != NULL)                      //Copy operation
        strcpy(message->operation, operation);
    
    return message;
}


void log_msg_delete(struct log_msg* msg) {
    free(msg);
}


char *log_msg_to_string(struct log_msg* msg, int detail) {
    char *message;
    int result = 0;
    errno = 0;
    
    message = malloc(sizeof(char) * MAX_LOG_MSG_STRING_SIZE);
    if(message == NULL || errno != 0) {
        fprintf(stderr, "Error in logMsgToString(): cannot allocate memory for msg\n");
        exit(EXIT_FAILURE);
    }
    
    //Write 'result' with correct detail level
    switch (detail) {
        case DETAIL_LOW:    //Only type and message
            result = sprintf(message, "[%s] %s", msg->type, msg->msg);
            break;
        case DETAIL_MEDIUM: //Type + time + message
            result = sprintf(message, "[%s] (%d:%d:%d) %s", msg->type, msg->hour,
                             msg->min, msg->sec, msg->msg);
            break;
        case DETAIL_HIGH:   //Type + date + time + message
            result = sprintf(message, "[%s] (%d/%d/%d) (%d:%d:%d) %s", msg->type,
                             msg->day, msg->month, msg->year, msg->hour, msg->min,
                             msg->sec, msg->msg);
            break;
        case DETAIL_EXTREME://Type + date + time + message + operation
            result = sprintf(message, "[%s] (%d/%d/%d) (%d:%d:%d) %s\nOperation: %s\n", msg->type,
                             msg->day, msg->month, msg->year, msg->hour, msg->min,
                             msg->sec, msg->msg, msg->operation);
            break;
    }
    
    if (result < 0) {
        fprintf(stderr, "Error in log_msg_to_string(): sprintf() has failed\n");
        exit(EXIT_FAILURE);
    }
    else
        return message;
}
