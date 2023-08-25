/*
 * memory_structures.h
 *
 *  Created on: Jul 16, 2023
 *      Author: AKhal
 */

#ifndef MEMORY_STRUCTURES_H_
#define MEMORY_STRUCTURES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "bit_operations.h"
#include "queue.h"

#define CACHE_SIZE       16384
#define BLOCK_SIZE       64   //byte
#define WORD_SIZE        4    //byte
#define Associativity    2
#define ADDR_WIDTH       32   //32-bits
#define NUM_BANKS        4
#define NUM_PORTS        1
#define NUM_INPUTS       4
#define WRITE_THROUGH    1
#define WRITE_RESPONSE   1
#define VICTIM_SIZE      0
#define MSHR_SIZE        8
#define LATENCY          4
#define CREQ_SIZE        2
#define CRSQ_SIZE        2
#define MREQ_SIZE        4
#define MRSQ_SIZE        1
#define WORD_PER_LINE    16



/*
 * Structure that contains parameters that describe the cache
 */

typedef struct{

	uint8_t 	cache_size;  		//log2 of the cache size.
	uint8_t 	block_size;         //log2 of the block size.
	uint8_t 	word_size;          //log2 of the word size.
	uint8_t 	associ_level;       //Log2 of the cache associativity
	uint8_t 	addr_width;     //This field represents the number of bits used for word addressing in the cache.
	uint8_t 	banks_num;          //The number of banks in the cache.
	uint8_t 	ports_per_bank;     //The number of ports per bank in the cache.
	uint8_t 	inputs_num;         //The number of inputs to the cache.
	bool    	write_through;      //A boolean flag indicating whether the cache is write-through (when true) or write-back (when false).
	bool    	write_reponse;      //A boolean flag indicating whether write response is enabled or not.
	uint16_t	mshr_size;          //Size of miss status holding registers
	uint8_t     latency;            //The pipeline latency of the cache.

}Cache_Config;

/*
 * Structure that contains some definition parameters
 */

typedef struct{


    //uint32_t sets_per_bank;
    //uint32_t blocks_per_set;
    uint32_t words_per_block;
    uint32_t log2_num_inputs;
    uint32_t word_addr_width;
    uint32_t mem_addr_width;
    uint32_t line_addr_width;
    uint32_t line_select_bits;
    uint32_t word_select_bits;

    uint32_t word_select_addr_start;
    uint32_t word_select_addr_end;

    uint32_t bank_select_addr_start;
    uint32_t bank_select_addr_end;

    uint32_t line_select_addr_start;
    uint32_t line_select_addr_end;

    uint32_t tag_select_addr_start;
    uint32_t tag_select_addr_end;


}Params_def;

typedef struct {

	bool 		valid;
	uint32_t    req_id;
	uint64_t    req_tag;

}bank_req_info_t;

typedef struct{

	bool   	 valid;
	bool   	 write;
	bool   	 mshr_replay;
	bool     is_flush;
	uint64_t addr;
	uint64_t *tag;
	uint32_t *req_data;
	uint32_t set_id;
	uint8_t  *pmask;
	uint8_t  *wsel;
	uint8_t  *byteen;
	uint8_t  *tid;

}bank_req_t;




void Init_Config(Cache_Config *pConfig);
void Init_params(Params_def *params , Cache_Config *pConfig);
uint32_t addr_bank_id(Params_def *params , uint64_t word_addr);
uint32_t addr_line_id(Params_def *params , uint64_t word_addr);
uint64_t addr_tag(Params_def *params , uint64_t word_addr);
uint32_t get_line_addr(Params_def *params , uint64_t word_addr);
uint32_t get_line_tag(Params_def *params , uint64_t word_addr);
uint32_t get_blk_address(Params_def *params , uint64_t word_addr);
uint32_t get_word_sel(Params_def *params , uint64_t word_addr);
uint64_t mem_addr(Params_def *params , uint32_t bank_id, uint32_t set_id, uint64_t tag);
void Init_bank_req(bank_req_t *bank_req);
void Free_bank_req(bank_req_t *bank_req);

//void VX_tag_access(int req_id , int stall , int lookup , uint32_t addr , bool fill , bool flush , bool tag_match);
//void VX_data_access(bool stall , bool read , bool fill , bool write , uint32_t addr , uint8_t *wsel , uint8_t *pmask , uint8_t *byteen , uint64_t *fill_data , uint64_t *write_data );



#endif /* MEMORY_STRUCTURES_H_ */
