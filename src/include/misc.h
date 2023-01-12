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

struct padded_str_s create_padded_str(char*, char*);
void write_delta_script(char*, struct proc_commands, uint);

/*
uint compute_cost(struct command* commands, int num_of_commands);
void print_command(struct command c);
void print_info();
*/

#endif