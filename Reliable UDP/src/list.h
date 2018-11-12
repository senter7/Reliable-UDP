//
//  list.h
//  Reliable UDP
//
//  Created by Simone Minasola on 08/11/15.
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
//  This header file contains functions that allows to create, read and delete
//  a file saved in a specific directory.
//  These functions are used respectively by server process (create and delete)
//  and client process (read). The name of file that 'create_list' creates is editable
//  in 'settings.h', as the name of file read by 'create_list'. See 'settings.h'
//  for details.
//  PS:
//  When the server creates a list, it is temporarily save into 'temp/' directory,
//  to avoid sending a list in which there is also the newly created file.


#ifndef __Reliable_UDP__list__
#define __Reliable_UDP__list__

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "utils.h"
#include "settings.h"
#include "window_controller.h"
#include "time_controller.h"
#include "strings.h"



/*  This function is used by client process to read the file 'LIST_FILE'.
 *  The path is editable in 'settings.h'
 *
 *  Parameters:         Nothing
 *
 *  Return:             Nothing
 *
 *  Effects:
 *  This function prints on screen the contents of the file saved.
 */
void print_list();


/*  This function is used by server process to create a file with the name of
 *  all files in the folder 'DATA_DIR'. The path is editable in 'settings.h'.
 *  This function can be performed by multiple processes, so the name of file
 *  must be different for each process. For this, the file name is based
 *  on local time.
 *
 *  Parameters:         Nothing
 *
 *  Return:             The name of file just created
 */
char *create_list();


/*  This function can remove a file into a directory.
 *
 *  Parameters:
 *  - filename:         The filename of file wich is going to be deleted
 *
 *  Return:             Nothing
 */
void remove_list(char *filename);

#endif /* defined(__Reliable_UDP__list__) */
