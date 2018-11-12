# commento

# definizione delle variabili
CC=gcc
CFLAGS= -Wall -Wextra
CFILES=$(shell cd src/ls *.c)
PROGS=$(CFILES:%.c=%)
DIR=$(shell cd src)

# NOME: PREREQUISITO
# [TAB] COMANDO


CLIENT: src/strings.h src/window_controller.h src/time_controller.h src/timer_wheel.h src/window.h src/utils.h src/packet.h src/time_data.h src/put.h src/get.h src/list.h src/settings.h src/timer.h src/server_status.h src/print_messages.h src/client.c
	$(CC) $(CFLAGS) -pthread src/strings.c src/window_controller.c src/time_controller.c src/timer_wheel.c src/window.c src/utils.c src/packet.c src/time_data.c src/put.c src/get.c src/list.c src/timer.c src/server_status.c src/print_messages.c src/client.c -o RUDP_client
	@echo "\033[32mClient: SUCCESS\033[0m"

SERVER: src/strings.h src/window_controller.h src/time_controller.h src/timer_wheel.h src/window.h src/utils.h src/packet.h src/time_data.h src/put.h src/get.h src/list.h src/settings.h src/timer.h src/server_status.h src/print_messages.h src/server.c
	$(CC) $(CFLAGS) -pthread src/strings.c src/window_controller.c src/time_controller.c src/timer_wheel.c src/window.c src/utils.c src/packet.c src/time_data.c src/put.c src/get.c src/list.c src/timer.c src/server_status.c src/print_messages.c src/server.c -o RUDP_server
	@echo "\033[32mServer: SUCCESS\033[0m"
	



# %: %.c
# 	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	rm -f $(PROGS) *.o
