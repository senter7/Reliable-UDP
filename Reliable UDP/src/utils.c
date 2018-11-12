//
//  utils.c
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


#include "utils.h"


int open_file(unsigned int ctrl, const char *path) {
    int fd;
    if(ctrl == WRITE)
        fd = open(path, O_WRONLY|O_CREAT, 0664);
    else
        fd = open(path, O_RDONLY);
    
    if(fd == -1) {
        perror("open_file()");
        exit(EXIT_FAILURE);
    }
    
    return fd;
}



void close_file(int fd) {
    if(close(fd) != 0) {
        perror("close_file()");
        exit(EXIT_FAILURE);
    }
}



FILE *fopen_file(const char *path) {
    FILE *f;
    
    f = fopen(path, "a");
    if (f == NULL) {
        perror("fopen_file()");
        exit(EXIT_FAILURE);
    }
    
    return f;
}


void fclose_file(FILE *fp) {
    if (fclose(fp) == 0) {
        perror("fclose_file()");
        exit(EXIT_FAILURE);
    }
}


char *search_file(char *filename) {
    
    int fd, i = 0, retries = 0;
    //Adjust filename with fullpath ('data/')
    char *copy = malloc((strlen(filename) + strlen(DATA_DIR)) * sizeof(char));
    if (snprintf(copy, (strlen(filename) + strlen(DATA_DIR) + 2) * sizeof(char), "%s/%s", DATA_DIR, filename) < 0) {
        perror("first snprintf() in search_file()");
        exit(EXIT_FAILURE);
    }
    
    while ((fd = open(copy, O_RDONLY)) != -1) {
        //Attempts to search a unique file
        retries++;
        
        if (retries == 1 || retries == 10 || retries == 100 || retries == 1000 || retries == 10000)
            i++;
        //  every time I find a name already used, realloc memory and updates the file name
        if (retries == 1)
            copy = realloc(copy, (strlen(filename) + 1) * sizeof(char) + 2 * sizeof(char) + i * sizeof(char));
        else
            copy = realloc(copy, (strlen(filename) + 1) * sizeof(char) + i * sizeof(char));
        
        if (copy == NULL) {
            perror("realloc() in search_file()");
            exit(EXIT_FAILURE);
        }
        //Write new filename (example: '(3)my_program.exe')
        if (snprintf(copy, ((strlen(filename) + strlen(DATA_DIR)) + 2) * sizeof(char) + i * sizeof(char) + 2 * sizeof(char), "%s/(%d)%s", DATA_DIR, retries, filename) < 0) {
            perror("second snprintf() in search_file()");
            exit(EXIT_FAILURE);
        }
        close_file(fd);
    }
    return copy;
}


/*  This function compare two packet and returns -1 if
 *  pkt1 has the sequence number less than pkt2, otherwise it returns 1.
 *  If sequence numbers are the same, it returns 0.
 *  (used by 'qsort()' function)
 */
int compare(const void *a, const void *b) {
    struct packet *pkt1 = (struct packet *) a;
    struct packet *pkt2 = (struct packet *) b;
    
    if (pkt1->seq < pkt2->seq)
        return -1;
    else if (pkt1->seq > pkt2->seq)
        return 1;
    else
        return 0;
}



void sort_window(struct window *w) {
    //Save the indexes
    int nE = w->E;
    int nS = w->S;
    
    int i = 0;
    int N;
    struct packet *v;  //Linear temporary buffer
    
    /*  Calculate how many packets need to be ordered
     *
     *  Example:
     *
     *  1)  N = 6 - 4 + 1 = 3
     *
     *    0   1   2   3   4   5
     *   -----------------------
     *  | x |   |   |   | x | x |   dim = 6
     *   -----------------------
     *        E           S
     *
     *
     *  2)  N = 4 - 1 = 3
     *
     *    0   1   2   3   4   5
     *   -----------------------
     *  |   | x | x | x |   |   |   dim = 6
     *   -----------------------
     *        S           E
     */
    if (nE < nS)
        N = (w->dim - nS + nE);
    else
        N = nE - nS;
    //Allocate memory
    v = malloc(sizeof(struct packet) * N);
    if (v == NULL) {
        perror("malloc() in sort_window()");
        exit(EXIT_FAILURE);
    }
    
    struct packet *temp;
    //Empty the window and fill the buffer
    while (window_is_empty(w) == 0) {
        temp = window_get_pkt(w);
        v[i] = *temp;
        ++i;
    }
    //Sort the buffer
    qsort(v, N, sizeof(struct packet), compare);
    //Reset indexes
    w->E = 0;
    w->S = 0;
    //Insert ordered packets into window again
    for (i = 0; i<N; ++i)
        window_add_pkt(w, &v[i]);
    
    free(v);
}


unsigned long long get_dimension(const char *f) {
    struct stat sstr;
    //Retrieve information about 'f'
    if(stat(f, &sstr) == -1) {
        perror("stat() in get_dimension()");
        exit(EXIT_FAILURE);
    }
    //Return the dimension
    return (unsigned long long) sstr.st_size;
}

unsigned long long int get_number(unsigned long long size) {
    unsigned long long n;
    
    if (size % MAX_BLOCK_SIZE == 0)
        n = size / MAX_BLOCK_SIZE;
    else
        n = (size / MAX_BLOCK_SIZE) + 1;
    
    return n;
}

void get_mutex(pthread_mutex_t *MTX) {
    if(pthread_mutex_lock(MTX) == -1){
        perror("get_mutex()");
        exit(EXIT_FAILURE);
    }
}

void release_mutex(pthread_mutex_t *MTX) {
    if(pthread_mutex_unlock(MTX) == -1){
        perror("release_mutex()");
        exit(EXIT_FAILURE);
    }
}

void msleep(int msec) {
    struct timespec interval;
    struct timespec remainder;
    
    interval.tv_sec = msec / 1000;
    interval.tv_nsec = (msec % 1000) * (1000 * 1000);
    
    if (nanosleep(&interval, &remainder) == -1) {
        if (errno == EINTR) {
            /* printf("nanosleep interrupted\n"); */
            /* printf("Remaining secs: %ld", remainder.tv_sec); */
            /* printf("Remaining nsecs: %ld", remainder.tv_nsec); */
        }
        
        else if (errno == EINVAL);
            /* (void)printf("errno == EINVAL\n"); */
        
        else {
            perror("nanosleep() in msleep()");
            exit(EXIT_FAILURE);
        }
    }
}

int read_operation(char *line) {
    int op;
    
    if (strncmp(line, "PUT ", 4) == 0)
        op = PKT_PUT;
    else if (strncmp(line, "GET ", 4) == 0)
        op = PKT_GET;
    else if (strncmp(line, "LIST", 4) == 0)
        op = PKT_LS;
    else if (strncmp(line, "HELP", 4) == 0)
        op = PKT_HELP;
    else if (strncmp(line, "LANG", 4) == 0)
        op = PKT_LANG;
    else
        op = PKT_ERR; //Now it is used locally
    
    return op;
}

void send_pkt(int sockfd, struct packet *pkt, struct sockaddr_in addr) {
    if(sendto(sockfd, pkt, sizeof(struct packet), 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("sendto() in send_pkt()");
        exit(EXIT_FAILURE);
    }
}

struct packet *recv_pkt(int sockfd, struct sockaddr_in *addr, socklen_t *len) {
    struct packet *pkt = malloc(sizeof(struct packet));
    
    if (pkt == NULL) {
        perror("malloc() in recv_pkt()");
        exit(EXIT_FAILURE);
    }
    
    ssize_t n;
    //Receive the data from network and put it into the new packet
    n = recvfrom(sockfd, pkt, sizeof(struct packet), 0, (struct sockaddr*)addr, len);
    if(n < 0) {
        perror("recvfrom() in recv_pkt()");
        exit(EXIT_FAILURE);
    }
    
    return pkt;
}

int is_accepted() {
    return (abs((rand() % 100)) < LOSS_PROBABILITY) ? 0 : 1;
}


char *convert_llint(long long int num) {
    char *v = malloc(sizeof(char) * 2048);
    if (v == NULL) {
        perror("malloc() in convert_llint()");
        exit(EXIT_FAILURE);
    }
    if (snprintf(v, 2048, "%lld", num) < 0) {
        perror("snprintf() in convert_llint()");
        exit(EXIT_FAILURE);
    }
    return v;
}


char *convert_int(int port) {
    char *v = malloc(sizeof(char) * 12);
    if (v == NULL) {
        perror("malloc() in convert_int()");
        exit(EXIT_FAILURE);
    }
    if (snprintf(v, 12, "%d", port) < 0) {
        perror("snprintf() in convert_int()");
        exit(EXIT_FAILURE);
    }
    return v;
}

char *get_current_time() {
    struct tm *newtime;
    time_t t;
    char *msg;
    int result;
    //Get the value of time in seconds since 0 hours, 0 minutes, 0 seconds, January 1, 1970
    time(&t);
    if (t == (time_t) -1) {
        perror("time() in get_current_time()");
        exit(EXIT_FAILURE);
    }
    //Convert time
    newtime = localtime(&t);
    if (newtime == NULL) {
        perror("newtime() in get_current_time()");
        exit(EXIT_FAILURE);
    }
    //Allocate memory for the string
    msg = malloc(sizeof(char) * 1024);
    if (msg == NULL) {
        perror("malloc() in get_current_time()");
        exit(EXIT_FAILURE);
    }
    //Copy time into string
    result = snprintf(msg, 1024, "[%2d:%2d:%2d]", newtime->tm_hour, newtime->tm_min, newtime->tm_sec);
    if (result < 0) {
        perror("snprintf() in get_current_time()");
        exit(EXIT_FAILURE);
    }
    
    return msg;
}

char *get_current_data() {
    struct tm *newtime;
    time_t t;
    char *msg;
    int result;
    //Get the value of time in seconds since 0 hours, 0 minutes, 0 seconds, January 1, 1970
    time(&t);
    if (t == (time_t) -1) {
        perror("time() in get_current_data()");
        exit(EXIT_FAILURE);
    }
    //Convert time
    newtime = localtime(&t);
    if (newtime == NULL) {
        perror("newtime() in get_current_data()");
        exit(EXIT_FAILURE);
    }
    //Allocate memory for the string
    msg = malloc(sizeof(char) * 1024);
    if (msg == NULL) {
        perror("malloc() in get_current_data()");
        exit(EXIT_FAILURE);
    }
    //Copy data into string
    result = snprintf(msg, 1024, "%d.%d.%d", newtime->tm_mday, newtime->tm_mon + 1, newtime->tm_year + 1900);
    if (result < 0) {
        perror("snprintf() in get_current-data()");
        exit(EXIT_FAILURE);
    }

    return msg;
}

/* NOT USED
 
 char *create_string(int num, ...) {
    char *string = malloc(sizeof(char) * 256);
    if (string == NULL) {
        fprintf(stderr, "MALLOC()\n");
        exit(EXIT_FAILURE);
    }
    
    int arg; //contatore argomenti
    va_list ap; //lista argomenti
    
    va_start(ap, num); //inizializzazione lista argomenti
    
    for (arg=0; arg<num; ++arg) {
        char *m = va_arg(ap, char*);
        if (m != NULL) {
            strncat(string, m, 256);
            strncat(string, " ", 256);
        }
    }
    
    va_end(ap);
    
    return string;
}
*/
