/*
 * Bank.h
 *
 *  Created on: Jul 16, 2023
 *      Author: AKhal
 */

#ifndef BANK_H_
#define BANK_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "memory_structures.h"
#include "mshr.h"
//#include "data_access.h"




typedef struct{

    bool 	 core_req_valid;
    uint8_t  *core_req_pmask;
    uint8_t  *core_req_wsel;
    uint8_t  *core_req_byteen;
    uint8_t  *core_req_tid;
    uint32_t *core_req_data;
    uint64_t *core_req_tag;
    bool     core_req_rw;
    uint64_t core_req_addr;

}Core_Request;


typedef struct{

	bool     core_rsp_valid;
	uint8_t  *core_rsp_pmask;
	uint8_t  *core_rsp_tid;
	uint32_t *core_rsp_data;
	uint64_t *core_rsp_tag;
	//bool     core_rsp_ready;

}Core_Response;




typedef struct{

	bool     mem_req_valid;
	bool     mem_req_rw;
	uint8_t  *mem_req_pmask;
	uint8_t  *mem_req_byteen;
	uint8_t  *mem_req_wsel;
	uint64_t mem_req_addr;
	uint32_t mem_req_id;
	uint32_t *mem_req_data;
	bool     mem_req_ready;


}Mem_Request;


typedef struct{

	bool      mem_rsp_valid;
	uint32_t  mem_rsp_id;
	uint32_t  *mem_rsp_data;
	bool      mem_rsp_ready;

}Memory_Response;


typedef struct{


    uint64_t req_id; /*req_id has a size of DBG_CACHE_REQ_IDW*/

    bool read;

    bool fill;

    bool write;

    uint64_t  addr; /*Maximum size of addr is LINE_ADDR_WIDTH (32 bits)*/

    uint8_t *wsel; /*[NUM_PORTS]*/ /*Aray of wires of size NUM_PORTS with each wire having a width of WIRE_SELECT_BITS*/

    uint8_t * pmask; /*[NUM_PORTS]*/ /*Array of wires of size NUM_PORTS with each wire having a width of 1*/

    uint8_t *byteen; /*[NUM_PORTS]*/ /*Array of wires of size NUM_PORTS with each wire having a width of WORD_SIZE*/

    uint32_t *fill_data;/*[WORDS_PER_LINE]*/ /*Array of wires of size WORDS_PER_LINE with each wire having a width of WORD_WIDTH*/

    uint32_t *write_data; /*[NUM_PORTS]*/ /*Array of wires of size NUM_PORTS with each wire having a width of WORD_WIDTH*/

    uint32_t *read_data;

}data_access_argumentsType;


typedef struct{

	uint32_t req_id;
	uint32_t addr;
	bool lookup;
	bool fill;
	bool flush;
	bool tag_match;

}tag_access;


/*
* This structure holds infos about the different requests
* comming from different core and upon these infos the requests
* are assigned to certain banks
*/

typedef struct{

	uint8_t  *core_req_valid;
	uint8_t  *core_req_rw;
	uint64_t *core_req_addr;
	uint8_t  *core_req_byteen;
	uint32_t *core_req_data;
	uint64_t *core_req_tag;

}Core_requests_info;

typedef struct{

	uint8_t  per_bank_valid[NUM_INPUTS];
	uint8_t  per_bank_core_req_pmask[NUM_INPUTS][NUM_PORTS];
	uint8_t  per_bank_core_req_rw[NUM_INPUTS];
	uint64_t per_bank_core_req_addr[NUM_INPUTS];
	uint8_t  per_bank_core_req_wsel[NUM_BANKS][NUM_PORTS];
	uint8_t  per_bank_core_req_byteen[NUM_BANKS][NUM_PORTS];
	uint32_t per_bank_core_req_data[NUM_BANKS][NUM_PORTS];
	uint8_t  per_bank_core_req_tid[NUM_BANKS][NUM_PORTS];
	uint64_t per_bank_core_req_tag[NUM_BANKS][NUM_PORTS];


}Core_requests_out_infos;



typedef struct{


	MSHR  mshr;
	Queue *CoreRequests;
	Queue *CoreResponse;
	Queue *MemRequest;
	Queue *MemReponse;
	Cache_Config *config;
	Params_def   *params;
	bool          flush_enable;
	uint32_t      flush_address;

}Bank;



//This function is used to initialize the cache bank
void Init_Bank(Bank *bank);
//This function is used to determine the source from which the bank will handle the request
void Schedule(Bank *bank, uint8_t RAM_SP[64][64], uint64_t TAG_RAM[64] , bank_req_t *bank_req);
//This function is used to handle the incoming requests after defining its source
void ProcessBankRequest(Bank* bank , bank_req_t *req , uint8_t RAM_SP[64][64]  ,  uint64_t TAG_RAM[64]);
//This function is used to fill the cache with the missed data that comes from the main memory
void MemoryFill(Bank *bank , Memory_Response *mem_rsp , uint8_t RAM_SP[64][64] ,  uint64_t TAG_RAM[64]);

void Data_access(Bank *bank , data_access_argumentsType *access_req , uint8_t RAM_SP[64][64]);
void Tag_store_access(tag_access *tag_req , uint64_t TAG_RAM[64]);

//This function is used to assign incoming requests to the different banks
void core_req_bank_sel(Core_requests_out_infos *output_infos , Core_requests_info *requests_info);

void initialize_RAM_SP_random(uint8_t RAM_SP[64][64]);
void initialize_TAG_SP_random(uint64_t TAG_RAM[64]);


#endif /* BANK_H_ */
