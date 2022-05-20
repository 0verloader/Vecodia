#include "../include/suffix_array.h"
#include "../include/divsufsort.h"

int max(int a, int b){ /*it ws originally inline */
    return a>b ? a : b;
}

uint *buildSuffixArray(uint8_t* sequence, uint seq_length){ 
    uint *suffix_array = (uint *)malloc(seq_length * sizeof(uint));
    divsufsort(sequence, suffix_array, seq_length);
    return suffix_array;
} 


uint *buildRankArray(uint* suffix_array, uint seq_length){
    uint *rank_array = (uint*)malloc(seq_length * sizeof(uint));
    for (uint i = 0; i < seq_length; i++){
        rank_array[suffix_array[i]] = i;
    }
    return rank_array;
}

/* Based on Kasai's trick linear LCP computation */
uint* buildLCPArray(uint* suffix_array, uint* rank_array, uint8_t* sequence, uint seq_length){
    uint *LCP_array = (uint*)malloc(seq_length * sizeof(uint));
    int k = 0;
    int j; 
  	
    for (uint i=0; i<seq_length; i++) 
    { 
        if (rank_array[i] == 0){
        	LCP_array[rank_array[i]] = 0;
        }
        else{
	        j = suffix_array[rank_array[i]-1]; 
	        while (i+k<seq_length && j+k<seq_length && sequence[i+k]==sequence[j+k]) 
                k++; 
	  
	        LCP_array[rank_array[i]] = k;
	        // Deleting the starting character from the string. 
	        k = max(k-1, 0);
   	 	}
    } 
    return LCP_array; 
}