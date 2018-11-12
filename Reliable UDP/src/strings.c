//
//  strings.c
//  Reliable UDP
//
//  Created by Simone Minasola on 16/11/15.
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

#include "strings.h"

/* Global variables */
char **STRINGS = NULL;          //Global array
int TOTAL_STRINGS_SAVED = 60;   //Total strings number
int LANG_TYPE = 0;              //Language ID

/* This function loads the english language */
void load_en_lang() {
    char *en_lang[1024] = {
        "  Welcome to Reliable UDP Server",
        "  Welcome to Reliable UDP Client",
        "Log service",
        "Verbose service",
        "OK",
        "ERROR",
        "ON",
        "OFF",
        "Command not found",
        "Server started",
        "waiting for connection request...",
        "request accepted",
        "max processes number reached. Request not accepted",
        "communication port selected",
        "creating child process...",
        "Child",
        "Operation started",
        "packets to receive",
        "Received pkt with seq",
        "The resource text does not exists",
        "Launch the program with: '-l' and/or '-v'",
        "Main process",
        "Date",
        "Time",
        "Not valid",
        "PKT_FIN reveiced",
        "Total time elapsed",
        "Total packets received",
        "Total packets discarded",
        "Average time of receipt",
        "Report",
        "Completition",
        "Operation completed",
        "Operation interrupted",
        "Max inactivity time elapsed",
        "try later",
        "Line too busy or connection interrupted",
        "File not found",
        "packets to send",
        "ACK received with seq",
        "PKT_FINACK received",
        "Sent packet with seq",
        "Total packets send",
        "Average time to send",
        "Launch the program with: 'IP addr.' and '-l' and/or '-v'",
        "IP address not valid",
        "Select a language:\nITA (italian)\nENG (english)",
        "English language selected",
        "No language recognized",
        "Shutdown",
        "Connection closed",
        "Client started",
        "List generated at",
        "PUT <file>, GET <file>, LIST, HELP or LANG <ita/eng>",
        "Problem detected. Control FIRST_AVAILABLE_PORT in 'settings.h'",
        "Server busy. Try later.",
        "Error: packet not found in window",
        "I cannot find help file in the selected language. I open english file.",
        "I cannot open help file!",
        "Upload speed",
        "Download speed"
    };
    
    STRINGS = en_lang;      //Set the pointer of the global array
    LANG_TYPE = LANG_EN;    //Set the new language ID
}


/* This function loads the italian language */
void load_it_lang() {
    char *it_lang[1024] = {
        "Benvenuto in Reliable UDP Server",
        "Benvenuto in Reliable UDP Client",
        "Ser. di log",
        "Ser. di verbose",
        "OK",
        "ERRORE",
        "ON",
        "OFF",
        "Comando non trovato",
        "Server partito",
        "in attesa di richieste di connessione...",
        "richiesta accettata",
        "numero massimo di processi raggiunto. Richiesta non accettata.",
        "porta di comunicazione selezionata",
        "creazione del processo figlio...",
        "Figlio",
        "Operazione avviata",
        "pacchetti da ricevere",
        "Ricevuto pkt con seq",
        "La risorsa testuale non esiste",
        "Lanciare con le seguenti opzioni : '-l' e/o '-v'",
        "Processo principale",
        "Data",
        "Ora ",
        "Non valido",
        "PKT_FIN ricevuto",
        "Tempo totale trascorso",
        "Totale pacchetti ricevuti",
        "Totale pacchetti scartati",
        "Tempo medio di ricezione",
        "Resoconto",
        "Completamento",
        "Operazione completata",
        "Operazione interrotte",
        "Tempo di inattivita' massimo raggiunto",
        "Riprova",
        "Linea sovraccarica o comunicazione interrotta",
        "File non trovato",
        "pacchetti da spedire",
        "ACK ricevuto con seq",
        "PKT_FINACK ricevuto",
        "Spedito pacchetto con seq",
        "Totale pacchetti spediti",
        "Tempo medio per l'invio",
        "Lanciare con le seguenti opzioni : 'IP' e '-l' e/o '-v'",
        "Indirizzo IP non valido",
        "Seleziona una lingua:\nITA (italiano)\nENG (inglese)",
        "Lingua italiana impostata",
        "Nessuna lingua riconosciuta",
        "Spegnimento",
        "Chiudo connessione",
        "Client partito",
        "Lista generata il",
        "PUT <file>, GET <file>, LIST, HELP o LANG <ita/eng>",
        "Problema riscontrato. Controlla FIRST_AVAILABLE_PORT in 'settings.h'",
        "Server occupato. Riprova piu' tardi",
        "Errore: pacchetto non trovato nella finestra",
        "File di aiuto non trovato per la lingua impostata. Apro in inglese.",
        "Non riesco ad aprire il file di aiuto!",
        "Velocita' di caricamento",
        "Velocita' di scaricamento"
    };
    
    STRINGS = it_lang;      //Set the pointer of the global array
    LANG_TYPE = LANG_IT;    //Set the new language ID
}



void select_language(int lang_id) {
    //Identify the correct language to load
    switch (lang_id) {
        case LANG_EN:
            load_en_lang();
            break;
        case LANG_IT:
            load_it_lang();
            break;
        default:
            load_en_lang();
            break;
    }
}



char *get_text(int string_id) {
    //If is passed a wrong 'string_id', return an error string.
    //Otherwise, return the correct string
    if (string_id > TOTAL_STRINGS_SAVED || string_id < 0)
        return STRINGS[STRING_PROBLEM_STRING];
    return STRINGS[string_id];
}



int get_total_strings() {
    return TOTAL_STRINGS_SAVED;
}



int get_lang_type() {
    return LANG_TYPE;
}