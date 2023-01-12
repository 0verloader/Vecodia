#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>
#include <sys/types.h>
#include <math.h>

#define ALPHA 2
#define BETA 5
#define MAX_LENGTH (pow(2,13)-1) /* 2^13 -1 13 bits used to encode the length of the encoded segment*/

struct command_init {
	uint16_t opcode:3;
	uint16_t length:13;
};

struct st_optArrays{
	uint* optA;
    uint* optC;
    uint* optR;
	uint* forw_add;
};

struct pullback_point {
	uint k;
	uint m;
	uint length;
	uint8_t type;
};

struct command { 
	unsigned int opcode ; // 0:ADD, 1:COPY0, 2: COPY1, 3:COPY2, 4:COPY3, 5+ DUMMY
	unsigned int length;
	unsigned int index_in_new;
	unsigned int m;
	unsigned int k;
	uint8_t* payload;
}; 

struct suffix { 
    int index; 
    int rank[2]; 
};

struct proc_commands {
	struct command* final_commands;
	uint num_of_commands;
};

struct limits {
	uint lim1,lim2,lim3,lim4,lim5;

};

struct padded_str_s { 
    uint8_t* padded_str; 
    uint8_t* new_f; 
    uint padded_length;
    uint new_length;
	struct limits lims;
}; 

#endif
