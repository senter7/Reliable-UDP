# Reliable UDP
This software was written for the Internet and Web Engineering course, degree in Computer Engineering, Tor Vergata.

# Table of Contents
1. [Project purpose](#purpose)
    - [Abstract](#purpose_abstract)
    - [Communication protocol](#purpose_com_prot)
    - [Server](#purpose_server)
    - [Client](#purpose_aclient)
    - [Reliable trasmission](#purpose_rel)
    - [Optional request](#purpose_opt)
2. [Folder organization](#folder)
3. [License](#license)

<a name="purpose"></a>
## Purpose
<a name="purpose_abstract"></a>
#### Abstract
The aim of the project is to design and implement in C language using the Socket API of Berkeley a client-server application for file transfer using the network service without connection (SOCK DGRAM socket, or UDP as protocol of transport layer).
The software must allow:
- Client-server connection without authentication;
- The display on the client of the files available on the server (list command);
- Downloading a file from the server (get command);
- Uploading a file to the server (put command);
- File transfer reliably.

<a name="purpose_comm_prot"></a>
#### Communication protocol
Communication between client and server must take place via an appropriate protocol. The communication protocol must provide for the exchange of two types of messages:
- ***command messages:*** are sent from the client to the server to request execution of the various operations;
- ***response messages:*** are sent from the server to the client in response to a command with the outcome of the operation.

<a name="purpose_server"></a>
#### Server functionality
The server, of a competing type, must provide the following features:
- Sending the reply message to the ****list**** command to the requesting client; the message of answer contains the filelist, that is the list of the names of the files available for sharing;
- Sending the reply message to the ****get**** command containing the requested file, if any, or a suitable error message;
- Receiving a ****put**** message containing the file to be uploaded to the server and sending a reply message with the result of the operation.

<a name="purpose_client"></a>
#### Client functionality
The concurrent client must provide the following features:
- Sending the message list to request the list of available file names;
- Sending the get message to get a file;
- The reception of a requested file through the get message or the management of the eventuality mistake;
- Sending the put message to upload a file on the server and receiving the
response message with the result of the operation;

<a name="purpose_rel"></a>
#### Reliable transmission
Message exchange takes place using an unreliable communication service. In order to ensure correct delivery/receipt of messages and files, both client and server implement (at the application level) the selective repeat protocol with shipping window ***N***.

To simulate the loss of messages on the network (a very unlikely event in a local network not to mention when client and server are running on the same host), it is assumed that each message is discarded from the sender with probabilities ***p***.

The size of the shipping window ***N***, the probability of loss of messages ***p***, and the duration of the ***T*** timeout, they are three configurable and equal constants for all processes.

The client and server must run in user space without requiring root privileges. The server must listen on a default port (configurable)

<a name="purpose_opt"></a>
#### Optional (done)
Each process dynamically calculates the value of the retransmission T timeout based on the dynamics of network delay times.

<a name="folder"></a>
## Folder Organization
```bash
.
├── LICENSE #GNU GPL V3 License copy
├── README.md 
├── Reliable UDP #Source code
├── Report #Detailed documentation
├── Results #Test results
└── Unused code #Unused code
```

<a name="license"></a>
## License
[GNU GPL V3](https://www.gnu.org/licenses/gpl-3.0.html)
