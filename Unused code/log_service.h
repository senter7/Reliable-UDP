//
//  log_service.h
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

#ifndef __Reliable_UDP__log_service__
#define __Reliable_UDP__log_service__

#include "verbose_service.h"
#include "settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


//questa enumerazione serve perchè il server tiene un unico file di log
//sempre aggiornato. Il client invece ne crea uno per ogni giorno
enum log_service_user {LS_SERVER, LS_CLIENT};

struct log_service {
    FILE* log;
    char filename[32];
    int detail;
    
};

//IL SERVER TIENE UN UNICO FILE DI LOG PER SEMPRE. IL CLIENT
//NE CREA UNO PER OGNI GIORNO. ENTAMBI I FILE DI LOG NON SONO MOLTO
//DETTAGLIATI...OVVERO NON DEVONO CONTENERE TRACCIA DI TUTTI I PACCHETTI
//INVIATI O DI TUTTI I PACCHETTI RICEVUTI, MA SOLO DELLE OPERAZIONI PIU
//IMPORTANTI, TIPO "Sending file..." "File send!" "Connection ok", ecc.

//RIGUARDO LA VERBOSE MODE VISUALIZZABILE A SCHERMO, PUO ESSERE PIU O MENO DETTAGLIATA.
//LA VERBOSE SI BASA SEMPRE SULLA STRUTTURA LOGMSG, MA PRINTA SOLO A SCHERMO.

struct log_service *new_log_service(const char *path, int detail, int user);

void log_info_msg(struct log_service *ls, char *msg);

void log_error_msg(struct log_service *ls, char *msg);

void log_warning_msg(struct log_service *ls, char *msg);

void log_success_msg(struct log_service *ls, char *msg);

#endif /* defined(__Reliable_UDP__log_service__) */
