//
//  verbose_service.c
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

#include "verbose_service.h"

void verbose_success_msg(char *msg, char *operation, int detail) {
    struct log_msg *message = new_log_msg(msg, LOG_SUCCESS, operation);
    char *string = log_msg_to_string(message, detail);
    fprintf(stdout, "%s\n", string);
    fflush(stdout);
    
    free(string);
    log_msg_delete(message);
}

void verbose_info_msg(char *msg, char *operation, int detail) {
    struct log_msg *message = new_log_msg(msg, LOG_INFO, operation);
    char *string = log_msg_to_string(message, detail);
    fprintf(stdout, "%s\n", string);
    fflush(stdout);
    
    free(string);
    log_msg_delete(message);
}

void verbose_error_msg(char *msg, char *operation, int detail) {
    struct log_msg *message = new_log_msg(msg, LOG_ERROR, operation);
    char *string = log_msg_to_string(message, detail);
    fprintf(stderr, "%s\n", string);
    fflush(stderr);
    
    free(string);
    log_msg_delete(message);
}

void verbose_warning_msg(char *msg, char *operation, int detail) {
    struct log_msg *message = new_log_msg(msg, LOG_WARNING, operation);
    char *string = log_msg_to_string(message, detail);
    fprintf(stderr, "%s\n", string);
    fflush(stderr);
    
    free(string);
    log_msg_delete(message);
}
