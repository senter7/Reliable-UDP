//
//  put.c
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



#include "put.h"

void *receiver_work(void *arg) {
    struct thread_data *data = (struct thread_data *) arg;
    struct packet *pkt;
    int end = 0;
    int percentage = 0, last_percentage = 0;
    long long int received = 0;
    
    //while (arrived < data->number && end == 0) {
    while (end == 0) {
        
        pkt = recv_pkt(data->sockfd, NULL, NULL);
        
        switch (pkt->type) {
            case PKT_ERR:
                print_err_arrived_msg(data->log, data->status, data->user, pkt->data);
                data->stop_err++;
                end = 1;
                break;
            case PKT_REQ:
                if (window_controller_resend_packet(data->wc, pkt->seq) == 1) {
                    fprintf(stderr, "%s\n", _(STRING_PKT_NOT_FOUND_IN_WINDOW));
                    exit(EXIT_FAILURE);
                }
                else
                    free(pkt);
                break;
            case PKT_FINACK:
                print_finack_arrived_msg(data->status, data->user, data->verbose);
                received++;
                if (window_controller_set_ack(data->wc, pkt->seq) == 0) {
                    fprintf(stderr, "Error: PKT_FIN not found in the window\n");
                    exit(EXIT_FAILURE);
                }
                else {
                    end = 1;
                    if (time_controller_delete_timer(data->tc, pkt->seq) == 0) {
                        //In this case, 'time_data' was not found in 'timer_wheel'
                    }
                    else {
                        //In this case, the 'time_data' was found and deleted
                        //pthread_cond_signal(&data->tc->full);
                    }
                    percentage = 100 * (int)received / (int)data->number;
                    if (percentage%10 == 0 && percentage != last_percentage) {
                        print_completition_msg(data->log, data->status, data->user, percentage);
                        last_percentage = percentage;
                    }
                }
                break;
            case PKT_ACK:
                received++;
                print_ack_arrived_msg(data->status, data->user, data->verbose, pkt->seq);
                if (window_controller_set_ack(data->wc, pkt->seq) == 0) {
                    //In this case, pkt was not 'acked'
                }
                else {
                    //In this case, pkt was acked
                    if (time_controller_delete_timer(data->tc, pkt->seq) == 0) {
                    //In this case, 'time_data' was not found
                    }
                    else {
                        //In this case, 'time_data' was found and deleted
                    }
                    //printf("Timer trovato!\n");
                }
                percentage = 100 * (int)received / (int)data->number;
                if (percentage%10 == 0 && percentage != last_percentage) {
                    print_completition_msg(data->log, data->status, data->user, percentage);
                    last_percentage = percentage;
                }
                break;
        }
    }
    
    time_controller_stop(data->tc);
    pthread_exit(NULL);
}


void send_file(int port, char *ip, int old_sockfd, int fd, char *filename, int user, struct server_status *status, FILE *log, int verbose) {
    struct sockaddr_in addr;
    struct time_controller *tc;
    struct window_controller *wc;
    struct thread_data data;
    /*  seq:    is the first sequence number of the pkt to send
     *  size:   dimension (in byte) of the file to send
     *  number: number of pkts needed to send entirely the file
     *  permanent_size: used to calculate upload speed
     */
    
    unsigned long long int seq = 1, size = get_dimension(filename);
    unsigned long long int number = get_number(size);
    unsigned long long int permanent_size = size;
    //Marker to exit in an error accurs
    int stop_err = 0;
    int new_sockfd = old_sockfd;
    //Number of pkts sent
    long long int sent = 0;
    /*  Timer to calculate the completion time and the average time of sending for
     *  each sent pkt
     */
    struct timer *timer = new_timer();
    /*  This variable identifies the number of 'laps' that the timer has done.
     *  This is used for the calculation of the average time of receipt of each
     *  accepted packet. See 'timer.h' for details.
     */
    long long int laps = 0;
    /* This variable is used to calculate the average time */
    double average = 0.0;
    //If user == LS_SERVER, create a new socket
    if (user == LS_SERVER) {
        new_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(new_sockfd < 0) {
            perror("socket() in send_file()");
            exit(EXIT_FAILURE);
        }
    }
    //Initialize 'addr' memory
    memset((void*)&addr, 0, sizeof(addr));
    //Fill the addr structure
    addr.sin_family = AF_INET;      //Address family
    addr.sin_port = htons(port);    //Port in network byte order
    //If ip == NULL, socket will be bound to all local interfaces
    if (ip == NULL)
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    else {
        //Convert string in a dot-decimal IP address
        if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
            perror("inet_pton() in send_file()");
            exit(EXIT_FAILURE);
        }
    }
    /*  Initialize 'time_controller' and 'window_controller' data structures.
     *  To send a file, both data structures are essential.
     *  See 'window_controller.h' and 'time_controller.h' for details.
     */
    tc = new_time_controller(TIME_CONTROLLER_GRANULARITY, WINDOW_DIMENSION, NULL, user, NULL);
    wc = new_window_controller(WINDOW_DIMENSION, tc, new_sockfd, addr, -1);
    //Start the controller thread for timeouts
    time_controller_start(tc, wc);
    
    //Fill 'thread_data' with all important value for the new receiver thread
    data.addr = addr;
    data.output = -1;
    data.sockfd = new_sockfd;
    data.tc = tc;
    data.wc = wc;
    data.number = number;
    data.stop_err = &stop_err;
    data.status = status;
    data.log = log;
    data.verbose = verbose;
    data.user = user;
    
    print_operation_started_msg(log, status, user, "PUT");
    
    /*  Creates a new thread and throw it on the function 'receiver_work()'.
     *  This new thread is responsible for receiving acks for packets sent
     */
    if(pthread_create(&data.thread, NULL, receiver_work, &data) != 0) {
        perror("pthread_create() in send_file()");
        exit(EXIT_FAILURE);
    }
    
    /*  The main thread is responsible to read progressively the file to send and
     *  send the packets over the network.
     */
    ssize_t m;
    //Start the timer
    set_timer(timer, TIMER_START);
    //The loop terminates when there are less than MAX_BLOCK_SIZE bytes to read in the file
    while (size >= MAX_BLOCK_SIZE && stop_err == 0) {
        //Create a new pkt with empy 'data' field
        struct packet *pkt = new_packet(PKT_DATA, seq, NULL, MAX_BLOCK_SIZE);
        //Fill 'data' field of the pkt with bytes read from file
        m = read(fd, pkt->data, MAX_BLOCK_SIZE);
        if (m < 0 || (size_t) m != MAX_BLOCK_SIZE) {
            perror("read() in send_file()");
            exit(EXIT_FAILURE);
        }
        //Add the packet into the sliding window
        window_controller_add_packet(wc, pkt);
        //Get a lap and update average time and laps
        set_timer(timer, TIMER_LAP);
        average += timer->last_time_catched;
        laps++;
        
        print_pkt_sent_msg(status, user, verbose, seq);
        
        seq++;
        sent++;
        size -= MAX_BLOCK_SIZE;
    }
    
    //Send the last PKT_DATA
    if (size != 0 && stop_err == 0) {
        struct packet *pkt = new_packet(PKT_DATA, seq, NULL, (size_t)size);
        m = read(fd, pkt->data, (size_t)size);
        if (m < 0 || (size_t) m != size) {
            perror("last read() in send_file()");
            exit(EXIT_FAILURE);
        }
        
        window_controller_add_packet(wc, pkt);
        set_timer(timer, TIMER_LAP);
        average += timer->last_time_catched;
        laps++;
        
        print_pkt_sent_msg(status, user, verbose, seq);
        
        seq++;
        sent++;
    }
    
    //wait the necessary condition to send last packet (PKT_FIN)
    while (window_controller_is_empty(wc) == 0 && stop_err == 0)
        //Wait until sliding window is not empty
        pthread_cond_wait(&wc->zero, &wc->m_zero);
    
    //Finally, send last packet (PKT_FIN)
    if (stop_err == 0) {
        struct packet *fin = new_packet(PKT_FIN, seq, NULL, 0);
        window_controller_add_packet(wc, fin);
    }
    
    //Wait until the receiver thread ends its work
    if (pthread_join(data.thread, NULL) != 0) {
        perror ("pthread_join() in send_file()");
        exit(EXIT_FAILURE);
    }
    
    //Free memory
    window_controller_dispose(wc);
    
    /*  msg: final report
     *  This message shows the data relating to the operation just ended
     *  1) total time elapsed
     *  2) average time between receipit (average / laps)
     *  3) total pkts received
     */
    get_sem(status);
    if (user == LS_SERVER) {
        printf("%s %s (%4d): %s\n\t   %s: %8f s.\n\t   %s: %8f s. per pkt\n\t   %s: %lld\n\n",
               get_current_time(), _(STRING_CHILD),
               getpid(), _(STRING_OPERATION_COMPLETED),
               _(STRING_TOTAL_TIME_ELAPSED), get_total_time_catched(timer),
               _(STRING_AVERAGE_TIME_TO_SEND),    average/laps,
               _(STRING_TOTAL_PKTS_SEND), sent);
        if (log)
            fprintf(log, "%s %s (%4d): %s\n\t   %s: %8f s.\n\t   %s: %8f s. per pkt\n\t   %s: %lld\n\n",
                    get_current_time(), _(STRING_CHILD),
                    getpid(), _(STRING_OPERATION_COMPLETED),
                    _(STRING_TOTAL_TIME_ELAPSED), get_total_time_catched(timer),
                    _(STRING_AVERAGE_TIME_TO_SEND),    average/laps,
                    _(STRING_TOTAL_PKTS_SEND), sent);
    }
    else {
        printf("%s %s\n\t   %s: %8f s.\n\t   %s: %8f s. per pkt\n\t   %s: %lld\n\t   %s: %8f MB/s\n\n",
               get_current_time(), _(STRING_OPERATION_COMPLETED),
               _(STRING_TOTAL_TIME_ELAPSED), get_total_time_catched(timer),
               _(STRING_AVERAGE_TIME_TO_SEND),    average/laps,
               _(STRING_TOTAL_PKTS_SEND), sent,
               _(STRING_UPLOAD_SPEED), (double)permanent_size/1024/1024/get_total_time_catched(timer));
        if (log)
            fprintf(log, "%s %s\n\t%s: %8f s.\n\t%s: %8f s. per pkt\n\t%s: %lld\n\t   %s: %8f MB/s\n\n",
                    get_current_time(), _(STRING_OPERATION_COMPLETED),
                    _(STRING_TOTAL_TIME_ELAPSED), get_total_time_catched(timer),
                    _(STRING_AVERAGE_TIME_TO_SEND),    average/laps,
                    _(STRING_TOTAL_PKTS_SEND), sent,
                    _(STRING_UPLOAD_SPEED), (double)permanent_size/1024/1024/get_total_time_catched(timer));
    }
    release_sem(status);

    /* end msg */
    fflush(stdout);
    if (log)
        fflush(log);
}