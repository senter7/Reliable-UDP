//
//  get.c
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

#include "get.h"

int USER = -1;
FILE *LOG = NULL;
struct server_status *STATUS = NULL;

/* Signal handler for SIGALARM signal */
void sig_handler(int sig) {
    //used to avoid compiler warning
    (void) sig;
    //Print user message
    print_interrupted_operation_msg(LOG, STATUS, USER);
    //Free port that was used before receiving signal
    if (USER == LS_SERVER)
        close_port1(STATUS, getpid());
    //Kill current process. If the user == LS_CLIENT, kill the entire program
    _exit(0);
}


/*  This function install a signal handler for SIGALARM signal */
void install_signal_handler(void) {
    
    struct sigaction sa;
    //Set the signal handler
    sa.sa_handler = sig_handler;
    //set the mask of blocked signals during signal handling
    if(sigemptyset(&sa.sa_mask) != 0) {
        perror("sigemptyset() in install_signal_handler()");
        exit(EXIT_FAILURE);
    }
    sa.sa_flags = 0;
    //Install new action for SIGALARM
    if(sigaction(SIGALRM, &sa, NULL) == -1)
    {
        perror("sigaction() in install_signal_handler()");
        exit(EXIT_FAILURE);
    }
}


/*  In the event that the sliding window is full, this function allows you to 
 *  request forcibly sending a package, and still receive packets until the 
 *  required package not reach its destination.
 *
 *  Parameters:
 *  - min:      The sequence number of the required packet
 *  - sockfd:   Descriptor for the socket
 *  - addr:     Address of the sending process
 *
 *  Return:     The required packet
 */
struct packet *recovery_mode(long long int min, int sockfd, struct sockaddr_in addr) {
    int flag = 0, retries = 0;
    socklen_t len;
    struct packet *pkt = NULL;
    struct packet *req = new_packet(PKT_REQ, min, NULL, 0);
    //Send a request for a specific packet
    send_pkt(sockfd, req, addr);
    
    while (flag == 0) {
        len = sizeof(addr);
        //Receive packet
        pkt = recv_pkt(sockfd, &addr, &len);
        //If the packet is what was requested, exit the loop and return.
        //Else, continue to receive packets.
        if (pkt->seq == min && is_accepted() == 1)
            flag = 1;
        else {
            free(pkt);
            retries++;
            //Every 10 retries, send a request pkt to sending process
            if (retries == 10) {
                retries = 0;
                send_pkt(sockfd, req, addr);
            }
        }
    }
    return pkt;
}

void receive_file(char *filename, int childPort, long int pkts_number, int user, char *ip, int old_sockfd, int verbose_mode, struct server_status *status, FILE *log) {
    USER = user;        //Set global variables
    LOG = log;          //
    STATUS = status;    //
    /*  fd:     if 'filename' == NULL, 'fd' represent the file descriptor for the
     *          list file. Infact, when a client requests the list of files to the server,
     *          the storage location of this text file is stored in LIST_FILE maco,
     *          so 'filename' must be NULL.
     *          See the case 'PKT_LS' in 'client.c' and 'server.c' to see how server
     *          and client work to get/send the file list
     *
     *  end:    this variable represents a marker to verify when the reception of a 
     *          file is finished. This variable is setted initially to 0. When
     *          a PKT_FIN si received, end is setted to 1 and the function exits 
     *          the loop of waiting for new packets
     */
    int new_sockfd = old_sockfd, fd, end = 0;
    /*  These variables are used to calculate and write the percentage of completion 
     *  of the transaction in progress.
     */
    int percentage = 0, last_percentage = 0;
    /*  min:        it represents the sequence number of the last packet written into the file.
     *              For more details, see 'write_contiguous()' in window_controller.h' and
     *              'window_controller.c'.
     *
     *  total:      it represents the total number of packets arrived, including those
     *              discarded to simulate packet loss on the network
     *
     *  discarded:  it represents the total number of discarded packets
     */
    long long int min = 1, total = 0, discarded = 0;
    unsigned long long int size = MAX_BLOCK_SIZE * pkts_number;
    struct sockaddr_in addr;
    socklen_t len;
    /*  Timer to calculate the completion time and the average time of receipt of
     *  each accepted package
     */
    struct timer *timer = new_timer();
    /*  This variable identifies the number of 'laps' that the timer has done. 
     *  This is used for the calculation of the average time of receipt of each 
     *  accepted packet. See 'timer.h' for details.
     */
    long long int laps = 0;
    /* This variable is used to calculate the average time */
    double average = 0.0;
    /* See comment about variable 'end' at the beginning of this function */
    if (filename != NULL) {
        //search if the file name already exists
        char *output = search_file(filename);
        fd = open_file(WRITE, output);
    }
    else {
        //create a file for the list file
        fd = open_file(WRITE, LIST_FILE);
    }
    
    struct packet *pkt;
    //ACK packet, prepared to be sent
    struct packet *ack = new_packet(PKT_ACK, 0, NULL, 0);
    struct window_controller *wc = NULL;
    
    /*  This array is used to idetify if a packet is already received or not.
     *  The number of array's slots are equal to the number of packets that
     *  it should receive.
     *  All slots in the array are initially set to 0. When a packet is
     *  received, the slot idetifies by sequence number of the pkt just received
     *  is set to 1.
     */
    int *v = malloc(sizeof(int) * pkts_number);
    if (v == NULL) {
        perror("malloc() in receive_file()");
        exit(EXIT_FAILURE);
    }
    int index;
    for (index = 0; index < pkts_number; ++index)
        v[index] = 0;

    //Create a new socket descriptor
    new_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(new_sockfd < 0) {
        perror("socket() in receive_file()");
        exit(EXIT_FAILURE);
    }
    //Initalize memory
    memset((void*)&addr, 0, sizeof(addr));
    //Fill the addr structure
    addr.sin_family = AF_INET;          //Address family
    addr.sin_port = htons(childPort);   //Port in network byte order
    //If ip == NULL, socket will be bound to all local interfaces
    if (ip == NULL)
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    else {
        //Convert string in a dot-decimal IP address
        if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
            perror("inet_pton() in receive_file()");
            exit(EXIT_FAILURE);
        }
    }
    //Binding
    if(bind(new_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind() in receive_file()");
        exit(EXIT_FAILURE);
    }
    /*  Initialize the 'window_controller' data structure. The second parameter
     *  is NULL, because to receive files doesn't need a 'time_controller'.
     *  See 'window_controller.h' for more details.
     */
    wc = new_window_controller(WINDOW_DIMENSION, NULL, new_sockfd, addr, fd);
    
    //Print messages
    if (filename != NULL)
        print_operation_started_msg(log, status, user, "GET");
    else
        print_operation_started_msg(log, status, user, "LIST");
    print_pkt_to_receive(log, status, user, pkts_number);
    
    /*  Install a new signal handler.
     *  See 'install_signal_handler()' in 'get.c' to read important details.
     */
    install_signal_handler();
    //Set the max inactivity timer
    alarm(MAX_INACTIVITY_TIME);
    //Start timer
    set_timer(timer, TIMER_START);
    //Run the cycle until receipt of PKT_FIN
    while (end == 0) {
        len = sizeof(addr);
        //Receive packet from network
        pkt = recv_pkt(new_sockfd, &addr, &len);
        total++;
        //Simulate loss probability
        //If pkt cannot be accepted, delete it
        if (is_accepted() == 0) {
            free(pkt);
            discarded++;
        }
        
        else {
            //Update the alarm
            alarm(MAX_INACTIVITY_TIME);
            //Get time between two receptions and update average time and laps
            set_timer(timer, TIMER_LAP);
            laps++;
            average += get_last_time_catched(timer);
            //Calculation of percentage of completition
            // percentage = (100 * total pkts received and accepted) / pkts number to receive
            percentage = 100 * (int)(total-discarded) / (int)pkts_number;
            if (percentage%10 == 0 && percentage != last_percentage) {
                //Print only the percentage in dozens
                print_completition_msg(log, status, user, percentage);
                last_percentage = percentage;
            }
            print_pkt_arrived_msg(status, user, verbose_mode, pkt->seq);
            //If received pkt is PKT_FIN, set 'end' = 1 to exit from the loop
            if (pkt->type == PKT_FIN) {
                end = 1;
                ack->type = PKT_FINACK;
                
                print_fin_arrived_msg(status, user, verbose_mode);
            }
            //If received pkt is PKT_ERR, print error message end prepare to exit from the loop
            else if( pkt->type == PKT_ERR) {
                print_err_arrived_msg(log, status, user, pkt->data);
                end = 1;
            }
            //If received pkt is PKT_DATA...
            else {
                //If a package has already been received, send only an ack...
                if (v[pkt->seq - 1] == 1) {
                    ack->type = PKT_ACK;
                }
                //...else, excecute all these operations:
                else {
                    //If sliding window is full, go to recovery mode
                    if (window_controller_is_full(wc) == 1) {
                        if (pkt->seq != min) {
                            free(pkt);
                            //See 'recovery_mode()' in 'get.c' for details
                            pkt = recovery_mode(min, new_sockfd, addr);
                        }
                        ssize_t m;
                        //Write pkt just received
                        m = write(fd, pkt->data, pkt->dimension);
                        if (m < 0 || (size_t) m != pkt->dimension) {
                            perror("write() in receive_file()");
                            exit(EXIT_FAILURE);
                        }
                        //Update the last seq. number of pkt just written
                        min++;
                    }
                    //If sliding window is not full:
                    else {
                        //Set the pkt as arrived
                        v[pkt->seq - 1] = 1;
                        //Add pkt into sliding window
                        window_controller_add_packet(wc, pkt);
                        //Write contiguous pkts and update 'min'
                        min = write_contiguous(wc, min);
                        ack->type = PKT_ACK;
                    }
                }
            }
            //Set sequence number for the ACK
            ack->seq = pkt->seq;
            //Send ACK
            send_pkt(new_sockfd, ack, addr);
        }
        
    }
    //Get the last lap time
    set_timer(timer, TIMER_LAP);
    
    /*  msg: final report
     *  This message shows the data relating to the operation just ended
     *  1) total time elapsed
     *  2) average time between receipit (average / laps)
     *  3) total pkts received
     *  4) total pkts discarded
     */
    get_sem(status);
    if (user == LS_SERVER) {
        printf("%s %s (%4d): %s\n\t   %s: %8f s.\n\t   %s: %8f s. per pkt\n\t   %s: %lld\n\t   %s: %lld\n\n",
                        get_current_time(), _(STRING_CHILD),
                        getpid(), _(STRING_OPERATION_COMPLETED),
                        _(STRING_TOTAL_TIME_ELAPSED), get_total_time_catched(timer),
                        _(STRING_AVERAGE_TIME),    average/laps,
                        _(STRING_TOTAL_PKTS_RECEIVED), total,
                        _(STRING_TOTAL_PKTS_DISCARDED), discarded);
        if (log)
            fprintf(log, "%s %s (%4d): %s\n\t   %s: %8f s.\n\t   %s: %8f s. per pkt\n\t   %s: %lld\n\t   %s: %lld\n\n",
                        get_current_time(), _(STRING_CHILD),
                        getpid(), _(STRING_OPERATION_COMPLETED),
                        _(STRING_TOTAL_TIME_ELAPSED), get_total_time_catched(timer),
                        _(STRING_AVERAGE_TIME),    average/laps,
                        _(STRING_TOTAL_PKTS_RECEIVED), total,
                        _(STRING_TOTAL_PKTS_DISCARDED), discarded);
    }
    else {
        printf("%s %s\n\t   %s: %8f s.\n\t   %s: %8f s. per pkt\n\t   %s: %lld\n\t   %s: %lld\n\t   %s: %8f MB/s\n\n",
                        get_current_time(), _(STRING_OPERATION_COMPLETED),
                        _(STRING_TOTAL_TIME_ELAPSED), get_total_time_catched(timer),
                        _(STRING_AVERAGE_TIME),    average/laps,
                        _(STRING_TOTAL_PKTS_RECEIVED), total,
                        _(STRING_TOTAL_PKTS_DISCARDED), discarded,
                        _(STRING_DOWNLOAD_SPEED), (double)size/1024/1024/get_total_time_catched(timer));
        if (log)
            fprintf(log, "%s %s\n\t   %s: %8f s.\n\t   %s: %8f s. per pkt\n\t   %s: %lld\n\t   %s: %lld\n\t   %s: %8f\n\n",
                        get_current_time(), _(STRING_OPERATION_COMPLETED),
                        _(STRING_TOTAL_TIME_ELAPSED), get_total_time_catched(timer),
                        _(STRING_AVERAGE_TIME),    average/laps,
                        _(STRING_TOTAL_PKTS_RECEIVED), total,
                        _(STRING_TOTAL_PKTS_DISCARDED), discarded,
                        _(STRING_DOWNLOAD_SPEED), (double)size/1024/1024/get_total_time_catched(timer));
    }
    release_sem(status);
    /* end msg */
    
    alarm(0);                       //deactivate the alarm
    free(timer);                    //free the timer
    free(v);                        //free the vector of received pkts
    free(ack);                      //free the ack packet
    fflush(stdout);                 //empty the buffer of standard output
    fflush(log);                    //empty the buffer of 'log' file
    close_file(fd);                 //close the file just written
    close(new_sockfd);              //close the created socket
    window_controller_dispose(wc);  //free sliding window
    USER = -1;                      //set global variables to default values
    STATUS = NULL;                  //
    if(log)
        fflush(log);
}
