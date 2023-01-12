#include "../include/finder.h"

inline
int min(int a, int b){
    return a < b ? a : b;
}

uint get_lcp(uint i, uint j, uint* lcp_arr){
	uint min_value;
	if (i<=j){
		min_value = lcp_arr[i+1];
		for(uint k = i+1; k <= j; k++)
			min_value = min(min_value, lcp_arr[k]);
	}
	else{
		min_value = lcp_arr[j+1];
		for(uint k = j+1; k <= i; k++)
			min_value = min(min_value, lcp_arr[k]);
	}
	return min_value;
}

struct pullback_point finderOutOfPlace(uint index_unpadded, uint* sa, uint* ra, uint* ha, struct limits lims){

	uint tmp_len, m, k, z, length, m_padded;
	uint prev_tmp_len = 0;
	uint8_t type;
	int offset;
	uint attempts = 0;

	uint index = lims.lim5 - index_unpadded - 1;
	
	offset = 1;
	uint L_index = -1;
	uint L_type = -1;
	uint L_length = 0;

	while(1){

		if (((int)ra[index] - offset) < (int)lims.lim1 || attempts >= 3)
			break;

		tmp_len = get_lcp(ra[index]-offset, ra[index], ha);

		if(tmp_len == prev_tmp_len){
			attempts++;
		}
		else{
			attempts = 1;
		}

		if (tmp_len == 0)
			break;

		prev_tmp_len = tmp_len;

		m = sa[ra[index] - offset] + tmp_len - 1;
		k = index + tmp_len - 1;
		z = lims.lim4 - (k - lims.lim4 + 1);

		if ((sa[ra[index]-offset] >= lims.lim1) && (m < lims.lim2)){
			L_index = sa[ra[index]-offset];
			L_length = tmp_len;
			L_type = 0;
			break;
		}
		else 
		if((sa[ra[index]-offset] >= lims.lim2) && (m < lims.lim3)){
			L_index = sa[ra[index]-offset];
			L_length = tmp_len;
			L_type = 1;
			break;
		}
		else if(k < sa[ra[index]-offset]){
			L_index = sa[ra[index]-offset];
			L_length = tmp_len;
			L_type = 2;
			break;
		}
		else if((sa[ra[index]-offset] >= lims.lim3) && (m < z)){
			L_index = sa[ra[index]-offset];
			L_length = tmp_len;
			L_type = 3;
			break;
		}

		offset++;
	}

	offset = 1;
	prev_tmp_len = 0;
	attempts = 0;
	uint R_index = -1;
	uint R_type = -1;
	uint R_length = 0;

	while(1){
		if (ra[index] + offset > lims.lim5-1 || attempts >= 3)
			break;

		tmp_len = get_lcp(ra[index] + offset, ra[index], ha);

		if(tmp_len == prev_tmp_len){
			attempts++;
		}
		else{
			attempts = 1;
		}

		if (tmp_len == 0)
			break;
		prev_tmp_len = tmp_len;



		if (tmp_len == 0)
			break;

		m = sa[ra[index] + offset] + tmp_len - 1;
		k = index + tmp_len - 1;
		z = lims.lim4 - (k - lims.lim4 + 1);

		if ((sa[ra[index]+offset] >= lims.lim1) && (m < lims.lim2)){
			R_index = sa[ra[index]+offset];
			R_length = tmp_len;
			R_type = 0;
			break;
		}
		else if((sa[ra[index]+offset] >= lims.lim2) && (m < lims.lim3)){
			R_index = sa[ra[index]+offset];
			R_length = tmp_len;
			R_type = 1;
			break;
		}
		else if(k < sa[ra[index]+offset]){
			R_index = sa[ra[index]+offset];
			R_length = tmp_len;
			R_type = 2;
			break;
		}
		else if((sa[ra[index]+offset] >= lims.lim3) && (m < z)){
			R_index = sa[ra[index]+offset];
			R_length = tmp_len;
			R_type = 3;
			break;
		}
		offset++;
	}

	if (L_length>=R_length){
		length = L_length;
		m_padded = L_index;
		type = L_type;
	}
	else{
		length = R_length;
		m_padded = R_index;
		type = R_type;
	}


    struct pullback_point ret_pb;

    if(length == 0){
    	ret_pb.k = index_unpadded + 1;
    	ret_pb.m = 0;
    	ret_pb.length = 0;
    	ret_pb.type = (uint8_t)0;
    }
    else{

    	k = index_unpadded - length + 1;

    	switch(type){
    		case 0: m = lims.lim2 - (m_padded + length); 	break;
    		case 1: m = m_padded + (length-1) - lims.lim2;  break;
    		case 2: m = lims.lim5 - (m_padded + length); 	break;
    		case 3: m = m_padded + (length-1) - lims.lim3;  break;
    	}

		ret_pb.k = index_unpadded - length + 1;
		ret_pb.m = m;
		ret_pb.length = length;
		ret_pb.type = (uint8_t)type;
    }
    return ret_pb;
}

struct pullback_point finderInPlace(uint index_unpadded, uint* sa, uint* ra, uint* ha, struct limits lims){

	uint tmp_len, m, k, z, length, m_padded;
	uint prev_tmp_len = 0;
	uint8_t type;
	int offset;
	uint attempts = 0;

	uint index = lims.lim5 - index_unpadded - 1;
	
	offset = 1;
	uint L_index = -1;
	uint L_type = -1;
	uint L_length = 0;

	while(1){

		if (((int)ra[index] - offset) < (int)lims.lim1 || attempts >= 3)
			break;

		tmp_len = get_lcp(ra[index]-offset, ra[index], ha);

		if(tmp_len == prev_tmp_len){
			attempts++;
		}
		else{
			attempts = 1;
		}

		if (tmp_len == 0)
			break;

		prev_tmp_len = tmp_len;

		m = sa[ra[index] - offset] + tmp_len - 1;
		k = index + tmp_len - 1;
		z = lims.lim4 - (k - lims.lim4 + 1);

		if ((sa[ra[index]-offset] >= lims.lim1) && (m < lims.lim2) && ((lims.lim2 - m) >= (lims.lim5 - k))){
			L_index = sa[ra[index]-offset];
			L_length = tmp_len;
			L_type = 0;
			break;
		}
		else 
		if((sa[ra[index]-offset] >= lims.lim2) && (m < lims.lim3) && ((sa[ra[index]-offset] - lims.lim2) >=  index_unpadded)){
			L_index = sa[ra[index]-offset];
			L_length = tmp_len;
			L_type = 1;
			break;
		}
		else if(k < sa[ra[index]-offset]){
			L_index = sa[ra[index]-offset];
			L_length = tmp_len;
			L_type = 2;
			break;
		}
		else if((sa[ra[index]-offset] >= lims.lim3) && (m < z)){
			L_index = sa[ra[index]-offset];
			L_length = tmp_len;
			L_type = 3;
			break;
		}

		offset++;
	}

	offset = 1;
	prev_tmp_len = 0;
	attempts = 0;
	uint R_index = -1;
	uint R_type = -1;
	uint R_length = 0;

	while(1){
		if (ra[index] + offset > lims.lim5-1 || attempts >= 3)
			break;

		tmp_len = get_lcp(ra[index] + offset, ra[index], ha);

		if(tmp_len == prev_tmp_len){
			attempts++;
		}
		else{
			attempts = 1;
		}

		if (tmp_len == 0)
			break;
		prev_tmp_len = tmp_len;



		if (tmp_len == 0)
			break;

		m = sa[ra[index] + offset] + tmp_len - 1;
		k = index + tmp_len - 1;
		z = lims.lim4 - (k - lims.lim4 + 1);

		if ((sa[ra[index]+offset] >= lims.lim1) && (m < lims.lim2) && ((lims.lim2 - m) >= (lims.lim5 - k))){
			R_index = sa[ra[index]+offset];
			R_length = tmp_len;
			R_type = 0;
			break;
		}
		else if((sa[ra[index]+offset] >= lims.lim2) && (m < lims.lim3) && ((sa[ra[index]+offset] - lims.lim2) >=  index_unpadded)){
			R_index = sa[ra[index]+offset];
			R_length = tmp_len;
			R_type = 1;
			break;
		}
		else if(k < sa[ra[index]+offset]){
			R_index = sa[ra[index]+offset];
			R_length = tmp_len;
			R_type = 2;
			break;
		}
		else if((sa[ra[index]+offset] >= lims.lim3) && (m < z)){
			R_index = sa[ra[index]+offset];
			R_length = tmp_len;
			R_type = 3;
			break;
		}
		offset++;
	}

	if (L_length>=R_length){
		length = L_length;
		m_padded = L_index;
		type = L_type;
	}
	else{
		length = R_length;
		m_padded = R_index;
		type = R_type;
	}


    struct pullback_point ret_pb;

    if(length == 0){
    	ret_pb.k = index_unpadded + 1;
    	ret_pb.m = 0;
    	ret_pb.length = 0;
    	ret_pb.type = (uint8_t)0;
    }
    else{

    	k = index_unpadded - length + 1;

    	switch(type){
    		case 0: m = lims.lim2 - (m_padded + length); 	break;
    		case 1: m = m_padded + (length-1) - lims.lim2;  break;
    		case 2: m = lims.lim5 - (m_padded + length); 	break;
    		case 3: m = m_padded + (length-1) - lims.lim3;  break;
    	}

		ret_pb.k = index_unpadded - length + 1;
		ret_pb.m = m;
		ret_pb.length = length;
		ret_pb.type = (uint8_t)type;
    }
    return ret_pb;
}