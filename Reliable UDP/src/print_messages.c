//
//  print_messages.c
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

#include "print_messages.h"

/* This mutex allows mutual exclusion between threads in the client */
pthread_mutex_t CLIENT_MUTEX = PTHREAD_MUTEX_INITIALIZER;



void print_waiting_msg(FILE *log, struct server_status *status) {
    /* msg: waiting for request... */
    get_sem(status);
    printf("%s %s: %s\n", get_current_time(), _(STRING_MAIN_PROCESS),
           _(STRING_WAITING_FOR_REQUEST));
    fflush(stdout);
    if (log) {
        fprintf(log,"%s %s: %s\n", get_current_time(), _(STRING_MAIN_PROCESS),
                _(STRING_WAITING_FOR_REQUEST));
        fflush(log);
    }
    release_sem(status);
    /* end msg */
}

void print_max_processes_msg(FILE *log, struct server_status *status) {
    /* msg: max processes number reached. Request not accepted */
    get_sem(status);
    printf("%s %s: %s\n", get_current_time(), _(STRING_MAIN_PROCESS),
               _(STRING_MAX_PROCESSES_NUM));
    fflush(stdout);
    if (log) {
        fprintf(log, "%s %s: %s\n", get_current_time(), _(STRING_MAIN_PROCESS),
                _(STRING_MAX_PROCESSES_NUM));
        fflush(log);
    }
    release_sem(status);
    /* end msg */
}

void print_request_accepted(FILE *log, struct server_status *status) {
    /* msg: request accepted */
    get_sem(status);
    printf("%s %s: %s\n", get_current_time(),_(STRING_MAIN_PROCESS),
           _(STRING_REQUEST_ACCEPTED));
    fflush(stdout);
    if (log) {
        fprintf(log, "%s %s: %s\n", get_current_time(),_(STRING_MAIN_PROCESS),
                _(STRING_REQUEST_ACCEPTED));
        fflush(log);
    }
    release_sem(status);
    /* end msg */
}

void print_port_msg(FILE *log, struct server_status *status, int verbose_mode, int port) {
    /* msg: selected port ('port') */
    get_sem(status);
    if (verbose_mode)
        printf("%s %s: %s (%d)\n", get_current_time(), _(STRING_MAIN_PROCESS),
               _(STRING_SELECTED_PORT), port);
    printf("%s %s: %s\n", get_current_time(), _(STRING_MAIN_PROCESS),
           _(STRING_CREATING_CHILD));
    fflush(stdout);
    if (log) {
        fprintf(log, "%s %s: %s (%d)\n", get_current_time(), _(STRING_MAIN_PROCESS),
                _(STRING_SELECTED_PORT), port);
        fprintf(log, "%s %s: %s\n", get_current_time(), _(STRING_MAIN_PROCESS),
                _(STRING_CREATING_CHILD));
        fflush(log);
    }
    release_sem(status);
    /* end msg */
}

void print_file_not_found_msg(FILE *log, struct server_status *status, char *data) {
    get_sem(status);
    printf("%s %s: %s <%s>\n", get_current_time(),_(STRING_MAIN_PROCESS),
           _(STRING_FILE_NOT_FOUND), data);
    fflush(stdout);
    if (log) {
        fprintf(log, "%s %s: %s <%s>\n", get_current_time(),_(STRING_MAIN_PROCESS),
                _(STRING_FILE_NOT_FOUND), data);
        fflush(log);
    }
    release_sem(status);
}

void print_server_welcome_msg(FILE *log, int verbose_mode) {
    printf("\n\n|==================================|\n");                           //
    printf("|%s  |\n| %s %s  %s %s |\n", _(STRING_SERVER_HELLO), _(STRING_DATE),//
           get_current_data(), _(STRING_TIME),    //
           get_current_time());                   //
    printf("| %s:                  |\n| - %s ", _(STRING_SERVER_STARTED),
           _(STRING_LOG_SERVICE));
    if (log) {
        fprintf(log, "================================\n");
        fprintf(log,"%s %s  %s %s          \n", _(STRING_DATE), get_current_data(), _(STRING_TIME), get_current_time());
        fprintf(log, "================================\n");
        printf("[%s]               |", _(STRING_ON));
        fflush(log);
    }
    else printf("[%s]              |", _(STRING_OFF));
    printf("\n| - %s ",   _(STRING_V_SERVICE));
    if (verbose_mode == 1)
        printf("[%s]           |",  _(STRING_ON));
    else printf("[%s]          |",    _(STRING_OFF));
    printf("\n|==================================|\n\n");
    fflush(stdout);
}

void print_client_welcome_msg(FILE *log, int verbose) {
    printf("\n\n|==================================|\n");                           //
    printf("|%s  |\n| %s %s  %s %s |\n", _(STRING_CLIENT_HELLO), _(STRING_DATE),//
           get_current_data(), _(STRING_TIME),    //
           get_current_time());                   //
    printf("| %s:                  |\n| - %s ", _(STRING_CLIENT_STARTED),
           _(STRING_LOG_SERVICE));
    if (log) {
        fprintf(log, "==============================\n");
        fprintf(log,"          %s          \n", get_current_time());
        fprintf(log, "==============================\n");
        printf("[%s]               |", _(STRING_ON));
        fflush(log);
    }
    else printf("[%s]              |", _(STRING_OFF));
    printf("\n| - %s ",   _(STRING_V_SERVICE));
    if (verbose == 1)
        printf("[%s]           |",  _(STRING_ON));
    else printf("[%s]          |",    _(STRING_OFF));
    printf("\n|==================================|\n\n");
    fflush(stdout);
}

void print_completition_msg(FILE *log, struct server_status *status, int user, int percentage) {
    /* msg: completition: 'x'% */
    if (user == LS_CLIENT)
        get_mutex(&CLIENT_MUTEX);
    else
        get_sem(status);
    if (user == LS_SERVER) {
        printf("%s %s (%4d): %s %d%%\n", get_current_time(), _(STRING_CHILD),
               getpid(), _(STRING_COMPLETITION), percentage);
        if (log)
            fprintf(log, "%s %s (%d): %s %d%%\n", get_current_time(),
                    _(STRING_CHILD), getpid(), _(STRING_COMPLETITION),
                    percentage);
    }
    else {
        printf("%s %s %d%%\n", get_current_time(), _(STRING_COMPLETITION),
               percentage);
        if (log)
            fprintf(log, "%s %s %d%%\n", get_current_time(),
                    _(STRING_COMPLETITION), percentage);
    }
    
    fflush(stdout);
    if (log)
        fflush(log);
    
    if (user == LS_CLIENT)
        release_mutex(&CLIENT_MUTEX);
    else
        release_sem(status);
    /* end msg */
}

void print_fin_arrived_msg(struct server_status *status, int user, int verbose_mode) {
    /* msg: PKT_FIN just received */
    get_sem(status);
    if (user == LS_SERVER) {
        if (verbose_mode) {
            printf("%s %s (%4d): %s\n", get_current_time(), _(STRING_CHILD),
                   getpid(), _(STRING_PKT_FIN));
            fflush(stdout);
        }
    }
    else
        if (verbose_mode) {
            printf("%s %s\n", get_current_time(), _(STRING_PKT_FIN));
            fflush(stdout);
        }
    release_sem(status);
    /* end msg */
}

void print_finack_arrived_msg(struct server_status *status, int user, int verbose_mode) {
    /* msg: PKT_FIN just received */
    if (user == LS_CLIENT)
        get_mutex(&CLIENT_MUTEX);
    else
        get_sem(status);
    if (user == LS_SERVER) {
        if (verbose_mode) {
            printf("%s %s (%4d): %s\n", get_current_time(), _(STRING_CHILD),
                   getpid(), _(STRING_FINACK_RECEIVED));
            fflush(stdout);
        }
    }
    else
        if (verbose_mode) {
            printf("%s %s\n", get_current_time(), _(STRING_FINACK_RECEIVED));
            fflush(stdout);
        }
    if (user == LS_CLIENT)
        release_mutex(&CLIENT_MUTEX);
    else
        release_sem(status);
    /* end msg */
}


void print_interrupted_operation_msg(FILE *log, struct server_status *status, int user) {
    get_sem(status);
    if (user == LS_SERVER) {
        printf("%s %s (%4d): %s. %s\n", get_current_time(), _(STRING_CHILD),
               getpid(), _(STRING_OPERATION_INTERRUPTED),
               _(STRING_MAX_INACTIVITY_TIME_ELAPSED));
        printf("%s %s (%4d): %s\n", get_current_time(), _(STRING_CHILD),
               getpid(), _(STRING_CLOSE_CONNECTION));
        if (log) {
            fprintf(log, "%s %s (%4d): %s. %s\n", get_current_time(), _(STRING_CHILD),
                    getpid(), _(STRING_OPERATION_INTERRUPTED),
                    _(STRING_MAX_INACTIVITY_TIME_ELAPSED));
            fprintf(log, "%s %s (%4d): %s\n", get_current_time(), _(STRING_CHILD),
                   getpid(), _(STRING_CLOSE_CONNECTION));
        }
    }
    else {
        printf("%s %s. %s\n%s, %s\n", get_current_time(), _(STRING_OPERATION_INTERRUPTED),
               _(STRING_MAX_INACTIVITY_TIME_ELAPSED), _(STRING_LINE_BUSY),
               _(STRING_TRY_LATER));
        if (log)
            fprintf(log, "%s %s. %s\n%s, %s\n", get_current_time(), _(STRING_OPERATION_INTERRUPTED),
                    _(STRING_MAX_INACTIVITY_TIME_ELAPSED), _(STRING_LINE_BUSY),
                    _(STRING_TRY_LATER));
            fflush(log);
            fclose(log);
            }
    fflush(stdout);
    release_sem(status);
}

void print_pkt_to_receive(FILE *log, struct server_status *status, int user, long int pkts_number) {
    get_sem(status);
    if (user == LS_SERVER) {
        printf("%s %s (%4d): %s -> (%ld)\n", get_current_time(),_(STRING_CHILD),
               getpid(), _(STRING_PKTS_TO_RECV), pkts_number);
        if (log)
            fprintf(log, "%s %s (%4d): %s -> (%ld)\n", get_current_time(),_(STRING_CHILD),
                   getpid(), _(STRING_PKTS_TO_RECV), pkts_number);
    }
    else {
        printf("%s %s -> (%ld)\n", get_current_time(), _(STRING_PKTS_TO_RECV),
               pkts_number);
        if (log)
            fprintf(log, "%s %s -> (%ld)\n", get_current_time(), _(STRING_PKTS_TO_RECV),
                   pkts_number);
    }
    
    
    fflush(stdout);
    if (log)
        fflush(log);
    
    release_sem(status);
}


void print_operation_started_msg(FILE *log, struct server_status *status, int user, const char *type) {
    /* msg: operation started */
    get_sem(status);
    if (user == LS_SERVER) {
        printf("%s %s (%4d): %s [%s]\n", get_current_time(),_(STRING_CHILD), getpid(),
               _(STRING_OPERATION_STARTED), type);
        if (log)
            fprintf(log, "%s %s (%4d): %s [%s]\n", get_current_time(),_(STRING_CHILD), getpid(),
                   _(STRING_OPERATION_STARTED), type);
    }
    else {
        printf("%s %s [%s]\n", get_current_time(), _(STRING_OPERATION_STARTED), type);
        if (log)
            fprintf(log, "%s %s [%s]\n", get_current_time(), _(STRING_OPERATION_STARTED), type);
    }
    
    
    fflush(stdout);
    if (log)
        fflush(log);
    
    release_sem(status);
    /* end msg */
}

void print_pkt_arrived_msg(struct server_status *status, int user, int verbose, long long int seq) {
    /* msg: packet received */
    if (user == LS_CLIENT)
        get_mutex(&CLIENT_MUTEX);
    else
        get_sem(status);
    if (user == LS_SERVER) {
        if (verbose) {
            printf("%s %s (%4d): %s '%lld'\n", get_current_time(), _(STRING_CHILD),
                   getpid(), _(STRING_PKT_RECEIVED), seq);
            fflush(stdout);
        }
    }
    else
        if (verbose) {
            printf("%s %s '%lld'\n", get_current_time(),
                   _(STRING_PKT_RECEIVED), seq);
            fflush(stdout);
        }
    if (user == LS_CLIENT)
        release_mutex(&CLIENT_MUTEX);
    else
        release_sem(status);
    /* end msg */
}

void print_ack_arrived_msg(struct server_status *status, int user, int verbose, long long int seq) {
    /* msg: ack received */
    if (user == LS_CLIENT)
        get_mutex(&CLIENT_MUTEX);
    else
        get_sem(status);
    if (user == LS_SERVER) {
        if (verbose) {
            printf("%s %s (%4d): %s '%lld'\n", get_current_time(), _(STRING_CHILD),
                   getpid(), _(STRING_ACK_RECEIVED), seq);
            fflush(stdout);
        }
    }
    else
        if (verbose) {
            printf("%s %s '%lld'\n", get_current_time(),
                   _(STRING_ACK_RECEIVED), seq);
            fflush(stdout);
        }
    if (user == LS_CLIENT)
        release_mutex(&CLIENT_MUTEX);
    else
        release_sem(status);
    /* end msg */
}

void print_err_arrived_msg(FILE *log, struct server_status *status, int user, char *data) {
    /* msg: PKT_ERR received */
    if (user == LS_CLIENT)
        get_mutex(&CLIENT_MUTEX);
    else
        get_sem(status);
    if (user == LS_SERVER) {
        fprintf(stderr, "%s %s (%4d): %s <%s>\n", get_current_time(),
                _(STRING_CHILD), getpid(), _(STRING_ERR), data);
        if (log)
            fprintf(log, "%s %s (%4d): %s <%s>\n", get_current_time(),
                    _(STRING_CHILD), getpid(), _(STRING_ERR), data);
    }
    else {
        fprintf(stderr, "%s %s <%s>\n", get_current_time(), _(STRING_ERR),
                data);
        if (log)
            fprintf(log, "%s %s <%s>\n", get_current_time(), _(STRING_ERR),
                    data);
    }
    
    fflush(stdout);
    if (log)
        fflush(log);
    
    if (user == LS_CLIENT)
        release_mutex(&CLIENT_MUTEX);
    else
        release_sem(status);
    /* end msg */
}

void print_pkt_sent_msg(struct server_status *status, int user, int verbose, long long int seq) {
    if (user == LS_CLIENT)
        get_mutex(&CLIENT_MUTEX);
    else
        get_sem(status);
    if (user == LS_SERVER) {
        if (verbose) {
            printf("%s %s (%4d): %s '%lld'\n", get_current_time(), _(STRING_CHILD),
                   getpid(), _(STRING_PKT_SEND), seq);
            fflush(stdout);
        }
    }
    else
        if (verbose) {
            printf("%s %s '%lld'\n", get_current_time(),
                   _(STRING_PKT_SEND), seq);
            fflush(stdout);
        }
    if (user == LS_CLIENT)
        release_mutex(&CLIENT_MUTEX);
    else
        release_sem(status);
}