

#include "bit_operations.h"




int custom_clz(uint32_t x){

	if(x <= 0){

		return -1;
	}

	int leading_zeros = 0;
	for(int i = 31 ; i >=0 ; i--){

		if((x & (1 << i)) != 0){

			break;
		}

		leading_zeros++;
	}

	return leading_zeros;
}

uint32_t log2ceil(uint32_t x){

	return 32 - custom_clz(x - 1);

}


uint64_t get_bit_field(uint64_t bits, uint32_t start, uint32_t end){

	assert(end >= start);
	assert(end <= 63);

	uint32_t shift = 63 - end;
	uint64_t result = (bits << shift) >> (shift + start);
	return result;

}
uint64_t clear_bit_field(uint64_t bits, uint32_t start, uint32_t end){

	assert(end >= start);
	assert(end <= 63);

	uint32_t shift = 63 - end;
    uint64_t mask = (0xffffffffffffffff << (shift + start)) >> shift;

    return bits & ~mask;


}

uint64_t set_bit_field(uint64_t bits, uint32_t start, uint32_t end, uint64_t value){

	assert(end >= start);
	assert(end <= 63);

	uint32_t shift = 63 - end;
	uint64_t field = (value << (shift + start)) >> shift;

	return clear_bit_field(bits, start, end) | field;
}

