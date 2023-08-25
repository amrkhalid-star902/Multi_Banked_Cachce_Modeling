
#include "mshr.h"

void MSHR_init(MSHR *pmshr, uint32_t size){

	pmshr->mshr_entries = (mshr_entry*)calloc(size  , sizeof(mshr_entry));
	for(uint32_t i = 0 ; i < size ; i++){

		mshr_entry *entry   = &pmshr->mshr_entries[i];
		entry->miss_request = (bank_req_t*)calloc(1 , sizeof(bank_req_t));
		Init_bank_req(entry->miss_request);

	}

	pmshr->size = 0;
	pmshr->capacity = size;


}

bool MSHR_empty(const MSHR *pmshr){

	return pmshr->size == 0;
}


bool MSHR_full(const MSHR *pmshr){

	return pmshr->size == pmshr->capacity;
}


int allocate(MSHR *pmshr , bank_req_t *bank_req, uint32_t block_id){

	for(uint32_t i = 0 ; i < pmshr->capacity ; i++){

		mshr_entry *entry = &pmshr->mshr_entries[i];
		if(!entry->valid){

			entry->miss_request              = bank_req;
			entry->miss_request->mshr_replay = false;
			entry->block_id = block_id;
			entry->valid    = true;
			pmshr->size++;
			return i;

		}
	}

	return -1;
}



int lookup(MSHR *pmshr , bank_req_t *bank_req){

	for(uint32_t i = 0 ; i < pmshr->capacity ; i++){

		mshr_entry *entry = &pmshr->mshr_entries[i];
		if(entry->valid
		   && entry->miss_request->addr == bank_req->addr){

			return i;

		}

	}

	return -1;

}


mshr_entry *replay(MSHR *pmshr, uint32_t id){

	//printf("start-replay\n");
	//mshr_entry *main_entry = (mshr_entry*)calloc(1 , sizeof(mshr_entry));

    mshr_entry *main_entry = &pmshr->mshr_entries[id];
	assert(main_entry->valid);
	//printf("main-entry\n");

	for(uint32_t i = 0 ; i < pmshr->capacity ; i++){

		mshr_entry *entry = &pmshr->mshr_entries[i];
		if(entry->valid
		   && entry->miss_request->set_id == main_entry->miss_request->set_id
		   && entry->miss_request->tag == main_entry->miss_request->tag){

			entry->miss_request->mshr_replay = true;
		}

	}

	//printf("end-replay\n");
	return main_entry;
}

bool is_ready_pop(MSHR *pmshr){

	for(uint32_t i = 0 ; i < pmshr->capacity ; i++){

		mshr_entry *entry = &pmshr->mshr_entries[i];

		if(entry->valid && entry->miss_request->mshr_replay){
			printf("valid replay\n");
			return true;

		}
	}
	printf("not valid replay\n");
	return false;
}

bank_req_t * mshr_dequeue(MSHR *pmshr){

	for(uint32_t i = 0 ; i < pmshr->capacity ; i++){

		mshr_entry *entry = &pmshr->mshr_entries[i];


		if(entry->valid && entry->miss_request->mshr_replay){

			bank_req_t *req = entry->miss_request;
			entry->valid = false;
			pmshr->size--;
			return req;

		}
	}
	return NULL;


}


void MSHR_clear(MSHR *pmshr){

	for(uint32_t i = 0 ; i < pmshr->size ; i++){

		mshr_entry *entry = &pmshr->mshr_entries[i];
		if(entry->miss_request->valid && entry->miss_request->mshr_replay){

			entry->miss_request->valid = false;

		}
	}

	pmshr->size = 0;
}
