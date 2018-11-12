//
//  verbose_service.h
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

#ifndef __Reliable_UDP__verbose_service__
#define __Reliable_UDP__verbose_service__

#include "log_msg.h"
#include "settings.h"
#include "strings.h"

#include <stdio.h>
#include <stdlib.h>

void verbose_success_msg(char *msg, char *operation, int detail);

void verbose_info_msg(char *msg, char *operation, int detail);

void verbose_error_msg(char *msg, char *operation, int detail);

void verbose_warning_msg(char *msg, char *operation, int detail);

#endif /* defined(__Reliable_UDP__verbose_service__) */
