/*
 * queue.h
 *
 *  Created on: Jul 16, 2023
 *      Author: AKhal
 */

#ifndef MSHR_H_
#define MSHR_H_


#include "memory_structures.h"

typedef struct {

	bank_req_t *miss_request;
	uint32_t   block_id;
	bool       valid;

}mshr_entry;



typedef struct{

	mshr_entry* mshr_entries;
	uint32_t size;
	uint32_t capacity;

}MSHR;

//This function is for initializing the miss status holding register
void MSHR_init(MSHR *pmshr, uint32_t size);

//This function determines whether the miss reservations is empty or not
bool MSHR_empty(const MSHR *pmshr);

//This function determines whether the miss reservations is full or not
bool MSHR_full(const MSHR *pmshr);


// This function is used to allocate new request
//in the available MSHR entries
int allocate(MSHR *pmshr , bank_req_t *bank_req, uint32_t block_id);


//This function is used to look up a request in the MSHR queue by its tag and id of the set where the memory address belong to
int lookup(MSHR *pmshr , bank_req_t *bank_req);


//This function is used to keep track of which entries are ready to be dequeued
mshr_entry *replay(MSHR *pmshr, uint32_t id);

//This function is used to remove requests from MSHR , that there data is arrived
//from the main memory
bool is_ready_pop(MSHR *pmshr);

bank_req_t * mshr_dequeue(MSHR *pmshr);

void MSHR_clear(MSHR *pmshr);



#endif /* MSHR_H_ */
