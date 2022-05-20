#ifndef MISC_H
#define MISC_H

#include "./structs.h"
#include <getopt.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

/*    
+------13------+--3--+
|    Length    | Opc |
+--------------+-----+
*/




struct padded_str_s create_padded_str(char* new_f, char* old_f);

/*
uint compute_cost(struct command* commands, int num_of_commands);
void print_command(struct command c);
void write_delta_script(char* fname, struct proc_commands cc, uint len_new);
void print_info();
*/

#endif