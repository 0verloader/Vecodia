#include "../include/misc.h"
#include <math.h>

uint8_t* reverse(uint8_t * arr, uint n) 
{ 
	uint8_t * arr_res = (uint8_t *)malloc(sizeof(uint8_t) * n);
	uint8_t tmp;
    for (uint i=0; i<=n/2; i++){
    	tmp = arr[i];
    	arr_res[i] = arr[n -i -1];
    	arr_res[n -i -1] = tmp;
    }
    return arr_res;
} 

struct padded_str_s create_padded_str(char* new_f, char* old_f){

    uint l1,l2,l3,l4,l5;

    struct stat sb1,sb2;
    int fdNew = open(new_f, O_RDONLY);
    int fdOld = open(old_f, O_RDONLY);

    fstat(fdNew, &sb1); 
    fstat(fdOld, &sb2);

    uint8_t * memNew = (uint8_t*)mmap(NULL, sb1.st_size, PROT_READ, MAP_PRIVATE, fdNew, 0);
    uint8_t * memOld = (uint8_t*)mmap(NULL, sb2.st_size, PROT_READ, MAP_PRIVATE, fdOld, 0);
    
    close(fdNew); 
    close(fdOld); 

    l1 = 0;
    l2 = (uint)sb2.st_size;
    l3 = (uint)(2*sb2.st_size);
    l4 = (uint)(2*sb2.st_size + sb1.st_size);
    l5 = (uint)(2*sb2.st_size + 2*sb1.st_size);

    uint8_t* padded_str = (uint8_t*)malloc(sizeof(uint8_t)*l5);  // l5 is the final size of the padded-str.

    uint8_t* new_reverse = reverse(memNew, sb1.st_size);
    uint8_t* old_reverse = reverse(memOld, sb2.st_size);

    memcpy(padded_str + l1, old_reverse, sb2.st_size);
    memcpy(padded_str + l2, memOld, sb2.st_size);
    memcpy(padded_str + l3, memNew, sb1.st_size);
    memcpy(padded_str + l4, new_reverse, sb1.st_size);

    free(new_reverse);
    free(old_reverse);

    struct padded_str_s ret;

    ret.padded_str = padded_str;
    ret.padded_length = l5;
    ret.new_f = memNew;
    ret.new_length = (uint)sb1.st_size;
    ret.lims.lim1 = l1;
    ret.lims.lim2 = l2;
    ret.lims.lim3 = l3;
    ret.lims.lim4 = l4;
    ret.lims.lim5 = l5;
    munmap(memOld, sb2.st_size);
    /* Do not munmap memNew because it is used later */
    return ret;
}


uint compute_cost(struct command* commands, int num_of_commands){
    int cost = 0;
    for(int i=0;i<num_of_commands;i++){
    	if(commands[i].opcode == 0 && commands[i].length == 1 && i!=num_of_commands-1)
    		continue;
        
        switch(commands[i].opcode){
            case 0: cost += ALPHA + commands[i].length;    break;
            case 1: cost += BETA;                          break;
            case 2: cost += BETA;                          break;
            case 3: cost += BETA;                          break;
            case 4: cost += BETA;                          break;
            case 5: cost += ALPHA;                         break;
            case 6: cost += 3;                             break;
            case 7: cost += 3;                             break;
            default:printf("ERROR [%d]\n",i);              break;
        }
    }
    return cost;
}


void print_command(struct command c){
    switch(c.opcode){
        case 0: printf("ADD %d\n", c.length);               break;
        case 1: printf("COPY0 %d %d\n",c.m, c.length);      break;
        case 2: printf("COPY1 %d %d\n",c.m, c.length);      break;
        case 3: printf("COPY2 %d %d\n",c.m, c.length);      break;
        case 4: printf("COPY3 %d %d\n",c.m, c.length);      break;
        case 5: printf("RUN %d\n", c.length);               break;
        case 6: printf("COPY REF- %d\n", c.length);         break;
        case 7: printf("COPY REF+ %d\n", c.length);         break;
        default:printf("ERROR\n");                          break;
    }
}


void write_delta_script(char* fname, struct proc_commands c_coms,uint len_new){

 	struct command* commands = c_coms.final_commands;
 	uint num_of_commands = c_coms.num_of_commands;
    struct command tmp;
    uint16_t cc ;
    FILE *fp;
    fp = fopen(fname,"wb");  // w for write, b for binary
    uint32_t len = len_new;


    fwrite(&len, 3, 1, fp);                                         /* FILE WRITE 3B */

    uint coms_singletons = 0;

    for(uint i=0; i<num_of_commands; i++){
        tmp = commands[i];
        if(tmp.opcode == 0 && tmp.length == 1 && i != (num_of_commands-1))
            coms_singletons +=1;
    }


    uint32_t valid_coms = ceil((num_of_commands - coms_singletons)/8.0) ;

    fwrite(&coms_singletons,3, 1, fp);                              /* FILE WRITE 2B */    
    fwrite(&valid_coms, 3, 1, fp);                                  /* FILE WRITE 2B */

    int iv = 0;
    int xv = 0;
    int ktt = 0;

    for(uint i=0; i<num_of_commands; i++){
        tmp = commands[i];
        if(tmp.opcode == 0 && tmp.length == 1 && i != (num_of_commands-1)){
            iv = iv ^ (1 << xv);
        }else{

            xv++;
        }

        if (xv == 8){
            fwrite(&iv, 1, 1, fp);
            ktt ++;
            xv = 0;
            iv = 0;
        }
    }

    if(xv != 0)
        fwrite(&iv, 1, 1, fp);

    for(uint i=0; i<num_of_commands; i++){
        tmp = commands[i];
        if(tmp.opcode == 0 && tmp.length == 1 && i != (num_of_commands-1)){
            uint8_t klm = *(tmp.payload);
            fwrite(&klm, 1, 1, fp);
        }
    }

    for(uint i=0; i<num_of_commands; i++){
        tmp = commands[i];

        if(tmp.opcode == 0 && tmp.length == 1 && i != (num_of_commands-1))
            continue;

        cc  = ( (tmp.length << 3)) ^ (tmp.opcode);
        fwrite(&cc, sizeof(uint16_t), 1, fp);

        if(tmp.opcode == 0)
            fwrite(tmp.payload, tmp.length, 1, fp);
        else if(tmp.opcode == 1 || tmp.opcode == 2 || tmp.opcode == 3 || tmp.opcode == 4){
        	uint32_t tmpp = tmp.m;
            fwrite(&tmpp, 3, 1, fp);
        }else if(tmp.opcode == 6){
            uint8_t tmpp = tmp.k - tmp.m;
            fwrite(&tmpp, 1, 1, fp);	
        }else if(tmp.opcode == 7){
            uint8_t tmpp = tmp.m - tmp.k;
            fwrite(&tmpp, 1, 1, fp);
        }
    }

    int t = 0;
    for(int i=0; i <60;i++){
        fwrite(&t, 4, 1, fp);
    }
    
    fclose(fp);


	printf("Delta  : %d\nUpdate : %d\n", 
		compute_cost(commands, num_of_commands) +
		coms_singletons + valid_coms + 9
		,
		len_new);

}


void print_info(){
    FILE *fp;
    char * line = NULL;
    ssize_t read;

    size_t len = 0;

    fp = fopen("INFO_VER", "r"); // read mode

    if (fp == NULL)
    {
      perror("Error reading INFO_VER file.\n");
      exit(1);
    }

    while ((read = getline(&line, &len, fp)) != -1) 
        printf(" %s", line);

    fclose(fp);
    return;
}