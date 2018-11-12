#!/bin/bash

#Clear screen
clear

#Delete server folder if exists
if [ -e "server" ];
    then
        rm -r server
fi

#Delete client folder if exists
if [ -e "client" ];
    then
        rm -r client
fi

echo "\033[32mBuilding executables..."; tput sgr0

#Build client program
make CLIENT

#Build server program
make SERVER

echo "\033[32mCreating folders..."; tput sgr0

#Create folder tree
mkdir server        #server directory
mkdir server/data   #data directory for the server
mkdir server/log    #log directory for the server
mkdir server/temp   #temp directory to save temporarily list file

mkdir client        #client directory
mkdir client/data   #data directory for the client
mkdir client/log    #log directory for the client
mkdir client/docs   #doc directory for the client (for help files)

echo "\033[32mFolders created!"; tput sgr0

echo "\033[32mMoving files..."; tput sgr0

#Check if the makefile generated the executable
if [ -e "RUDP_server" ];
#If yes, copy exec into folder, else exit
    then
        mv RUDP_server server
    else
        echo "\033[31mExecutable missing!"; tput sgr0
        exit
fi

#Check if the makefile generated the executable
if [ -e "RUDP_client" ];
#If yes, copy exec into folder, else exit
    then
        mv RUDP_client client
    else
        echo "\033[31mExecutable missing!"; tput sgr0
        exit
fi

#Copy all file from data_example to data dir on server
for f in $( ls data_example );
do
    cp data_example/$f server/data
done

#Copy all file from help_files to docs dir on client
for f in $( ls help_files );
do
    cp help_files/$f client/docs
done

echo "\033[32mFiles moved!"; tput sgr0

#end message
echo "\033[32mInstallation complete."; tput sgr0
echo "\033[32mYou can move folders \033[33mserver\033[32m and \033[33mclient\033[32m anywhere."; tput sgr0