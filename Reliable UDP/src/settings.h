//
//  settings.h
//  Reliable UDP
//
//  Created by Simone Minasola on 01/10/15.
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
//  This header file contains macros editable by user. Each macro is setted
//  to default value. If you want to modify any value, you have to read
//  description in order to avoid problems during execution. If you have changed
//  a value but can not remember the default value, you can read this below.
//
//
//  DEFAULT VALUES
//
//  MAX_BLOCK_SIZE                  1024
//  DEFAULT_TIMEOUT_USEC            3000
//  DEFAULT_TIMEOUT_SEC             0
//  MAX_RETRIES_SENDING_PKT         15
//  WINDOW_DIMENSION                31
//  TIME_CONTROLLER_GRANULARITY     500
//  SERV_PORT                       5593
//  MAX_OP_STRING_SIZE              256
//  LOSS_PROBABILITY                0
//  MAX_PROCESSES_NUMBER            10
//  DEFAULT_MSG_DETAIL              DETAIL_MEDIUM
//  SERVER_LOG_FILE_PATH            "LOG.txt"
//  DATA_DIR                        "data"
//  LIST_FILE                       "server_list.txt"
//  FIRST_AVAILABLE_PORT            5594
//  MAX_INACTIVITY_TIME             60




#ifndef Reliable_UDP_settings_h
#define Reliable_UDP_settings_h

/*  MAX_BLOCK_SIZE define the max number of bytes that sending process
 *  can send over the network into a packet. Sending process will only read
 *  MAX_BLOCK_SIZE bytes at a time from file.
 *
 *  WARNING:
 *  If you want to change this macro, consider that a too small value could 
 *  generate a high number of packets sent for a file, and if the loss probability
 *  is very high, sending the file could take a long time.
 */
#define MAX_BLOCK_SIZE                  1024

/*  DEFAULT_TIMEOUT_USEC and DEFAULT_TIMEOUT_SEC are used to set the timeout for 
 *  the packets when still did not receive any ACK to calculate the timeout dynamically.
 *  The first one is used to sed secs, the second one to set the usecs
 *
 *  WARNING:
 *  A high value of this setting can generate a high delay re-forwarding for the
 *  packets that were shipped first
 */
#define DEFAULT_TIMEOUT_USEC            3000
#define DEFAULT_TIMEOUT_SEC             0

/*  MAX_RETRIES_SENDING_PKT identifies the maximum number of attempts to send a 
 *  single packet. When attempts reaches this value, the connection between client 
 *  and server is interrupted because it is believed that the line may be too busy 
 *  or loss probability may be too high.
 *
 *  WARNING:
 *  A too low value may lead to loss of the connection early, as well as too high 
 *  would lead to timeouts too high: for each retransmission, the timeout of a package 
 *  is doubled
 */
#define MAX_RETRIES_SENDING_PKT         15

/*  WINDOW_DIMENSION identifies the size of the window containing the 'on-the-fly'
 *  packets. 'window' is a structure based on circular array, then the value in 
 *  this definition is calculated in less than 1.
 *  In example, to set the window dimension at 7, you have to set the macro
 *  WINDOW_DIMENSION at 7+1=8.
 *
 *  WARNING:
 *  If the value of this macro is set to 2, you will have a stop-and-wait protocol.
 *  A too high value can lead to an incorrect use of the bandwidth, while a value 
 *  too small can slow down the operations of sending and receiving a file.
 *  Do not set the value to below 1.
 */
#define WINDOW_DIMENSION                31

/*  TIME_CONTROLLER_GRANULARITY defines the time interval (in msecs) in which the 
 *  thread sleeps before checking all timers in the 'timer_wheel'.
 *
 *  WARNING:
 *  A smaller value will lead to increased use of the CPU , but also at a more 
 *  accurate control of timeouts. A larger value will decrease the use of the CPU, 
 *  but the control of timeouts will be less accurate
 */
#define TIME_CONTROLLER_GRANULARITY     500

/*  MAX_PROCESSES_NUMBER identifies the max number of connection that the server
 *  can manage.
 */
#define MAX_PROCESSES_NUMBER            10

/*  SERV_PORT identifies the server communication port.
 *
 *  WARNING:
 *  Do not assign a port number already used by operating system or by applications
 */
#define SERV_PORT                       5593

/*  FIRST_AVAILABLE_PORT is used to create new communication ports for each client 
 *  that connects to the server.
 *
 *  WARNING:
 *  The server will create MAX_PROCESSES_NUMBER communication ports. So, you have
 *  to set this macro at least MAX_PROCESSES_NUMBER before SERV_PORT, otherwise
 *  after SERV_PORT
 */
#define FIRST_AVAILABLE_PORT            5594

/*  MAX_OP_STRING_SIZE defines the maximum number of characters that the client 
 *  can take in input during the selection of an operation,  including the name 
 *  of the file to send or receive.
 *
 *  WARNING:
 *  A too small value may not be able to contain the names of files that are too large
 */
#define MAX_OP_STRING_SIZE              256

/*  LOSS_PROBABILITY defines the percentage chance of not accepting a packet.
 *
 *  WARNING:
 *  A too high value can simulate a line too busy, and retransmissions of 
 *  packets may increase.
 */
#define LOSS_PROBABILITY                0 //in percentage

/*  DEFAULT_MSG_DEATIL defines the level of detail wich messages are written.
 *  See 'log_msg.h' for details.
 *
 *  WARNING:
 *  In the last release, this macro is UNUSED, because log_msg, log_service and
 *  verbose_service data structures are been removed from the project.
 *  See the report to find out the reason for removing.
 */
//#define DEFAULT_MSG_DETAIL              DETAIL_MEDIUM

/*  SERVER_LOG_FILE_PATH identifies the name of the server LOG file. */
#define SERVER_LOG_FILE_PATH            "log/LOG.txt"

/*  DATA_DIR identifies the name of the directory where the data is saved. */
#define DATA_DIR                        "data"

/*  LIST_FILE identifies the name of the file wich contains the list of files
 *  avaible in the server
 */
#define LIST_FILE                       "data/server_list.txt"

/*  MAX_INACTIVITY_TIME defines the maximum time that the receiving process can
 *  wait before close connection if it doesn't not receive any packets anymore
 */
#define MAX_INACTIVITY_TIME             60 //in secs

#endif
