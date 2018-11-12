# Reliable UDP
This software was written for the Internet and Web Engineering course, degree in Computer Engineering, Tor Vergata. For specific details, see [README](https://github.com/senter7/Reliable-UDP/blob/master/README.md) file.

# Table of Contents
1. [Configuration](#configuration)
2. [Installation](#installation)
3. [Generated folders](#after_install)
4. [Launch](#launch)
    - [Server launch](#launch_server)
    - [Client launch](#launch_client)
5. [Folders organization](#source)
5. [License](#license)

<a name="configuration"></a>
## Configuration
Before installing the program, it is recommended to configure the settings. If you omit this step, the software will be compiled and executed with the default settings.
To change the settings, open the `${settings.h} file in the ***src*** folder. This header file contains several user-editable definitions. The most important are:
- **WINDOW DIMENSION**: the size of the shipping window
- **LOSS PROBABILITY**: the percentage that identifies the probability of loss
- **MAX BLOCK SIZE**: the size in bytes of the data field of the packets

Changing any value may create unexpected malfunctions during execution. We recommend that you carefully read the comments and warnings of each definition of the which you want to change the default value. The default values are saved as a comment in the initial part of the file.

<a name="installation"></a>
## Installation
The software installation is completely automatic: a small bash script compiles the source codes with a MakeFile, and creates the necessary folders to launch the program. Below are showed the steps for the installation
1. Copy the Reliable UDP folder to your computer in a place where you have writing and reading privileges (eg Desktop)
2. Open the system terminal, and go (via cd command) inside the folder just copied
3. Run the script with ```sh install.sh``` and wait until the end of the installation. If a problem occurs during the installation procedure, it will be displayed on the screen with an error message
4. The program is ready to be launched

<a name="after_install"></a>
## Generated folders
The program installer creates two new folders within Reliable UDP: **server** and **client**.
These two folders respectively contain the server-side and client-side programs already compiled ready to be launched, as well as essential folders for software operation. The directory trees created during installation are shown below.
```bash
Reliable UDP
    ├── client #Client folder
    │   ├── RUDP_client #Executable (client-side)
    │   ├── data #Files ransferred between client and server
    │   ├── docs #Help files (ita, eng)
    │   └── log #Logs file
    └── server #Server folder
        ├── RUDP_server #Executable (server-side)
        ├── data #Files transferred between client and server
        ├── log #Logs file
        └── temp #Reserved for temporary files
```

<a name="launch"></a>
## Launch
<a name="launch_server"></a>
##### Server  
  
```sh
./RUDP_server [-l logging] [-v verbose mode]
```
In the server folder, launch the program with the following different options (the order of the arguments is not important):
- **./RUDP_server**
Starting the basic program without any additional functionality.
- **./RUDP_server -l**
Launch of the program with creation and management of the log file. For the server, the log file is unique and will only be created once. Every time the server with the argument **-l** starts up, the log file will be updated with the current operations. To consult the log file, open the **LOG.txt** file in the log folder.
- **./RUDP_server -v**
Launch of the program in verbose mode. It will show a series of detailed messages regarding operations in progress on the server. Useful for debugging.
- **./RUDP_server -l -v**
Start of the program in verbose mode and with management of the log file.

<a name="launch_client"></a>
##### Client  
  
```sh
./RUDP_client [Server IP] [-l logging] [-v verbose mode]
```
In the client folder, launch the program with the following different options (outside of the
first, the order of the arguments is not important):
- **./RUDP_client 127.0.0.1**
Starting the basic program without any additional functionality.
- **./RUDP_client 127.0.0.1 -l**
Launch of the program with creation and management of the log file. For the client, it is created
a new log file every day. Every time the client starts with the argument **-l**, the log file will be created/updated with the current operations. To consult the log, open the **LOG [today's date].txt** in the log folder.
- **./RUDP_client 127.0.0.1 -v**
Launch of the program in verbose mode. It will show a series of detailed messages regarding operations in progress on the server. Useful for debugging.
- **./RUDP_client 127.0.0.1 -l -v**
Start of the program in verbose mode and with management of the log file.

<a name="source"></a>
## Folders organization
```bash
Reliable\ UDP
    ├── Makefile
    ├── README.txt
    ├── data_example #Sample file for testing
    │   ├── Python-2.7.10.tgz
    │   ├── gapil.pdf
    │   ├── gettext-0.19.tar.gz
    │   ├── icu4c-56-data.zip
    │   ├── icu4c-56-doc.zip
    │   ├── install-latex.tar.gz
    │   └── tor_vergata.jpg
    ├── help_files #User documentation (lite version)
    │   ├── help_eng.txt
    │   └── help_ita.txt
    ├── install.sh #Installer
    └── src #Source codes
        ├── client.c
        ├── get.c
        ├── get.h
        ├── list.c
        ├── list.h
        ├── packet.c
        ├── packet.h
        ├── print_messages.c
        ├── print_messages.h
        ├── put.c
        ├── put.h
        ├── server.c
        ├── server_status.c
        ├── server_status.h
        ├── settings.h
        ├── strings.c
        ├── strings.h
        ├── time_controller.c
        ├── time_controller.h
        ├── time_data.c
        ├── time_data.h
        ├── timer.c
        ├── timer.h
        ├── timer_wheel.c
        ├── timer_wheel.h
        ├── utils.c
        ├── utils.h
        ├── window.c
        ├── window.h
        ├── window_controller.c
        └── window_controller.h
```

<a name="license"></a>
## License
[GNU GPL V3](https://www.gnu.org/licenses/gpl-3.0.html)
