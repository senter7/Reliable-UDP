//
//  utils.h
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
//  This header file contains a lot of functions and data structures that have
//  proven useful during the development of the project.
//  The development of this software has changed considerably over the time.
//  So, some of these features may have been used in previous releases since the
//  last. For completeness, the code of each developed function will be maintained,
//  even if not used in the latest release.


#ifndef __Reliable_UDP__utils__
#define __Reliable_UDP__utils__

#define WRITE 1
#define READ 0

#include "packet.h"
#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <semaphore.h>


enum user_type { LS_SERVER, LS_CLIENT };

/*  This data structure contains all the necessary parameters to be passed to the 
 *  thread that is responsible for receiving ACKs during PUT operation.
 *  This data structure is used in 'put.h'
 */
struct thread_data {
    int sockfd;                     //Socket file descriptor
    int output;                     //File descripto of the output file
    struct sockaddr_in addr;        //Addres
    struct window_controller *wc;   //window_controller created by main thread
    struct time_controller *tc;     //time controller created by main thread
    pthread_t thread;               //Where the thread ID is saved
    int port;                       //Communication port
    char *filename;                 //Name of the file that is trasmitted
    unsigned long long int number;  //Number of pkts to send/receive
    int *stop_err;                  //Pointer to a flag: 0 = continue, 1 = stop (error)
    struct server_status *status;   //Pointer to 'server_status' struct to use semaphores
    FILE *log;                      //File pointer to the log file (if exists)
    int verbose;                    //1 -> verbose mode on, 0 -> verbose off
    int user;                       //LS_CLIENT or LS_SERVER
};


/*  This function allows you to open a file in two different way:
 *  only read or only write,
 *
 *  Parameters:
 *  - ctrl:     WRITE to write the file, otherwise READ
 *  - path:     The path of the file
 *
 *  Return:     File descriptor (int) of the opened file
 */
int open_file(unsigned int ctrl, const char *path);


/*  This function clos an opened file knowing its file descriptor
 *
 *  Parameters:
 *  - fd:       File descriptor
 *
 *  Return:     Nothing
 */
void close_file(int fd);


/*  This function open a file in 'a' mode.
 *
 *  Parameters:
 *  - path:     The path of the file
 *
 *  Return:     File pointer (FILE *)
 */
FILE *fopen_file(const char *path);


/* Close a file previously opened with 'fopen_file()' */
void fclose_file(FILE *fp);


/*  This function allows to calculate the dimension in bytes
 *  of a file.
 *
 *  Parameters:
 *  - f:        The path of the file
 *
 *  Return:     The size of the file in bytes
 */
unsigned long long get_dimension(const char *f);


/*  This function allows to calculate the number of packets needed to
 *  send a file, knowing its dimension in bytes. This function used the macro
 *  MAX_BLOCK_SIZE in 'settings.h' to calculate the number of pkts.
 *
 *  Parameters:
 *  - size:     The size in byte of the file (use 'get_dimension()' to calculate it)
 *
 *  Return:     Number of packets needed to send the file
 */
unsigned long long int get_number(unsigned long long size);


/*  This function search if a file already exists, and if it exists return an
 *  alternative name for save new file.
 *
 *  Example:
 *  the file 'my_program.exe already exists.
 *
 *  char *new_name = search_file("my_program.exe");
 *  
 *  'new_name' will be "(1)my_program.exe"
 *
 *  Parameters:
 *  - filename.     Name of the file to seach
 *
 *  Return:         An alternative file name (char *)
 */
char *search_file(char *filename);


/* lock a mutex 'pthread_mutex_t' */
void get_mutex(pthread_mutex_t *MTX);


/* unlock a mutex 'pthread_mutex_t' */
void release_mutex(pthread_mutex_t *MTX);


/*  This function uses 'nanosleep()' to put to sleep a thread.
 *
 *  Parameters:
 *  - msec:         The stop time in msecs
 *
 *  Return:         Nothing
 */
void msleep(int msec);


/*  This function is used by client o interpret the user input.
 *
 *  Parameters:
 *  - line.         The line written by user and captured by client
 *
 *  Return:         PKT_PUT  -> if the user typed "PUT " as the first four letters
 *                  PKT_GET  -> if the user typed "GET " as the first four letters
 *                  PKT_LS   -> if the user typed "LIST" as the first four letters
 *                  PKT_HELP -> if the user typed "HELP" as the first four letters
 *                  PKT_LANG -> if the user typed "LANG" as the first four letters
 *                  PKT_ERR  -> for all other input
 */
int read_operation(char *line);


/*  This function sends a packet on the network through 'sendto()' function
 *
 *  Parameters:
 *  - sockfd:       Socket file descriptor
 *  - pkt:          Packet to be send
 *  - addr:         Addres for sending packet
 *
 *  Return.         Nothing
 */
void send_pkt(int sockfd, struct packet *pkt, struct sockaddr_in addr);


/*  This function allow to receive a packet from the network through 'recvfrom()'
 *  function.
 *
 *  Parameters:
 *  - sockfd:       Socket file descriptor
 +  - addr:         Addres for receiving packet
 *  - len:          SIZE OF ADDRESS
 *
 *  Return:         A 'packet' data structure filled with the data received
 *                  from the network
 */
struct packet *recv_pkt(int sockfd, struct sockaddr_in *addr, socklen_t *len);


/*  this function sorts all the elements in a 'window' data structure
 *
 *  Parameters:
 *  - w:            'window' data structure to be sorted
 *
 *  Return:         Nothing
 */
void sort_window(struct window *w);


/*  This function simulates packet loss. This function is related to
 *  LOSS_PROBABILITY in 'settings.h'
 *
 *  Parameters:     Nothing
 *
 *  Return:         1 if the pkt have to be accepted, 0 otherwise
 */
int is_accepted();

/* Convert an integer to a string */
char *convert_int(int port);

/* convert a long long integer to a string */
char *convert_llint(long long int num);

/* return current local time in format [hours:mins:secs]*/
char *get_current_time();

/* return current local data in format day.month.year*/
char *get_current_data();

#endif /* defined(__Reliable_UDP__utils__) */
