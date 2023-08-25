
#include "memory_structures.h"


void Init_Config(Cache_Config *pConfig){

	pConfig->cache_size 	= log2ceil(CACHE_SIZE);
	pConfig->block_size 	= log2ceil(BLOCK_SIZE);
	pConfig->word_size  	= log2ceil(WORD_SIZE);
	pConfig->addr_width = ADDR_WIDTH;
	pConfig->associ_level 	= log2ceil(Associativity);
	pConfig->banks_num      = NUM_BANKS;
	pConfig->ports_per_bank = NUM_PORTS;
	pConfig->inputs_num     = NUM_INPUTS;
	pConfig->write_through  = WRITE_THROUGH;
	pConfig->write_reponse  = WRITE_RESPONSE;
	pConfig->mshr_size      = MSHR_SIZE;
	pConfig->latency        = LATENCY;

}


void Init_params(Params_def *params , Cache_Config *pConfig){

	params->log2_num_inputs = log2ceil(pConfig->inputs_num);
	params->words_per_block = BLOCK_SIZE / WORD_SIZE;
	params->word_addr_width = 32 - log2ceil(WORD_SIZE);
	params->mem_addr_width  = 32 - log2ceil(BLOCK_SIZE);
	params->line_addr_width = params->mem_addr_width - log2ceil(NUM_BANKS);


	uint32_t bank_size       = CACHE_SIZE / NUM_BANKS;
	uint32_t lines_per_bank  = bank_size / BLOCK_SIZE;

	uint32_t word_select_bits = log2ceil(params->words_per_block);
	params->word_select_bits       = word_select_bits;
	params->word_select_addr_start = 0;
	params->word_select_addr_end   = (params->word_select_addr_start + word_select_bits - 1);

	uint32_t bank_select_bits       = log2ceil(NUM_BANKS);
	params->bank_select_addr_start  = (params->word_select_addr_end + 1);
	params->bank_select_addr_end    = (params->bank_select_addr_start + bank_select_bits - 1);

	uint32_t line_select_bits       = log2ceil(lines_per_bank);
	params->line_select_bits        = line_select_bits;
	params->line_select_addr_start  = (params->bank_select_addr_end + 1);
	params->line_select_addr_end    = (params->line_select_addr_start + line_select_bits - 1);

	//uint32_t tag_select_bits        = (params->word_addr_width - 1 - params->line_select_addr_end );
	params->tag_select_addr_start   = (params->line_select_addr_end + 1);
	params->tag_select_addr_end     = (params->word_addr_width - 1);
}


uint32_t addr_bank_id(Params_def *params , uint64_t word_addr){

	uint32_t start = params->bank_select_addr_start;
	uint32_t end   = params->bank_select_addr_end;

	if(end >= start){
		return (uint32_t)get_bit_field(word_addr , start , end);
	}else{
		return 0;
	}
}


uint32_t addr_line_id(Params_def *params , uint64_t word_addr){

	uint32_t start = params->line_select_addr_start;
	uint32_t end   = params->line_select_addr_end;

	if(end >= start){
		return (uint32_t)get_bit_field(word_addr , start , end);
	}else{
		return 0;
	}
}

uint64_t addr_tag(Params_def *params , uint64_t word_addr){

	uint32_t start = params->tag_select_addr_start;
	uint32_t end   = params->tag_select_addr_end;

	if(end >= start){
		return get_bit_field(word_addr , start , end);
	}else{
		return 0;
	}

}

uint32_t get_line_addr(Params_def *params , uint64_t word_addr){

	uint32_t start = params->line_select_addr_start;
	uint32_t end   = 31;

	if(end >= start){
		return get_bit_field(word_addr , start , end);
	}else{
		return 0;
	}
}


uint32_t get_line_tag(Params_def *params , uint64_t word_addr){

	uint32_t start = params->line_select_bits;
	//uint32_t end = 31;
	uint32_t end   = params->word_addr_width - 1;

	if(end >= start){
		return get_bit_field(word_addr , start , end);
	}else{
		return 0;
	}

}

uint32_t get_blk_address(Params_def *params , uint64_t word_addr){

	uint32_t start = 0;
	uint32_t end   = params->line_select_bits - 1;

	if(end >= start){
		return get_bit_field(word_addr , start , end);
	}else{
		return 0;
	}
}


uint32_t get_word_sel(Params_def *params , uint64_t word_addr){

	uint32_t start = 0;
	uint32_t end   = params->word_select_bits - 1;

	if(end >= start){
		return get_bit_field(word_addr , start , end);
	}else{
		return 0;
	}

}


uint64_t mem_addr(Params_def *params , uint32_t bank_id, uint32_t set_id, uint64_t tag){

	uint64_t address = 0;

	uint32_t start_bank = params->bank_select_addr_start;
	uint32_t end_bank   = params->bank_select_addr_end;

	if(end_bank >= start_bank){
		address = set_bit_field(address, start_bank, end_bank, bank_id);
	}

	uint32_t start_set = params->line_select_addr_start;
	uint32_t end_set   = params->line_select_addr_end;

	if(end_set >= start_set){
		address = set_bit_field(address, start_set, end_set, set_id);
	}

	uint32_t start_tag = params->tag_select_addr_start;
	uint32_t end_tag   = params->tag_select_addr_end;

	if(end_tag >= start_tag){
		address = set_bit_field(address, start_tag, end_tag, tag);
	}


	return address;

}


void Init_bank_req(bank_req_t *bank_req){
    bank_req->valid   		= false;
    bank_req->write   		= false;
    bank_req->mshr_replay   = false;
    bank_req->is_flush      = false;
    bank_req->pmask         = (uint8_t*)calloc(1, sizeof(uint8_t));
    bank_req->wsel          = (uint8_t*)calloc(1, sizeof(uint8_t));
    bank_req->byteen        = (uint8_t*)calloc(1, sizeof(uint8_t));
    bank_req->set_id        = 0;
    bank_req->req_data      = (uint32_t*)calloc(1, sizeof(uint32_t));
    bank_req->addr          = 0;
    bank_req->tag           = (uint64_t*)calloc(1, sizeof(uint64_t));
    bank_req->tid           = (uint8_t*)calloc(1, sizeof(uint8_t));

    if (bank_req->pmask == NULL || bank_req->wsel == NULL || bank_req->byteen == NULL
        || bank_req->req_data == NULL || bank_req->tag == NULL || bank_req->tid == NULL) {
        // Handle memory allocation error
        free(bank_req->pmask);
        free(bank_req->wsel);
        free(bank_req->byteen);
        free(bank_req->req_data);
        free(bank_req->tag);
        free(bank_req->tid);
        return;
    }
}


void Free_bank_req(bank_req_t *bank_req) {
    if (bank_req->tag != NULL) {
        free(bank_req->tag);
        bank_req->tag = NULL;
    }
    if (bank_req->req_data != NULL) {
        free(bank_req->req_data);
        bank_req->req_data = NULL;
    }
    if (bank_req->pmask != NULL) {
        free(bank_req->pmask);
        bank_req->pmask = NULL;
    }
    if (bank_req->wsel != NULL) {
        free(bank_req->wsel);
        bank_req->wsel = NULL;
    }
    if (bank_req->byteen != NULL) {
        free(bank_req->byteen);
        bank_req->byteen = NULL;
    }
    if (bank_req->tid != NULL) {
        free(bank_req->tid);
        bank_req->tid = NULL;
    }
}
