//
//  list.c
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

#include "list.h"

void print_list() {
    FILE *fp;
    char string[1024];
    
    //Open the file
    fp = fopen(LIST_FILE, "r");
    if (fp == NULL) {
        perror("fopen() in print_list()");
        exit(EXIT_FAILURE);
    }
    
    printf("\n_________________LIST_________________\n");//
    //Write 1024 chars until feof is reached
    while(fgets(string, 1024, fp) != NULL)
        printf("%s", string);
    
    printf("______________________________________\n\n");
    //Close file
    if (fclose(fp) != 0) {
        perror("fclose() in print_list()");
        exit(EXIT_FAILURE);
    }
}


char *create_list() {
    DIR *dirp = NULL;
    FILE *fp = NULL;
    //Create a unique filename based on local time
    time_t t = time(NULL);
    char *temp_name = convert_llint(t);
    char *new_name = malloc((strlen(temp_name) + 8) * sizeof(char));
    if (new_name == NULL) {
        perror("malloc() in create_list()");
        exit(EXIT_FAILURE);
    }
    //The file is saved in 'temp/' directory
    if (snprintf(new_name, (strlen(temp_name)  + 8) * sizeof(char), "temp/%s", temp_name) < 0) {
        perror("snprintf() in create_list()");
        exit(EXIT_FAILURE);
    }
    
    struct dirent *dp;
    dirp = opendir(DATA_DIR);  //Open directory
    
    fp = fopen(new_name, "w"); //Open file in write mode
    if (fp == NULL) {
        perror("fopen() in create_list()");
        exit(EXIT_FAILURE);
    }
    
    fprintf(fp, "%s %s %s\n", _(STRING_LIST_GENERATED), get_current_data(), get_current_time());
    
    int i = 1;
    //Read all files in directory
    while (dirp) {
        if ((dp = readdir(dirp)) != NULL) {
            //Do not include hidden files (like '.DS_STORE')
            if (strncmp(dp->d_name, ".", 1) == 0);
            else {
                fprintf(fp, "%3d) %33s\n", i, dp->d_name);
                ++i;
            }
        }
        else {
            closedir(dirp);//Close directory
            dirp = NULL;
        }
    }
    
    if (fclose(fp) != 0) {//Close file
        perror("fclose() in create_list()");
        exit(EXIT_FAILURE);
    }
    
    return new_name;
}

void remove_list(char *filename) {
    FILE *fp = NULL;
    //Adjust filename with full path
    char *new_name = malloc((strlen(filename) + strlen(DATA_DIR) + 1) * sizeof(char));
    if (new_name == NULL) {
        perror("malloc() in remove_list()");
        exit(EXIT_FAILURE);
    }
    
    if (snprintf(new_name, (strlen(filename) + strlen(DATA_DIR) + 1) * sizeof(char), "%s/%s", DATA_DIR, filename) < 0) {
        perror("snprintf() in remove_list()");
        exit(EXIT_FAILURE);
    }
    
    fp = fopen(new_name, "r");
    
    if (fp != NULL) {
        if (fclose(fp) != 0) {
            perror("fclose() in remove_list()");
            exit(EXIT_FAILURE);
        }
        if (remove(filename) != 0) {//Delete file
            perror("remove() in remove_list()");
            exit(EXIT_FAILURE);
        }
    }
}