#ifndef BIT_OPERATIONS_H_
#define BIT_OPERATIONS_H_

#include <stdint.h>
#include <assert.h>

int custom_clz(uint32_t x);
uint32_t log2ceil(uint32_t x);
uint64_t get_bit_field(uint64_t bits, uint32_t start, uint32_t end);
uint64_t set_bit_field(uint64_t bits, uint32_t start, uint32_t end, uint64_t value);
uint64_t clear_bit_field(uint64_t bits, uint32_t start, uint32_t end);

#endif /* BIT_OPERATIONS_H_ */
