//
//  log_service.c
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

#include "log_service.h"

//#define ERROR_LAST_LOG_MSG  "The last message was not written in the log file\n"

struct log_service *new_log_service(const char *path, int detail, int user) {
    struct log_service *ls;
    FILE *log;
    struct log_msg *msg = new_log_msg(NULL, LOG_SUCCESS, NULL); //Mi serve solo la data
    
    ls = malloc(sizeof(struct log_service));
    if (ls == NULL) {
        fprintf(stderr, "Error in newlog_service(): cannot allocate memory for struct.\n");
        exit(EXIT_FAILURE);
    }
    
    //Server side
    //The server maintains a unique log file. If this file does not exist,
    //it will be created.
    if (user == LS_SERVER) {
        //Check if the file exists
        log = fopen(path, "r");
        
        //File doesn't exist
        if (log == NULL) {
            verbose_error_msg("Log file not found", NULL, DEFAULT_MSG_DETAIL);
            verbose_info_msg("Creating new log file...", NULL, DEFAULT_MSG_DETAIL);
            //Create new file
            log = fopen(path, "w");
            if (log == NULL) {
                fprintf(stderr, "Error in newlog_service(): cannot create log file\n");
                exit(EXIT_FAILURE);
            }
            else {
                verbose_success_msg("Log file created", NULL, DEFAULT_MSG_DETAIL);
                char temp [64] = "File name: \0";
                strncat(temp, path, 63);
                verbose_info_msg(temp, NULL, DEFAULT_MSG_DETAIL);
                strcpy(ls->filename, path);
                
                fprintf(log, "\n\n==============================\n          %d/%d/%d          \n==============================\n\n", msg->day, msg->month, msg->year);
            }
        }
        else {
            //File exists
            strcpy(ls->filename, path);
            
            if(fclose(log) < 0) {
                fprintf(stderr, "Error in newlog_service(): cannot close log file\n");
                exit(EXIT_FAILURE);
            }
            
            //File exists, so use the existing file
            log = fopen(path, "a");
            if (log == NULL) {
                fprintf(stderr, "Error in newlog_service(): cannot open log file\n");
                exit(EXIT_FAILURE);
            }
            else {
                verbose_success_msg("Log file found\n", NULL, DEFAULT_MSG_DETAIL);
                fprintf(log, "\n\n====================\n     %d/%d/%d     \n====================\n\n", msg->day, msg->month, msg->year);
            }
        }
    }
    
    //Client side
    //The client creates a log file for each process.
    else if (user == LS_CLIENT) {
        //Create unique file name
        char filename[32];
        sprintf(filename, "%d-%d-%d (%d.%d.%d).txt", msg->day, msg->month, msg->year, msg->hour, msg->min, msg->sec);
        
        //Create file with filename specified above
        log = fopen(filename, "w");
        if (log == NULL) {
            fprintf(stderr, "Error in newlog_service(): cannot create log file\n");
            exit(EXIT_FAILURE);
        }
        
        char temp [64] = "File name: \0";
        strcat(temp, filename);
        verbose_success_msg("Log file successfully created\n", NULL, DEFAULT_MSG_DETAIL);
        verbose_info_msg(temp, NULL, DEFAULT_MSG_DETAIL);
        
        strcpy(ls->filename, filename);
    }
    
    else {
        fprintf(stderr, "Error in newlog_service(): 'user' parameter not identified\n");
        exit(EXIT_FAILURE);
    }
    //Set detail parameter of data structure
    ls->detail = detail;
    
    //Close file
    if(fclose(log) < 0) {
        fprintf(stderr, "Error in newlog_service(): cannot close log file\n");
        exit(EXIT_FAILURE);
    }
    
    return ls;
}


void log_info_msg(struct log_service *ls, char *msg) {
    if (ls == NULL)
        return;
    
    struct log_msg *message = new_log_msg(msg, LOG_INFO, NULL);
    FILE *log;
    
    if ((log = fopen(ls->filename, "a")) == NULL)
        verbose_error_msg("Log file not found\n", NULL, DEFAULT_MSG_DETAIL);
    
    
    char *t = log_msg_to_string(message, ls->detail);
    if (fprintf(log, "%s\n", t) < 0)
        verbose_error_msg("The last message was not written in the log file\n", NULL, DEFAULT_MSG_DETAIL);
    
    
    if (fclose(log) < 0) {
        fprintf(stderr, "Error in logInfoMsg(): cannot close de log file\n");
        exit(EXIT_FAILURE);
    }
    
    log_msg_delete(message);
    free(t);
}

void log_error_msg(struct log_service *ls, char *msg) {
    if (ls == NULL)
        return;
    
    struct log_msg *message = new_log_msg(msg, LOG_ERROR, NULL);
    FILE *log;
    
    if ((log = fopen(ls->filename, "a")) == NULL)
        verbose_error_msg("Log file not found\n", NULL, DEFAULT_MSG_DETAIL);
    
    
    char *t = log_msg_to_string(message, ls->detail);
    if (fprintf(log, "%s\n", t) < 0)
        verbose_error_msg("The last message was not written in the log file\n", NULL, DEFAULT_MSG_DETAIL);
    
    
    if (fclose(log) < 0) {
        fprintf(stderr, "Error in logErrorMsg(): cannot close de log file\n");
        exit(EXIT_FAILURE);
    }
    
    log_msg_delete(message);
    free(t);
}

void log_warning_msg(struct log_service *ls, char *msg) {
    if (ls == NULL)
        return;
    
    struct log_msg *message = new_log_msg(msg, LOG_WARNING, NULL);
    FILE *log;
    
    if ((log = fopen(ls->filename, "a")) == NULL)
        verbose_error_msg("Log file not found\n", NULL, DEFAULT_MSG_DETAIL);
    
    
    char *t = log_msg_to_string(message, ls->detail);
    if (fprintf(log, "%s\n", t) < 0)
        verbose_error_msg("The last message was not written in the log file\n", NULL, DEFAULT_MSG_DETAIL);
    
    
    if (fclose(log) < 0) {
        fprintf(stderr, "Error in logWarningMsg(): cannot close de log file\n");
        exit(EXIT_FAILURE);
    }
    
    log_msg_delete(message);
    free(t);
}

void log_success_msg(struct log_service *ls, char *msg) {
    if (ls == NULL)
        return;
    
    struct log_msg *message = new_log_msg(msg, LOG_SUCCESS, NULL);
    FILE *log;
    
    if ((log = fopen(ls->filename, "a")) == NULL)
        verbose_error_msg("Log file not found\n", NULL, DEFAULT_MSG_DETAIL);
    
    char *t = log_msg_to_string(message, ls->detail);
    if (fprintf(log, "%s\n", t) < 0)
        verbose_error_msg("The last message was not written in the log file\n", NULL, DETAIL_MEDIUM);
    
    
    if (fclose(log) < 0) {
        fprintf(stderr, "Error in logSuccessMsg(): cannot close de log file\n");
        exit(EXIT_FAILURE);
    }
    
    log_msg_delete(message);
    free(t);
}
