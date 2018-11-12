//
//  Server.c
//  Reliable UDP
//
//  Created by Simone Minasola on 24/08/15.
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
//  This is the main program for the server. This file includes the starting
//  point function (main()).
//  The server behavior is as follows:
//  1)  The server is waiting for a new request
//  2)  A new request is received
//  3)  The server processes the request and generates a free port number to be
//      sent to the client. Then, a new child process is created. This process
//      is responsible to execute the request.
//  4)  The father process return to listening for a new request
//  5)  When the child process terminates the work, it is killed


#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "window_controller.h"
#include "time_controller.h"
#include "utils.h"
#include "settings.h"
#include "put.h"
#include "get.h"
#include "list.h"
#include "strings.h"
#include "server_status.h"
#include <locale.h>

/*  Global variables:
 *
 *  verbose_mode:   if 1, the verbose mode will be activated
 *  log_file:       if 1, the log file will be written
 *
 *  These variables are configured via input
 */
int verbose_mode = 0;
int log_file = 0;


int main(int argc, char *argv[]) {
    //Set the language
    select_language(LANG_EN);
    
    //Input control begin
    if (argc >= 2) {
        if (strcmp(argv[1], "-l") == 0)     //Log service on
            log_file = 1;
        else if (strcmp(argv[1], "-v") == 0)//Verbose mode on
            verbose_mode = 1;
        else {
            fprintf(stderr, "%s: <%s>\n", _(STRING_COMMAND_NOT_FOUND), argv[1]);
            exit(EXIT_FAILURE);
        }
    }
    if (argc == 3) {
        if (strcmp(argv[2], "-l") == 0)     //Log service on
            log_file = 1;
        else if (strcmp(argv[2], "-v") == 0)//Verbose service on
            verbose_mode = 1;
        else {
            fprintf(stderr, "%s: <%s>\n", _(STRING_COMMAND_NOT_FOUND), argv[2]);
            exit(EXIT_FAILURE);
        }
    }
    if (argc > 3 || argc < 1) {
        fprintf(stderr, "%s\n", _(STRING_INSTRUCTION_SERVER));
        exit(EXIT_FAILURE);
    }
    //Input control end
    
    //Initialize 'server_status' struct
    struct server_status *status = create_shared_memory();
    //If status == NULL, there is a problem in FIRST_AVAILABLE_PORT in 'settings.h'
    if (status == NULL) {
        printf("%s\n", _(STRING_PROBLEM_FIRST_AV_PORT));
        exit(EXIT_FAILURE);
    }

    int sockfd;             //Socket descriptor
    struct sockaddr_in addr;//Address
    socklen_t len;          //Size of address
    //Empty pkt where requests are stored
    struct packet *pkt;// = malloc(sizeof(struct packet));

    //Empty pkt where response is stored for each request
    struct packet *response = NULL;
    //Struct where free ports are stored for each accepted request
    struct free_p *port;
    /*  For the server, exists only one log file named LOG.txt. In this file
     *  are stored all the operations performed during the sessions with the
     *  service log on. The file is saved in 'log/' directory.
     */
    FILE *log = NULL;
    if (log_file == 1)
        log = fopen_file(SERVER_LOG_FILE_PATH);
    
    print_server_welcome_msg(log, verbose_mode);
    
    //Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        perror("socket() in main()");
        exit(EXIT_FAILURE);
    }
    //Initialize memory
    memset((void*)&addr, 0, sizeof(addr));
    //Fill address
    addr.sin_family = AF_INET;       //Address family
    //INADDR_ANY allows the server to receive packets destined to any of the interfaces
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERV_PORT);//Port in network byte order
    //Binding
    if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind() in main()");
        exit(EXIT_FAILURE);
    }
    
    int fd;
    //Infinite loop
    while (1) {
        len = sizeof(addr);
        
        print_waiting_msg(log, status);
        //Receive pkt
        pkt = recv_pkt(sockfd, &addr, &len);

        switch (pkt->type) {
            //PUT REQUEST RECEIVED
            case PKT_PUT:
                //If MAX_PROCESSES_NUMBER is reached, then refuse connection
                if (get_processes(status) == MAX_PROCESSES_NUMBER) {
                    print_max_processes_msg(log, status);
                    response = new_packet(PKT_ERR, 0, _(STRING_SERVER_BUSY_ERR), 0);
                }
                //If MAX_PROCESS_NUMBER is not reached, accept connection
                else {
                    print_request_accepted(log, status);
                    //Select first free port available
                    port = first_available_port(status);
                    
                    print_port_msg(log, status, verbose_mode, port->port);
                    //Create new chil process
                    pid_t pid = fork();
                    if (pid < 0) {
                        perror("fork() in main() [case PKT_PUT]");
                        exit(EXIT_FAILURE);
                    }
                    //CHILD PROCESS WORK
                    else if (pid == 0) {
                        get_sem(status);
                        //Set itself as unique user of this port
                        port->user = getpid();
                        int my_port = port->port;
                        release_sem(status);
                        //Prepare to receive the file
                        receive_file(pkt->data, my_port, (long int) pkt->dimension,
                                     LS_SERVER, NULL, 0, verbose_mode, status, log);
                        //At the end, decrease number of active processes...
                        decrease_processes(status);
                        //...and free the port
                        close_port(status, my_port);
                        //exit (kill)
                        _exit(0);
                    }
                    //FATHER PROCESS WORK
                    else {
                        //Increase the number of active process
                        increase_processes(status);
                        //Set response packet to begin operation
                        response = new_packet(PKT_ACK, 0, NULL, (size_t)port->port);
                    }
                }
                //Send response to the client
                send_pkt(sockfd, response, addr);
                free(response);
                free(pkt);
                break;
            //GET REQUEST RECEIVED
            case PKT_GET:
                //If MAX_PROCESSES_NUMBER is reached, then refuse connection
                if (get_processes(status) == MAX_PROCESSES_NUMBER) {
                    print_max_processes_msg(log, status);
                    response = new_packet(PKT_ERR, 0, _(STRING_SERVER_BUSY_ERR), 0);
                }
                //If MAX_PROCESS_NUMBER is not reached, accept connection
                else {
                    //Modify the filename to search inside the right directory
                    char *filename = malloc((strlen(pkt->data) + strlen(DATA_DIR) + 2) * sizeof(char));
                    if (snprintf(filename, (strlen(pkt->data) + strlen(DATA_DIR) + 2) * sizeof(char), "%s/%s", DATA_DIR, pkt->data) < 0) {
                        perror("snprintf() in main() [case PKT_GET]");
                        exit(EXIT_FAILURE);
                    }
                    //Try to open the file
                    fd = open(filename, O_RDONLY);
                    //If file doesn't exists, send an error packet
                    if (fd == -1) {
                        print_file_not_found_msg(log, status, pkt->data);
                        response = new_packet(PKT_ERR, 0, _(STRING_FILE_NOT_FOUND), 0);
                    }
                    //If file exists, prepare to send it
                    else {
                        print_request_accepted(log, status);
                        //Retrieve the number of pkts needed to send entirely the file
                        long long int pkts = get_number(get_dimension(filename));
                        //Select first free port available
                        port = first_available_port(status);
                        
                        print_port_msg(log, status, verbose_mode, port->port);
                        //Create chil process
                        pid_t pid = fork();
                        if (pid < 0) {
                            perror("fork() in main() [case PKT_GET]");
                            exit(EXIT_FAILURE);
                        }
                        //CHILD WORK
                        else if (pid == 0) {
                            get_sem(status);
                            //Set itself as unique user of this port
                            port->user = getpid();
                            //Save the port in a local variable
                            int my_port = port->port;
                            release_sem(status);
                            //Prepare to send the file
                            send_file(my_port, NULL, 0, fd, filename, LS_SERVER, status, log, verbose_mode);
                            //At the end, decrease the number of active processes
                            decrease_processes(status);
                            //Free used port
                            close_port(status, my_port);
                            //Exit (kill)
                            _exit(0);
                        }
                        //FATHER WORK
                        else {
                            //Increase the number of active process
                            increase_processes(status);
                            //Set response pkt to send to begin operation
                            response = new_packet(PKT_ACK, 0, convert_llint(pkts), (size_t)port->port);
                        }
                    }
                }
                //Send response to the client
                send_pkt(sockfd, response, addr);
                free(response);
                free(pkt);
                break;
            //LIST REQUEST RECEIVED
            case PKT_LS:
                //If MAX_PROCESSES_NUMBER is reached, then refuse connection
                if (get_processes(status) == MAX_PROCESSES_NUMBER) {
                    print_max_processes_msg(log, status);
                    response = new_packet(PKT_ERR, 0, _(STRING_SERVER_BUSY_ERR), 0);
                }
                //If MAX_PROCESS_NUMBER is not reached, accept connection
                else {
                    print_request_accepted(log, status);
                    //Select first available port
                    port = first_available_port(status);
                    print_port_msg(log, status, verbose_mode, port->port);
                    //Create the file wich contains the list of file
                    char *list_file = create_list();
                    //Retrieve the number of pkts needed to send entirely the file
                    long long int pkts = get_number(get_dimension(list_file));
                    //Create child process
                    pid_t pid = fork();
                    if (pid < 0) {
                        perror("fork() in main() [case PKT_LS]");
                        exit(EXIT_FAILURE);
                    }
                    //CHILD WORK
                    else if (pid == 0) {
                        get_sem(status);
                        //Set itself as unique user of this port
                        port->user = getpid();
                        //Save the port in a local variable
                        int my_port = port->port;
                        release_sem(status);
                        //Open the list file to send it
                        int fd = open_file(READ, list_file);
                        //Prepare to send the file
                        send_file(my_port, NULL, 0, fd, list_file, LS_SERVER, status, log, verbose_mode);
                        //At the end, remove the file from 'temp/' directory
                        remove_list(list_file);
                        //Decrease number of active process befor exit
                        decrease_processes(status);
                        //Close used port
                        close_port(status, my_port);
                        //Exit (kill)
                        _exit(0);
                    }
                    //FATHER WORK
                    else {
                        //Increase the number of active process
                        increase_processes(status);
                        //Set the response packet
                        response = new_packet(PKT_ACK, 0, convert_llint(pkts), (size_t)port->port);
                    }
                }
                //Send response to the client
                send_pkt(sockfd, response, addr);
                free(response);
                free(pkt);
                break;
            }
    }
    
    //Never reached...i hope :)
    return 0;
}