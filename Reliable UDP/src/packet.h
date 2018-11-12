//
//  packet.h
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
//  This header file contains the primary data structure of the entire project,
//  that represents the packet of data sent over the network. The data structure
//  'packet' is located on the first level of the N-layer architecture in the project,
//  as well as 'time_data'. Each packet sent or received is composed of a lot of
//  information (such as the type and sequence number), but only the process that
//  sends data (through the PUT operation) connects each packets to a structure
//  'time_data', that contains information to the timing of sending, receiving
//  and timeout. In fact, when a data packet is sent via 'sendto()', the parameter
//  'time_data *td' is not unreferenced. In this way, to the receiving process,
//  it is sent only a pointer to a memory area that is not in any way connected
//  to the sending process. The sending process uses the parameter 'td' to calculate
//  and control the timeout for each packet, while the receiving process does not
//  use in any way the fields 'td' of received packets, because it has a detection
//  mechanism of inactivity. The maximum inactivity time (MAX_INACTIVITY_TIME)
//  is configurable in 'settings.h'. Moreover, you can also change the maximum
//  number of bytes that can be sent with each packet, changing the value of
//  MAX_BLOCK_SIZE in 'settings.h'.


#ifndef __Reliable_UDP__packet__
#define __Reliable_UDP__packet__

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "settings.h"
#include "time_data.h"


/*  This enumeration contains all the possible types of packets that the client
 *  and server can send and receive. The first three types are used to establish
 *  the connection between client and server.
 *
 *  =============================================================================
 *  |PKT_LS:     It is a request to get a list of available files on the server |
 *  |PKT_GET:    It is a request to receive a specific file                     |
 *  |PKT_PUT:    It is a request to send a file                                 |
 *  =============================================================================
 *
 *  PKT_HELP:           Used to identify (locally) a request of help to see all available
 *                      command that you can use with the program
 *  PKT_LANG:           Used locally to select a text language
 *  PKT_INFO:           Not used in this release: it is designed for sending information
 *                      messages between client and server
 *  PKT_ACK:            Is used to send an ACK for a packet correctly received
 *  PKT_DATA:           It is used to send a packet that contains parts of the file you
 *                      want to send or receive
 *  PKT_FIN:            It is used to send the last packet wich indicates that the
 *                      file has been completely sent
 *  PKT_FINACK:         It is used to send the last ACK that declares the end of
 *                      the connection between client and server
 *  PKT_ERR:            It is used to send an error message between client and
 *                      server and to stop the connection
 *  PKT_REQ:            In the 'recovery_mode', it is used to request the forced
 *                      sending of a packet with a given sequence number
 *
 *  In a future release, PKT_HELP and PKT_LANG could be used not only locally ,
 *  but respectively to request the help page for the server configuration and 
 *  the language setting for the server.
 */
enum packet_type {PKT_LS, PKT_GET, PKT_PUT, PKT_HELP, PKT_LANG, PKT_INFO, PKT_ACK, PKT_DATA, PKT_FIN, PKT_FINACK, PKT_ERR, PKT_REQ};


struct packet {
    struct time_data *td;       //Pointer to a 'time_data' structure
    int type;                   //Indicates the type of the packet
    long long int seq;          //Sequence number (It is unique for each packet)
    int acked;                  //Indicates if a packets was acked (1) or not (0)
    char data[MAX_BLOCK_SIZE];  //Data read from the file
    size_t dimension;           //Real size of the 'data' field
    int retries;                //How many times it was sent back a package
};


/*  This function is used to allocate memory and initialize a data structure
 *  'packet'.
 *
 *  Parameters:
 *  - type:             The type (from 'packet_type' enumeration) of the packet
 *  - seq:              The sequence number of the packet
 *  - data:             bytes to be sent with the packet; if NULL, the field is
 *                      filled by '\0'
 *  - dimensio:         The real dimension of the data field
 *
 *  Return:             Pointer to a new packet
 */
struct packet *new_packet(int type, long long int seq, char *data, size_t dimension);


/*  This function physically deletes a packet
 *
 *  Parameters:
 *  - pkt:              The packet you want to delete
 *
 *  Return:             Nothing
 */
void packet_delete(struct packet *pkt);


#endif /* defined(__Reliable_UDP__packet__) */
