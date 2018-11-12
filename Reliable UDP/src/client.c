//
//  Client.c
//  Reliable UDP
//
//  Created by Simone Minasola on 22/08/15.
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
//  This is the main program for the client. This file includes the starting
//  point function (main()). In addition, this file contains the function
//  'sendCMD()', that allows to establish a connection to the server, and prepare
//  both to perform a task.
//  The client behavior is as follows:
//  1)  The user types in the operation that he wants to make
//  2)  The client connects to the server welcome port and requires the execution
//      of operation
//  3)  The client receives the communication port number from the server
//  4)  The client begins the operation
//  5)  At the end, the client return to listening a for new operation


#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "window_controller.h"
#include "time_controller.h"
#include "utils.h"
#include "settings.h"
#include "put.h"
#include "get.h"
#include "list.h"
#include "strings.h"


/*  Global variables:
 *
 *  verbose_mode:   if 1, the verbose mode will be activated
 *  log_file:       if 1, the log file will be written
 *
 *  These variables are configured via input
 */
int verbose_mode = 0;
int log_file = 0;


/*  This function is used to read the help file when user type HELP on the
 *  client program. Helps file are txt files in 'docs/" directory.
 *  This function read the read file in the correct language.
 *
 *  Parameters:     Nothing
 *
 *  Return:         Nothing
 *
 *  Effects:
 *  This function prints on the screen the contents of the help file
 */
void read_help_file() {
    FILE *fp = NULL;
    if (get_lang_type() == LANG_IT)
        fp = fopen("docs/help_ita.txt", "r");
    else if (get_lang_type() == LANG_EN)
        fp = fopen("docs/help_eng.txt", "r");
    else {
        printf("%s\n", _(STRING_NO_HELP_FILE_FOUND));
        fp = fopen("docs/help_eng.txt", "r");
    }
    
    if (fp == NULL) {
        printf("%s\n", _(STRING_CANNOT_OPEN_HELP_FILE));
        return;
    }
    char line[1024];
    while (fgets(line, 1024, fp) != NULL)
        printf("%s", line);
}


/*  This function creates the connection between server and client.
 *  Each command (GET, PUT and LIST) is sent via this function to
 *  the 'welcome port' of the server. When the server accepts the connection 
 *  request, it selects a communication port for data transmission.
 *  All data received from the server at this stage, is saved in a 'long int'
 *  array (named 'data'), that will contains the communication port selected
 *  by server (data[0]) and, depending on the case, the number of packets
 *  to receive (data[1]).
 *
 *  Parameters:
 *  - type:     Type of pkt to send (PKT_PUT, PKT_GET, PKT_LS)
 *  - sockfd:   Socket file descriptor
 *  - addr:     Address to send the pkt for connection request
 *  - filename: The name of file to receive or to send
 *
 *  Return:     A two element array that contains the communication port selected
 *              by server (data[0]) and, depending on the case, the number of
 *              packets to receive (data[1])
 *
 *
 *
 *  These are the possible interactions between server and client according 
 *  to the requests sent
 *
 *  1) PUT:     The client calculates the number of packets required to send the 
 *              selected file to the server. A new  PKT_PUT packet is created,
 *              and the fields are filled as follows:
 *              - type:         PKT_PUT
 *              - seq:          0, because it is the first pkt
 *              - data:         the name of file to send
 *              - dimension:    number of pkts to send
 *              The server responds by sending an ACK packet on successful, 
 *              otherwise a PKT_ERR packet. On successful, the received packet
 *              contains in the field 'dimension' the valid port number for the 
 *              data transmission, on error, contains in the field 'data' an
 *              error message to print.
 *
 *  2) GET:     The client sends a PKT_GET packet with fields filled as
 *              follow:
 *              - type:         PKT_GET
 *              - seq:          0, because it is the first pkt
 *              - data:         the name of the file you want to receive
 *              - dimension:    0
 *              The server looks for the requested file. If the file is not found, 
 *              it sends a ERR_PKT. Otherwise, the server calculates the number of 
 *              packets required to send the file, and responds with a PKT_ACK 
 *              packet. This packet contains in the field 'dimension' the selected 
 *              port from the server for data transmission, and in the field 'date' 
 *              the number of packets that must be sent.
 *
 *  3) LIST:    The client sends a PKT_LS packet to the server with fields filled as
 *              as follow:
 *              - type:         PKT_LS
 *              - seq:          0, because it is the first pkt
 *              - data:         the name of the file you want to receive
 *              - dimension:    0
 *              The server creates the list into a txt file. If an error accurs,
 *              a PKT_ERR is sent. Otherwise, the server calculates the number of
 *              packets required to send the list, and responds with a PKT_ACK
 *              packet. This packet contains in the field 'dimension' the selected
 *              port for data transmission, and in the field 'date' the number 
 *              of packets that must be sent.
 *
 */
long int *sendCMD(int type, int sockfd, struct sockaddr_in addr, char *old_filename) {
    unsigned long long int size = 0;
    unsigned long long int number = 0;
    char *filename = NULL;
    //Allocate memory for the array
    long int *data = malloc(sizeof(long int) * 2);
    struct packet *pkt = NULL;
    if (data == NULL) {
        perror("malloc()\n");
        exit(EXIT_FAILURE);
    }
    //Insert the correct path in the filename
    if (type == PKT_PUT) {
        filename = malloc((strlen(old_filename) + strlen(DATA_DIR) + 2) * sizeof(char));
        if (snprintf(filename, (strlen(old_filename) + strlen(DATA_DIR) + 2) * sizeof(char), "%s/%s", DATA_DIR, old_filename) < 0) {
            perror("snprintf() in main()");
            exit(EXIT_FAILURE);
        }
    }
    else
        filename = old_filename;
    
    //Initialize elements
    data[0] = data[1] = (long int) 0;
    //If it is a PUT operation, calculate number of pkts to send
    if (type == PKT_PUT) {
            size = get_dimension(filename);
            number = get_number(size);
    }
    //Create the pkt
    pkt = new_packet(type, 0, old_filename, number);
    //Send the pkt
    send_pkt(sockfd, pkt, addr);
    //Receive the response pkt
    pkt = recv_pkt(sockfd, NULL, NULL);
    //If the response is a PKT_ERR, print on screen the error and exit
    if (pkt->type == PKT_ERR) {
        fprintf(stderr, "%s\n", pkt->data);
        return data;
    }
    //If the response is a PKT_ACK...
    else {
        //...save the port number...
        data[0] = (long int) pkt->dimension;
        //...if it is a GET or LIST operation, save the number of pkts to receive too
        if (type == PKT_GET || type == PKT_LS)
            /*  I can use atol() because i am sure that the server sends a
             *  'long int' type. If it was not sure, i should have used the
             *  'strtol()' function, wich is more secure.
             */
            data[1] = atol(pkt->data);  //Convert the string in a 'long int'
    }
    return data;
}


//STARTING POINT
int main(int argc, char *argv[]) {
    //Set the language
    select_language(LANG_EN);
    
    //Input control begin
    if (argc > 4 || argc < 2) {
        fprintf(stderr, "%s\n", _(STRING_CLIENT_INSTRUCTION));
        exit(EXIT_FAILURE);
    }
    if (argc >= 3) {
        if (strcmp(argv[2], "-l") == 0)     //log service activated
            log_file = 1;
        else if (strcmp(argv[2], "-v") == 0)//verbose mode activated
            verbose_mode = 1;
        else {
            fprintf(stderr, "%s: <%s>\n", _(STRING_COMMAND_NOT_FOUND), argv[1]);
            exit(EXIT_FAILURE);
        }
    }
    if (argc == 4) {
        if (strcmp(argv[3], "-l") == 0)     //log service activated
            log_file = 1;
        else if (strcmp(argv[3], "-v") == 0)//verbose service activated
            verbose_mode = 1;
        else {
            fprintf(stderr, "%s: <%s>\n", _(STRING_COMMAND_NOT_FOUND), argv[3]);
            exit(EXIT_FAILURE);
        }
    }
    //Input control end
    
    int sockfd;             //Socket descriptor
    struct sockaddr_in addr;//Network address
    long int *port = NULL;  //To store the return of 'sendCMD()' function
    FILE *log = NULL;       //Log file pointer
    
    //Create the socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
    //Initialize memory
    memset((void*)&addr, 0, sizeof(addr));
    //Fill address
    addr.sin_family = AF_INET;          //Address family
    addr.sin_port = htons(SERV_PORT);   //Port in network byte order
    //Convert string in a dot-decimal IP address
    if (inet_pton(AF_INET, argv[1], &addr.sin_addr) <= 0) {
        perror(_(STRING_IP_NOT_VALID));
        exit(EXIT_FAILURE);
    }
    /*  If log service is active, create or open a new log file.
     *  For the client, it creates a different log file every day.
     *  The files are saved in 'log/' directory.
     */
    if (log_file == 1) {
        char filename[64];
        snprintf(filename, 64, "log/LOG %s.txt", get_current_data());
        log = fopen_file(filename);
    }
    //Welcome message
    print_client_welcome_msg(log, verbose_mode);
    
    char operation[MAX_OP_STRING_SIZE];
    char *name;
    
    while (feof(stdin) == 0) {
        
        printf("\n%s\n\n", _(STRING_CLIENT_OP));
        //Get user input
        if(fgets(operation, MAX_OP_STRING_SIZE, stdin) == NULL) {
//            perror("fgets() in main()");
//            exit(EXIT_FAILURE);
            break;
        }
        //Remove the newline
        if(operation[strlen(operation)-1] == '\n')
            operation[strlen(operation)-1] = '\0';
        
        int fd;
        //Read user input. See 'read_ooperation()' in 'utils.h' for details
        switch (read_operation(operation)) {
            //PUT OPERATION
            case PKT_PUT:
                //Insert the correct path in the filename
                name = malloc((strlen(operation+4) + strlen(DATA_DIR) + 2) * sizeof(char));
                if (snprintf(name, (strlen(operation+4) + strlen(DATA_DIR) + 2) * sizeof(char), "%s/%s", DATA_DIR, operation+4) < 0) {
                    perror("snprintf() in main()");
                    exit(EXIT_FAILURE);
                }
                //Try open file
                fd = open(name, O_RDONLY);
                //If file doen't exists, print an error message
                if (fd == -1)
                    printf ("'%s' %s\n\n", operation+4, _(STRING_FILE_NOT_FOUND));
                //If file exists, begin to establish the connection with the server
                else {
                    //Establish connection
                    char *my_name = name;
                    port = sendCMD(PKT_PUT, sockfd, addr, operation+4);
                    if (port[0] == 0) {
                        break;
                    }
                    //Prepare and send the file
                    send_file((int)port[0], argv[1], sockfd, fd, my_name, LS_CLIENT, NULL, log, verbose_mode);
                    //Close the file
                    close_file(fd);
                }
                free(name);
                break;
            //GET OPERATION
            case PKT_GET:
                //Establish connection with the server
                port = sendCMD(PKT_GET, sockfd, addr, operation+4);
                if (port[0] == 0) {
                    break;
                }
                //Prepare to receive the file
                receive_file(operation+4, (int)port[0], port[1], LS_CLIENT, argv[1], sockfd, verbose_mode, NULL, log);
                break;
            //LIST OPERATION
            case PKT_LS:
                //Establish connection
                port = sendCMD(PKT_LS, sockfd, addr, NULL);
                if (port[0] == 0) {
                    break;
                }
                //Remove previous file list if exists
                remove_list(LIST_FILE);
                //Prepare to receive the new file list
                receive_file(NULL, (int)port[0], port[1], LS_CLIENT, argv[1], sockfd, verbose_mode, NULL, log);
                //Print file list on the screen
                print_list();
                break;
            //HELP OPERATION
            case PKT_HELP:
                //Read the help file
                read_help_file();
                break;
            //LANG OPERATION
            case PKT_LANG:
                //Interpret user input...
                if (strcmp(operation+5, "ITA") == 0 || strcmp(operation+5, "ita") == 0) {
                    select_language(LANG_IT);
                    printf("|=================================|\n");
                    printf("|%29s    |\n", _(STRING_LANG_SELECTED));
                    printf("|=================================|\n");
                    if (log)
                        fprintf(log, "%s %s\n", get_current_time(), _(STRING_LANG_SELECTED));
                }
                else if (strcmp(operation+5, "ENG") == 0 || strcmp(operation+5, "eng") == 0) {
                    select_language(LANG_EN);
                    printf("|=================================|\n");
                    printf("|%29s    |\n", _(STRING_LANG_SELECTED));
                    printf("|=================================|\n");
                    if (log)
                        fprintf(log, "%s %s\n", get_current_time(), _(STRING_LANG_SELECTED));
                }
                else printf("%s\n", _(STRING_NO_LANG_RECOGNIZED));
                break;
            //NOT VALID OPERATION
            case PKT_ERR:
                //Print error message
                printf("%s\n", _(STRING_COMMAND_NOT_FOUND));
                break;
        }
    }
    if (log) {
        fprintf(log, "%s %s\n", get_current_time(), _(STRING_SHUTDOWN));
        fflush(log);
        fclose_file(log);
    }
    printf("%s %s\n", get_current_time(), _(STRING_SHUTDOWN));
    fflush(stdout);
    return EXIT_SUCCESS;
}
