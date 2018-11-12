//
//  strings.h
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
//
//
//  ABSTRACT
//
//  This header file contains the functions that are used to internationalize the
//  program. The method by which the language is set is very simple: an array of
//  strings is defined as a global variable. When you set a language using
//  'select_language()', the address pointer is updated to a memory area that
//  contains the text strings of the selected language. The function 'get_text()'
//  retrieves the strings to display due to their position, as indicated by
//  '#define' at the beginning of this file. For this reason it is important to
//  maintain the order of the strings.
//  The language will be set at runtime. This will make it possible to change
//  language during program execution .
//  If you want to insert a new language, the steps are the following:
//  1)  create an ID for the language in the enumeration 'language' and add a 'case'
//      into the function 'select_language ()'
//  2)  translate the strings creating an array, keeping the same order, and create
//      the assignment of the global array pointer 'STRINGS'
//  3)  in 'client.c', add the ability for the user to select the new language,
//      following code already written in 'case PKT_LANG'.
//  It is important to note that there are more efficient ways to internationalize
//  a program, such as use tools like 'GNU gettext' or 'International Components
//  for Unicode' (ICU). In this project, it was decided to use this way for two
//  reasons:
//  1)  internationalization is considered secondary to the operation of the program
//  2)  for small projects like this, it was not considered necessary to use professional
//      tools


#ifndef __Reliable_UDP__strings__
#define __Reliable_UDP__strings__

//Abbreviation for the use of 'get_text()'
#define _(str) get_text(str)


/*  This set of definitions are used to search for a particular string in the global
 *  array . No matter which language was selected, because translations respect 
 *  the order of the strings.
 *  See ABSTRACT above for details.
 */
#define STRING_SERVER_HELLO                             0
#define STRING_CLIENT_HELLO                             1
#define STRING_LOG_SERVICE                              2
#define STRING_V_SERVICE                                3
#define STRING_OK                                       4
#define STRING_ERR                                      5
#define STRING_ON                                       6
#define STRING_OFF                                      7
#define STRING_COMMAND_NOT_FOUND                        8
#define STRING_SERVER_STARTED                           9
#define STRING_WAITING_FOR_REQUEST                      10
#define STRING_REQUEST_ACCEPTED                         11
#define STRING_MAX_PROCESSES_NUM                        12
#define STRING_SELECTED_PORT                            13
#define STRING_CREATING_CHILD                           14
#define STRING_CHILD                                    15
#define STRING_OPERATION_STARTED                        16
#define STRING_PKTS_TO_RECV                             17
#define STRING_PKT_RECEIVED                             18
#define STRING_PROBLEM_STRING                           19
#define STRING_INSTRUCTION_SERVER                       20
#define STRING_MAIN_PROCESS                             21
#define STRING_DATE                                     22
#define STRING_TIME                                     23
#define STRING_NOT_VALID                                24
#define STRING_PKT_FIN                                  25
#define STRING_TOTAL_TIME_ELAPSED                       26
#define STRING_TOTAL_PKTS_RECEIVED                      27
#define STRING_TOTAL_PKTS_DISCARDED                     28
#define STRING_AVERAGE_TIME                             29
#define STRING_REPORT                                   30
#define STRING_COMPLETITION                             31
#define STRING_OPERATION_COMPLETED                      32
#define STRING_OPERATION_INTERRUPTED                    33
#define STRING_MAX_INACTIVITY_TIME_ELAPSED              34
#define STRING_TRY_LATER                                35
#define STRING_LINE_BUSY                                36
#define STRING_FILE_NOT_FOUND                           37
#define STRING_PKTS_TO_SEND                             38
#define STRING_ACK_RECEIVED                             39
#define STRING_FINACK_RECEIVED                          40
#define STRING_PKT_SEND                                 41
#define STRING_TOTAL_PKTS_SEND                          42
#define STRING_AVERAGE_TIME_TO_SEND                     43
#define STRING_CLIENT_INSTRUCTION                       44
#define STRING_IP_NOT_VALID                             45
#define STRING_SELECT_LANG                              46
#define STRING_LANG_SELECTED                            47
#define STRING_NO_LANG_RECOGNIZED                       48
#define STRING_SHUTDOWN                                 49
#define STRING_CLOSE_CONNECTION                         50
#define STRING_CLIENT_STARTED                           51
#define STRING_LIST_GENERATED                           52
#define STRING_CLIENT_OP                                53
#define STRING_PROBLEM_FIRST_AV_PORT                    54
#define STRING_SERVER_BUSY_ERR                          55
#define STRING_PKT_NOT_FOUND_IN_WINDOW                  56
#define STRING_NO_HELP_FILE_FOUND                       57
#define STRING_CANNOT_OPEN_HELP_FILE                    58
#define STRING_UPLOAD_SPEED                             59
#define STRING_DOWNLOAD_SPEED                           60


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Enumeration for languages */
enum languages {
    LANG_EN,    //English
    LANG_IT     //Italian
};


/*  This function allows you to choose a language among those present.
 *  The language will be set at runtime. This will make it possible to change 
 *  language during program execution.
 *
 *  IMPORTANT:
 *  This function MUST BE the first operation performed by the program.
 *
 *  Parameters:
 *  - lang_id:      One of the IDs of the languages ​​present in the enumeration
 *                  'languages'
 *
 *  Return:         Nothing
 *
 *  EFFECTS:
 *  If the language ID exists, this will be set as the primary language. 
 *  If the language does not exist, it is set to the English language by default.
 */
void select_language(int lang_id);


/*  This function is used to retrieve a string (no matter what language is) from
 *  the global array 'STRINGS'. For this function is often used its abbreviation
 *  '_(str)' defined at the beginning of this file.
 *
 *  Parameters:
 *  - string_id:    The string ID identified by one of the definitions at the
 *                  beginning of the file
 *
 *  Return:         The retrieved string
 */
char *get_text(int string_id);


/*  This function returns the total number of the strings
 *
 *  Parameters:     Nothing
 *
 *  Return:         Number of strings (integer)
 *
 */
int get_total_strings();


/*  This function returns the ID of the language setted
 *
 *  Parameters:     Nothing
 *
 *  Return:         Language ID (integer)
 */
int get_lang_type();

#endif /* defined(__Reliable_UDP__strings__) */
