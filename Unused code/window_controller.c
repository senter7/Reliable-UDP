//
//  window_controller.c
//  New Reliable UDP
//
//  Created by Simone Minasola on 29/10/15.
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

#include "window_controller.h"

struct window_controller *new_window_controller(struct time_controller *tc, int sockfd, struct sockaddr_in addr, int output) {
    struct window_controller *wc = malloc(sizeof(struct window_controller));
    if (wc == NULL) {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }
    
    if(pthread_mutex_init(&wc->MTX, NULL) != 0) {
        fprintf(stderr, "Error in newWindowController(): cannot initialize mutex\n");
        exit(EXIT_FAILURE);
    }
    
    if (pthread_cond_init((&wc->empty), NULL) != 0 || pthread_cond_init(&wc->full, NULL) != 0) {
        fprintf(stderr, "Error in newWindowController(): cannot create conditions\n");
        exit(EXIT_FAILURE);
    }
    
    wc->root = NULL;
    wc->tc = tc;
    wc->dynamic_timeout.tv_sec = 0;
    wc->dynamic_timeout.tv_usec = 0;
    wc->addr = addr;
    wc->sockfd = sockfd;
    wc->output = output;
    wc->current_window_dimension = 0;
    
    return wc;
}

void window_controller_add_packet(struct window_controller *wc, struct packet *pkt) {
    get_mutex(&wc->MTX);
    
    while (wc->current_window_dimension == WINDOW_DIMENSION)
        //in attesa che si liberi uno slot
        pthread_cond_wait(&wc->full, &wc->MTX);
    
    //inserisci pacchetto nella finestra
    wc->root = node_insert(wc->root, pkt);
    //aumenta numero di slot occupati
    wc->current_window_dimension++;
    //quando inserisco un pacchetto la finestra ne contiene almeno uno da poter prelevare
    pthread_cond_signal(&wc->empty);
    
    release_mutex(&wc->MTX);
    
    
    //se è il server che mette il pkt nella finestra, allora invialo e inserisci timer
    //se wc->tc != NULL, vuol dire che è il server a chiamare questa funzione
    if (wc->tc != NULL) {
        send_pkt(wc->sockfd, pkt, wc->addr);
        
        //inserisco il timer del pacchetto nella timer_wheel gestita dal time_controller
        if (wc->dynamic_timeout.tv_sec == 0 && wc->dynamic_timeout.tv_usec == 0) {
            pkt->td->timeout.tv_sec = DEFAULT_TIMEOUT_SEC;
            pkt->td->timeout.tv_usec = DEFAULT_TIMEOUT_USEC;
        }
        else {
            pkt->td->timeout.tv_sec = wc->dynamic_timeout.tv_sec;
            pkt->td->timeout.tv_usec = wc->dynamic_timeout.tv_usec;
        }
        
        time_controller_add_new_timer(pkt->td, wc->tc);
    }
    
}

void delete_contiguous(struct window_controller *wc) {
    struct tree_node *node;
    int deleted = 0;
    
    get_mutex(&wc->MTX);
    
    while (wc->current_window_dimension != 0) {
        node = find_min(wc->root);
        
        //se node==NULL allora l'albero è vuoto
        if (node == NULL)
            break;
        
        //se non è NULL, controllo se è acked
        else if (node->pkt->acked == 1) {
            deleted = 1;
            node_delete(wc->root, node->pkt->seq);
        }
        
        //se non è NULL ma non è neanche ACKED
        else
            break;
    }
    
    //se ho cancellato almeno un pkt dalla windows, allora non è piu piena
    if (deleted == 1)
        pthread_cond_signal(&wc->full);
}

void calculate_dynamic_timeout(struct window_controller *wc) {
    wc->estimatedRTT = 0.875 * wc->estimatedRTT + 0.125 * wc->sampleRTT;
    
    wc->devRTT = 0.75 * wc->devRTT + 0.25 * fabsl(wc->sampleRTT - wc->estimatedRTT);
    
    double timeout = wc->estimatedRTT + 4 * wc->devRTT;
    
    long int sec = timeout / 1000000;
    
    wc->dynamic_timeout.tv_sec = sec;
    wc->dynamic_timeout.tv_usec = timeout - sec * 1000000;
}

int window_controller_set_ack(struct window_controller *wc, long long int seq) {
    struct tree_node *node;
    
    get_mutex(&wc->MTX);
    
    node = find_node(wc->root, seq);
    
    if (node == NULL) {
        release_mutex(&wc->MTX);
        return 0;
    }
    
    else {
        node->pkt->acked = 1;
        
        //memorizzo il tempo di arrivo per utilizzi futuri
        gettimeofday(&node->pkt->td->time_recv, NULL);
        
        //aggiorno il sampleRTT ad ogni nuovo ack
        wc->sampleRTT = (node->pkt->td->time_recv.tv_sec - node->pkt->td->time_send.tv_sec) * 1000000 + (node->pkt->td->time_recv.tv_usec - node->pkt->td->time_send.tv_usec);
        
        //se sto inviando il file, allora cancello i pkt ricevuti dalla finestra
        if (wc->output == 1)
            delete_contiguous(wc);
    }
    
    //ad ogni arrivo di un ack ricalcolo dinamicamente il timeout
    calculate_dynamic_timeout(wc);
    
    release_mutex(&wc->MTX);
    
    return 1;
}

int window_controller_remaining_slots(struct window_controller *wc) {
    int slots = 0;
    get_mutex(&wc->MTX);
    slots = WINDOW_DIMENSION - wc->current_window_dimension;
    release_mutex(&wc->MTX);
    return slots;
}

int window_controller_resend_packet(struct window_controller *wc, long long int seq) {
    struct tree_node *node;
    
    get_mutex(&wc->MTX);
    
    node = find_node(wc->root, seq);
    
    if (node == NULL) {
        release_mutex(&wc->MTX);
        return 1;
    }
    
    else {
        node->pkt->retries++;
        
        if (node->pkt->retries == MAX_RETRIES_SENDING_PKT) {
            fprintf(stderr, "TROPPE TENTATIVI DI INVIO!! LINEA OCCUPATA O CADUTO COLLEGAMENTO?\n");
            //POTREI MANDARE UN PACCHETTO SPECIALE DI ERRORE, MA SE NON C'E PIU CONNESSIONE
            //ALLORA E' INUTILE!! MEGLIO LANCIARE UN TIMER AL SERVER E QUANDO E' TROPPO
            //TEMPO CHE ASPETTA ALLORA INTERROMPE RICEZIONE
            exit(EXIT_FAILURE);
        }
        
        send_pkt(wc->sockfd, node->pkt, wc->addr);
        
        //NON C'E' BISOGNO DI USARE IL SEMAFORO SUL TIME_CONTROLLER PERCHE è PROPRIO IL
        //TIME CONTROLLER CHE CHIAMA QUESTA FUNZIONE
        gettimeofday(&node->pkt->td->time_send, NULL);
        
        node->pkt->td->timeout.tv_sec = node->pkt->td->timeout.tv_sec * 2;
        node->pkt->td->timeout.tv_usec = node->pkt->td->timeout.tv_usec * 2;
        
        release_mutex(&wc->MTX);
        
        return 0;
    }
}

long long int write_contiguous(struct window_controller *wc, long long int min) {
    long long int last = min;
    int deleted = 0;
    struct tree_node *node;
    ssize_t m;
    
    get_mutex(&wc->MTX);
    
    //continuo al massimo fino a svuotare la finestra
    while (wc->current_window_dimension != 0) {
        node = find_min(wc->root);
        
        //se node==NULL allora l'albero è vuoto
        if (node == NULL)
            break;
        
        if (node->pkt->seq == min) {
            m = write(wc->output, (void *) node->pkt->data, node->pkt->dimension);
            if (m == -1 || m != node->pkt->dimension) {
                fprintf(stderr, "Error in writeContiguous(): cannot write on output file\n");
                exit(EXIT_FAILURE);
            }
            last++;
            deleted = 1;
            node_delete(wc->root, node->pkt->seq);
            wc->current_window_dimension--;
        }
        
        else
            break;
    }
    
    //se almeno un elemento è stato tolto dalla finestra, allora la finestra non è piu piena!
    if (deleted == 1)
        pthread_cond_signal(&wc->full);
    
    return last;
}

void window_controller_dispose(struct window_controller *wc) {
    dispose_tree(wc->root);
    free(wc);
}