#include "../include/commands_computation.h"
#include "../include/structs.h"
#include <math.h>
#define PENALTY 999999999


extern inline
int min(int a, int b) {
    return a < b ? a : b;
}


struct st_optArrays computeOptArraysExecutables(struct limits lims, uint* SA, uint* RA, uint* LCP, uint8_t* T, uint lenNew, int in_place){

    uint k, m, val1, val2, val_t, size, type, offset;
    struct pullback_point ss;

    uint* opt  = (uint*)malloc(sizeof(uint)*lenNew);
    uint* optA = (uint*)malloc(sizeof(uint)*lenNew);
    uint* optC = (uint*)malloc(sizeof(uint)*lenNew);
    uint* optR = (uint*)malloc(sizeof(uint)*lenNew);
    uint* forw_add  = (uint*)malloc(sizeof(uint)*lenNew);

	optA[0] = ALPHA + 1;
	optC[0] = PENALTY;
    optR[0] = PENALTY;
	opt[0] = ALPHA + 1;
	forw_add[0] = 0;

    for(uint i=1; i<lenNew; i++){
        if (in_place)
            ss = finderInPlace(i, SA, RA, LCP, lims);
        else
            ss = finderOutOfPlace(i, SA, RA, LCP, lims);

        k = ss.k;
        m = ss.m;
        size = ss.length;
        type = ss.type;
        
        if(size >= MAX_LENGTH){
            offset = size - MAX_LENGTH + 1; 
            size = MAX_LENGTH-1;
            k += offset;
            if(type==0 || type==2)
                m += offset;
            else
                m -= offset;
        }

        if (k >= i){
            optC[i] = PENALTY;
            optR[i] = PENALTY;
        }
        else{
        	if (k == 0){
                if((!in_place && abs(m - k) < 256 && type==0) || (in_place && abs(m - k) < 256 && (type==0 || type==2)))
            	   optC[i] = 3;
                else
                   optC[i] = BETA;

                if(m == k && type == 0)
                    optR[i] = ALPHA;
                else
                    optR[i] = PENALTY;
            }
        	else{
                if((!in_place && abs(m - k) < 256 && type==0) || (in_place && abs(m - k) < 256 && (type==0 || type==2)))
                   optC[i] = opt[k-1] + 3;
                else
                   optC[i] = opt[k-1] + BETA;

                if(m == k && type == 0)
                    optR[i] = opt[k-1] + ALPHA;
                else
                    optR[i] = PENALTY;
            }
        }

        val_t = min(optC[i-1], optR[i-1]);

		val1 = optA[i-1] + 1;
		val2 = val_t + ALPHA + 1;


		forw_add[i] = val1 <= val2 ? 1:0;

		optA[i] = min(val1,val2);

        if (optA[i] <= optC[i] && optA[i] <= optR[i])
            opt[i] = optA[i];
        else if (optC[i] <= optA[i] && optC[i] <= optR[i])
            opt[i] = optC[i];
        else
            opt[i] = optR[i];
	}
    free(opt);
   	struct st_optArrays ret;
    ret.optA = optA;
    ret.optR = optR;
   	ret.optC = optC;
   	ret.forw_add = forw_add;
    return ret;
}


struct proc_commands computeOptCommandsExecutables(struct limits lims, uint* SA,
                        uint* RA, uint* LCP, uint8_t* T, uint lenNew, uint* optA, uint* optC,
                        uint* optR, uint* forw_add, uint8_t* newStr, int in_place){

    uint k ,m, size, type, offset, val1, val2, index_in_new;
    struct command command_i;
    struct pullback_point ss;
    struct command tmp_command;


    struct command* commands = (struct command*)malloc(sizeof(struct command)*lenNew);
    uint num_of_commands = 0;

    int i = lenNew - 1;
    uint z;
    while(i >= 0){

		if(optC[i] < optA[i] && optC[i] < optR[i]){

            if (in_place)
                ss = finderInPlace(i, SA, RA, LCP, lims);
            else
                ss = finderOutOfPlace(i, SA, RA, LCP, lims);

			k = ss.k;
			m = ss.m;
			size = ss.length;
			type = ss.type;
            
            if(size >= MAX_LENGTH){
                offset = size - MAX_LENGTH + 1; 
                size = MAX_LENGTH-1;
                k += offset;
                if(type==0 || type==2)
                    m += offset;
                else
                    m -= offset;
            }

            if((!in_place && abs(m - k) < 256 && type==0) || (in_place && abs(m - k) < 256 && (type==0 || type==2))){

                if(m<k){
                    command_i.opcode = 6;
                    command_i.length = size;
                    command_i.m = m;
                    command_i.k = k;
                }else{

                    command_i.opcode = 7;
                    command_i.length = size;
                    command_i.m = m;
                    command_i.k = k;
                }
            }
            else{

                command_i.opcode = type + 1;
                command_i.length = size;
                command_i.m = m;
                command_i.k = k;
            }

    		commands[num_of_commands] = command_i;
    		i = k - 1;
    	}
    	else if(optA[i] <= optC[i] && optA[i] <= optR[i]){

    		z = forw_add[i];

    		if(z == 0){
	            command_i.opcode = 0;
	            command_i.index_in_new = i;
	    		command_i.length = 1;
				command_i.payload = (uint8_t*)malloc(1);
				memcpy(command_i.payload, newStr+command_i.index_in_new, 1);

				i = i - 1;
    		}
    		else{

	    		int n_i = i-1;
	    		int k_ = 1;

	    		while(z == 1 && n_i >= 0 &&  (i-n_i < MAX_LENGTH) ){
		    		z = forw_add[n_i];
		            n_i --;
		            k_++;
	    		}

	            command_i.opcode = 0;
	            command_i.index_in_new = n_i+1;
	            command_i.length = k_;

				command_i.payload = (uint8_t*)malloc(k_);
				memcpy(command_i.payload, newStr+command_i.index_in_new, k_);

				i = n_i;
    		}

    		commands[num_of_commands] = command_i;
    	}
        else{
            if (in_place)
                ss = finderInPlace(i, SA, RA, LCP, lims);
            else
                ss = finderOutOfPlace(i, SA, RA, LCP, lims);
            
            k = ss.k;
            m = ss.m;
            size = ss.length;
            type = ss.type;

            if(size >= MAX_LENGTH){
                offset = size - MAX_LENGTH + 1; 
                size = MAX_LENGTH-1;
                k += offset;
                m += offset;
            }

            command_i.opcode = 5;
            command_i.length = size;


            commands[num_of_commands] = command_i;
            i = k - 1;

        }
    	num_of_commands++;   
	}

    /* Reverse the array of commands */
    for(uint i=0; i<num_of_commands/2;i++){
        tmp_command = commands[i];
        commands[i] = commands[num_of_commands -1 -i];
        commands[num_of_commands -1 -i] = tmp_command;
    }

    struct proc_commands ret;
    ret.final_commands = commands;
    ret.num_of_commands = num_of_commands;

    free(optA);
    free(optC);
    free(forw_add);
    return ret;
}


struct proc_commands computeCommands(struct limits lims, uint* SA, uint* RA, uint* LCP,
    uint8_t* T, uint lenNew, uint8_t* new_payload, int in_place){
	struct st_optArrays opt_arr = computeOptArraysExecutables(lims, SA, RA, LCP, T, lenNew, in_place);
    struct proc_commands final_commands = computeOptCommandsExecutables(lims, SA, RA, LCP,
                                 T, lenNew, opt_arr.optA, opt_arr.optC, opt_arr.optR,
                                opt_arr.forw_add, new_payload, in_place);
    return final_commands;
}