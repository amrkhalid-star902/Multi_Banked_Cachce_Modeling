
#include "Bank.h"



void Init_Bank(Bank *bank){

    //Intializing different parameters of the bank struct
    bank->CoreRequests = (Queue*)calloc(CREQ_SIZE, sizeof(Queue));
    bank->CoreResponse = (Queue*)calloc(CRSQ_SIZE, sizeof(Queue));
    bank->MemRequest = (Queue*)calloc(MREQ_SIZE, sizeof(Queue));
    bank->MemReponse = (Queue*)calloc(MRSQ_SIZE, sizeof(Queue));
    bank->config = (Cache_Config*)malloc(sizeof(Cache_Config));
    bank->params = (Params_def*)malloc(sizeof(Params_def));

    if ( bank->CoreRequests == NULL || bank->CoreResponse == NULL
        || bank->MemRequest == NULL || bank->MemReponse == NULL || bank->config == NULL
        || bank->params == NULL) {
        free(bank->CoreRequests);
        free(bank->CoreResponse);
        free(bank->MemRequest);
        free(bank->MemReponse);
        free(bank->config);
        free(bank->params);
        return;
    }

    init_queue(bank->CoreRequests , CREQ_SIZE);
    init_queue(bank->CoreResponse , CRSQ_SIZE);
    init_queue(bank->MemRequest , MREQ_SIZE);
    init_queue(bank->MemReponse, MRSQ_SIZE);

    Init_Config(bank->config);
    Init_params(bank->params , bank->config);
    MSHR_init(&bank->mshr, bank->config->mshr_size);

    bank->flush_enable  = 0;
    bank->flush_address = 0;
}




void Schedule(Bank *bank, uint8_t RAM_SP[64][64], uint64_t TAG_RAM[64] , bank_req_t *bank_req) {

    //This function it is used to determine which type of requests will be handled.
    //The type of requests are as follow: CoreRequest , MemoryFillRequest , MSHR request , flush request.
    //The order of the priority is as follow:
    //1.Flush Request
    //2.MSHR Request
    //3.MemoryFill Request
    //4.Core Request

    if (bank->flush_enable) {

        //Flush request detected
        bank_req->is_flush = true;
        bank_req->addr = bank->flush_address;
        ProcessBankRequest(bank, bank_req, RAM_SP, TAG_RAM);
        printf("flush , address %lx\n" , bank->flush_address);

    } else {


        //If the core response queue is full , no incoming requests will be handled.
        if (is_full(bank->CoreResponse))
            return;

        //Check if there is a pending mshr request
        uint8_t ready = is_ready_pop(&bank->mshr);

        if (!ready) {

            //Check if there is a memory fill request
            if (!is_empty(bank->MemReponse)) {
                Memory_Response* mem_rsp = front(bank->MemReponse);
                if (mem_rsp->mem_rsp_valid) {

                    //If there is a valid memory response , the response will be forwarded to the bank to be handled
                    MemoryFill(bank, mem_rsp, RAM_SP, TAG_RAM);

                }
                dequeue(bank->MemReponse);
                printf("after-memory-fill\n");
				return;

            }


            //In order to the core request to be handled the following conditions must be meeted:
            //Check that there is a pending core request inside CoreRequests queue.
            //Check that the miss reservation stations is not full.
            //Check that the CoreResponse queue is not full.
            //Check that the MemoryRequests queue is not full.
            if (!is_empty(bank->CoreRequests) && !MSHR_full(&bank->mshr) && !is_full(bank->CoreResponse) && !is_full(bank->MemRequest)) {
                Core_Request* core_req = front(bank->CoreRequests);

                uint32_t set_id = 0;
                bank_req->valid = core_req->core_req_valid;
                bank_req->write = core_req->core_req_rw;
                bank_req->mshr_replay = false;
                bank_req->is_flush = false;
                bank_req->addr = core_req->core_req_addr;
                bank_req->byteen = core_req->core_req_byteen;
                bank_req->pmask = core_req->core_req_pmask;
                bank_req->wsel = core_req->core_req_wsel;
                bank_req->tag = core_req->core_req_tag;
                bank_req->req_data = core_req->core_req_data;
                bank_req->set_id = set_id;
                bank_req->tid = core_req->core_req_tid;


                //printf("core-request rw=%x address=%lx pmask=%x tag=%llx tid=%x\n" ,core_req->core_req_rw,core_req->core_req_addr,core_req->core_req_pmask[0],core_req->core_req_tag[0],core_req->core_req_tid[0]);


                ProcessBankRequest(bank, bank_req, RAM_SP, TAG_RAM);
                dequeue(bank->CoreRequests);


            } else {

                return;
            }
        } else {

            //Pending mshr requests are detected.
            //MSHR replay request is sent to be handled by the bank.
            bank_req_t* mshr_req = mshr_dequeue(&bank->mshr);
            //printf("pop-mshr address=%lx tag=%llx pmask=%x tid=%x\n" ,mshr_req->addr,mshr_req->tag[0],mshr_req->pmask[0],mshr_req->tid[0]);

            ProcessBankRequest(bank, mshr_req, RAM_SP, TAG_RAM);

        }

    }

}


void ProcessBankRequest(Bank* bank , bank_req_t *req , uint8_t RAM_SP[64][64]  ,  uint64_t TAG_RAM[64]){



    if(req->is_flush){

        //Flush Request is detected.
        //The address required to be flushed will be sended to the tag access uint.
        tag_access  tag_request;
        tag_request.addr   = req->addr;
        tag_request.fill   = false;
        tag_request.flush  = true;
        tag_request.lookup = false;
        tag_request.req_id = req->tag[0];
        tag_request.tag_match = false;

        Tag_store_access(&tag_request , TAG_RAM);

    }

    if(req->mshr_replay){

        //Mshr replay request is detected.
        //Response will be with required information.
        //printf("core-read-rsp address=%lx tag=%llx pmask=%x tid=%x data=%lx wsel=%x\n" ,req->addr,req->tag[0],req->pmask[0],req->tid[0],req->req_data[req->wsel[0]] , req->wsel[0]);
        Core_Response *core_rsp = (Core_Response*)calloc(1 , sizeof(core_rsp));
        core_rsp->core_rsp_valid = true;
        core_rsp->core_rsp_pmask = req->pmask;
        core_rsp->core_rsp_tag   = req->tag;
        core_rsp->core_rsp_tid   = req->tid;
        core_rsp->core_rsp_data  = req->req_data;

        enqueue(bank->CoreResponse, core_rsp);

		free(core_rsp);

    }else{

        //Core request is detected and will be handled
        if(!req->valid)
            return;

        tag_access tag_request1;
        tag_request1.addr  = req->addr;
        tag_request1.fill   = false;
        tag_request1.flush  = false;
        tag_request1.lookup = false;
        tag_request1.req_id = req->tag[0];
        tag_request1.tag_match = false;


        //The request will be checked , to find whether it produces hit or miss in the cache memory.
        Tag_store_access(&tag_request1 , TAG_RAM);
        uint8_t hit = tag_request1.tag_match;


        if(hit){

            printf("hit detected\n");

            //If the hit_request is read request the data will be fetched from the cache data store.
            //If the hit_request is write request the data will be written to the cache data store ,
            //then later it will be written to the lower memory levels according to the cache writing policy,
            //whether it is write through cache or write back cache.
            data_access_argumentsType  hit_data_access;
            hit_data_access.addr   = req->addr;
            hit_data_access.byteen = req->byteen;
            hit_data_access.fill   = false;
            hit_data_access.pmask  = req->pmask;
            hit_data_access.read   = !req->write;
            hit_data_access.write  = req->write;
            hit_data_access.write_data = req->req_data;
            hit_data_access.wsel   = req->wsel;


            Data_access(bank , &hit_data_access , RAM_SP);


            if(req->write && bank->config->write_through){

                //If the cache is write through cache then the lower level memory
                //will be updated with the new data written to the cache as soon as possiable.
                //If the cache is writeback the lower level memory will be updated when a miss request occur.
                Mem_Request  mem_req;
                mem_req.mem_req_addr = req->addr;
                mem_req.mem_req_byteen = req->byteen;
                mem_req.mem_req_data   = req->req_data;
                mem_req.mem_req_pmask  = req->pmask;
                mem_req.mem_req_rw     = true;
                mem_req.mem_req_valid  = true;
                //printf("mem_req\n");
                printf("memory data hit = %lx\n" , mem_req.mem_req_data[0]);
                enqueue(bank->MemRequest, &mem_req);

                if(bank->config->write_reponse){

                    //If the cache is write response cache then a response will be send back to the requesting core.

                    Core_Response  core_rsp;
                    core_rsp.core_rsp_valid = false;
                    core_rsp.core_rsp_pmask = req->pmask;
                    core_rsp.core_rsp_tag   = req->tag;
                    core_rsp.core_rsp_tid   = req->tid;
                    //printf("write-back addr=%lx data=%lx byteen=%x\n",req->addr,req->req_data[0],req->byteen[0]);
                    enqueue(bank->CoreResponse, &core_rsp);

                }


            }else{

                //The hit request is read request
                //A response with fetched data from the cache will be sent to the requesting core
                //printf("core-read-rsp address=%lx tag=%llx pmask=%x tid=%x data=%lx wsel=%x\n" ,req->addr,req->tag[0],req->pmask[0],req->tid[0],hit_data_access.read_data[0],req->wsel[0]);
                Core_Response  core_rsp;
                core_rsp.core_rsp_valid = true;
                core_rsp.core_rsp_data  = hit_data_access.read_data;
                core_rsp.core_rsp_pmask = req->pmask;
                core_rsp.core_rsp_tag   = req->tag;
                core_rsp.core_rsp_tid   = req->tid;
                enqueue(bank->CoreResponse, &core_rsp);

            }



        }else{

            //A miss is occured
            if(req->write){
                printf("write missed data\n");
                //Write miss request occur
                //The request data is written to the cache and the lower level memory
                tag_access  write_miss_tag;
                write_miss_tag.addr  = req->addr;
                write_miss_tag.fill   = true;
                write_miss_tag.flush  = false;
                write_miss_tag.lookup = false;
                write_miss_tag.req_id = req->tag[0];

                Tag_store_access(&write_miss_tag , TAG_RAM);

                data_access_argumentsType  write_data_access;
                write_data_access.addr   = req->addr;
                write_data_access.byteen = req->byteen;
                write_data_access.fill   = false;
                write_data_access.pmask  = req->pmask;
                write_data_access.read   = false;
                write_data_access.write  = true;
                write_data_access.write_data = req->req_data;
                write_data_access.wsel   = req->wsel;

                Data_access(bank , &write_data_access , RAM_SP);


                //Send memory request to write the data to the lower level memory
                //Mem_Request *write_mem_req = (Mem_Request*)calloc(1 , sizeof(Mem_Request));
                Mem_Request write_mem_req;
                write_mem_req.mem_req_addr = req->addr;
                write_mem_req.mem_req_byteen = req->byteen;
                write_mem_req.mem_req_data   = req->req_data;
                write_mem_req.mem_req_pmask  = req->pmask;
                write_mem_req.mem_req_rw     = true;
                write_mem_req.mem_req_valid  = true;
                printf("memory data = %lx\n" , write_mem_req.mem_req_data[0]);
                enqueue(bank->MemRequest, &write_mem_req);


                if(bank->config->write_reponse){

                    //Send a response to requesting tool if the cache is write response
                    Core_Response write_core_rsp;
                    write_core_rsp.core_rsp_valid = false;
                    write_core_rsp.core_rsp_pmask = req->pmask;
                    write_core_rsp.core_rsp_tag   = req->tag;
                    write_core_rsp.core_rsp_tid   = req->tid;
                    //printf("write-back addr=%lx data=%lx byteen=%x\n",req->addr,req->req_data[0],req->byteen[0]);
                    enqueue(bank->CoreResponse, &write_core_rsp);


                }






            }else{

                //Read Miss , MSHR lookup is going to be done
            	printf("handling miss request\n");
                int pending = lookup(&bank->mshr, req);

                //If there is no pending entries in the mshr which has the same address as the request allocate new entry.
                if(pending == -1){

                    uint32_t block_id = get_blk_address(bank->params, req->addr);
                    int mshr_id = allocate(&bank->mshr, req , block_id);

                    //A fill request will be send to the main memory with tag equal to the allocated
                    //mshr entry so , the request can be easily tracked and identified
                    Mem_Request read_mem_req;
                    read_mem_req.mem_req_addr = req->addr;
                    read_mem_req.mem_req_byteen = req->byteen;
                    read_mem_req.mem_req_id     = mshr_id;
                    read_mem_req.mem_req_pmask  = req->pmask;
                    read_mem_req.mem_req_rw     = false;
                    read_mem_req.mem_req_valid  = true;

                    enqueue(bank->MemRequest, &read_mem_req);



                }
            }

        }
    }


}

void MemoryFill(Bank *bank , Memory_Response *mem_rsp , uint8_t RAM_SP[64][64] ,  uint64_t TAG_RAM[64]){

    //Use the memory response to determine the mshr entry to be replayed
	mshr_entry *entry = replay(&bank->mshr, mem_rsp->mem_rsp_id);

	entry->miss_request->req_data = mem_rsp->mem_rsp_data;

    //printf("start-tag-access\n");

    //Fill the tag store with the request tag
    tag_access tag_request;
	tag_request.addr  = entry->miss_request->addr;
	tag_request.fill   = true;
	tag_request.flush  = false;
	tag_request.lookup = false;
	tag_request.req_id = entry->miss_request->tag[0];
    //printf("fill-rsp address=%lx , id=%lx\n" , entry->miss_request->addr , mem_rsp->mem_rsp_id);

	Tag_store_access(&tag_request , TAG_RAM);

    //Fill the data store with data from the main memory
    data_access_argumentsType  data_access;
	data_access.addr   = entry->miss_request->addr;
	data_access.byteen = entry->miss_request->byteen;
	data_access.fill   = true;
	data_access.pmask  = entry->miss_request->pmask;
	data_access.read   = false;
	data_access.write  = false;
	data_access.fill_data = mem_rsp->mem_rsp_data;
	data_access.wsel   = entry->miss_request->wsel;



	Data_access(bank , &data_access , RAM_SP);


}


void Data_access(Bank *bank , data_access_argumentsType *access_req, uint8_t RAM_SP[64][64]){

    uint32_t rdata[WORD_PER_LINE];
    uint32_t wdata[WORD_PER_LINE];
    uint8_t  wren[BLOCK_SIZE];


    if (WORD_PER_LINE > 1) {

        printf("Word bigger 1\n");
        uint32_t wdata_r[WORD_PER_LINE];
        uint8_t wren_r[WORD_PER_LINE];

        if (NUM_PORTS > 1) {

            if (!access_req->read && !access_req->fill) {

                for (int i = 0; i < NUM_PORTS; i++) {

                    if (access_req->pmask[i]) {

                        uint8_t word_num = access_req->wsel[i];
                        wdata_r[word_num] = access_req->write_data[i];
                        wren_r[word_num] = access_req->byteen[i];
                    }
                }
            }


        } else {

            if (!access_req->read && !access_req->fill) {

                for (int j = 0; j < WORD_PER_LINE; j++) {

                    wdata_r[j] = access_req->write_data[0];
                }

                wren_r[access_req->wsel[0]] = access_req->byteen[0];
            }
        }

        if (!access_req->read) {

            for (int j = 0; j < WORD_PER_LINE; j++) {

                wdata[j] = access_req->write ? wdata_r[j] : access_req->fill_data[j];
            }

            for (int j = 0; j < WORD_PER_LINE; j++) {

                uint8_t byteenable = wren_r[j];
                int idx = 4*j;
                for(int k = idx ; k < idx + 4 ; k++){

                    if(access_req->write){
                        uint8_t bit = byteenable & 1;
                        if(bit){
                            wren[k] = 1;
                        }else{
                            wren[k] = 0;
                        }

                        byteenable >>= 1;
                    }else{

                        wren[k] = access_req->fill;
                    }

                }



            }

        }
    }

    uint8_t addr = get_blk_address(bank->params, access_req->addr);
    printf("line address=%x\n" , addr);

    if (access_req->write || access_req->fill) {

        for (int i = 0; i < WORD_PER_LINE; i++){

            int idx = i * 4;
            uint32_t val = wdata[i];
            for(int j = idx ; j < idx + 4 ; j++){

                uint8_t write_byte = val & 0xFF;
                if(wren[j]){

                    RAM_SP[addr][j] = write_byte;
                }

                val >>= 8;
            }
        }


    } else {

        for (int i = 0; i < WORD_PER_LINE; i++) {

            uint8_t idx = i * 4;
            uint8_t byte1 = RAM_SP[addr][idx];
            uint8_t byte2 = RAM_SP[addr][idx + 1];
            uint8_t byte3 = RAM_SP[addr][idx + 2];
            uint8_t byte4 = RAM_SP[addr][idx + 3];

            rdata[i] = ((uint32_t)byte4 << 24) | ((uint32_t)byte3 << 16) | ((uint32_t)byte2 << 8) | (uint32_t)byte1;


        }


        access_req->read_data = (uint32_t *)calloc(1 , sizeof(uint32_t));
        for(int i = 0 ; i < NUM_PORTS ; i++){


            uint32_t word = rdata[access_req->wsel[i]];
            access_req->read_data[i] = word;

        }



    }


}



void Tag_store_access(tag_access *tag_req , uint64_t TAG_RAM[64]){

    Cache_Config config;
	Init_Config(&config);

    Params_def params;
	Init_params(&params, &config);

	uint8_t  line_addr = get_blk_address(&params, tag_req->addr);
	uint64_t line_tag  = get_line_tag(&params, tag_req->addr);

	if(tag_req->flush){

		TAG_RAM[line_addr] = 0;

	}else if(tag_req->fill){

		TAG_RAM[line_addr] = line_tag;

	}else{

		uint64_t tag = TAG_RAM[line_addr];

		if(tag == line_tag){

			printf("tag matched\n");
			tag_req->tag_match = true;

		}

	}


}


void core_req_bank_sel(Core_requests_out_infos *output_infos , Core_requests_info *requests_info){

    uint32_t core_req_line_addr[NUM_INPUTS];
    uint8_t  core_req_wsel[NUM_INPUTS];
    uint8_t  core_req_bid[NUM_INPUTS];

    Cache_Config config;
    Init_Config(&config);
    Params_def params;
    Init_params(&params , &config);


    for(int i = 0 ; i < NUM_INPUTS ; i++){

        core_req_line_addr[i] = get_line_addr(&params , requests_info->core_req_addr[i]);
        core_req_wsel[i]      = get_word_sel(&params , requests_info->core_req_addr[i]);

    }

    for(int i = 0 ; i < NUM_INPUTS ; i++){

        if(NUM_BANKS > 1){

            core_req_bid[i] = addr_bank_id(&params ,requests_info->core_req_addr[i]);
        }else{

            core_req_bid[i] = 0;
        }
    }

    if(NUM_INPUTS > 1){

        if(NUM_PORTS > 1){

            uint32_t per_bank_line_addr_r[NUM_INPUTS];
            uint8_t per_bank_rw_r[NUM_INPUTS];
            uint8_t core_req_line_match[NUM_INPUTS];

            for(int i = NUM_INPUTS - 1 ; i >= 0 ; i--){

                if(requests_info->core_req_valid[i]){

                    per_bank_line_addr_r[core_req_bid[i]] = core_req_line_addr[i];
                    per_bank_rw_r[core_req_bid[i]] = requests_info->core_req_rw[i];

                }
            }

            for(int i = 0 ; i < NUM_INPUTS ; i++){

                core_req_line_match[i] = (core_req_line_addr[i] == per_bank_line_addr_r[core_req_bid[i]])
                                      && (requests_info->core_req_rw[i] == per_bank_rw_r[core_req_bid[i]]);

            }

            if(NUM_PORTS < NUM_INPUTS){

                //uint8_t req_select_table_r[NUM_BANKS][NUM_PORTS];

                for(int i = NUM_INPUTS - 1 ; i >= 0 ; i--){

                    if(requests_info->core_req_valid[i]){

                        output_infos->per_bank_valid[core_req_bid[i]]                          = 1;
                        output_infos->per_bank_core_req_pmask[core_req_bid[i]][i % NUM_PORTS]  = core_req_line_match[i];
                        output_infos->per_bank_core_req_wsel[core_req_bid[i]][i % NUM_PORTS]   = core_req_wsel[i];
                        output_infos->per_bank_core_req_byteen[core_req_bid[i]][i % NUM_PORTS] = requests_info->core_req_byteen[i];
                        output_infos->per_bank_core_req_rw[core_req_bid[i]]                    = requests_info->core_req_rw[i];
                        if(requests_info->core_req_rw[i]){

                            output_infos->per_bank_core_req_data[core_req_bid[i]][i % NUM_PORTS] = requests_info->core_req_data[i];
                        }

                        output_infos->per_bank_core_req_tid[core_req_bid[i]][i % NUM_PORTS]     = i;
                        output_infos->per_bank_core_req_tag[core_req_bid[i]][i % NUM_PORTS]     = requests_info->core_req_tag[i];
                        output_infos->per_bank_core_req_addr[core_req_bid[i]]                   = core_req_line_addr[i];

                    }

                }

            }
        }else{

            for(int i = NUM_INPUTS - 1 ; i >= 0 ; i--){

                    if(requests_info->core_req_valid[i]){

                        output_infos->per_bank_valid[core_req_bid[i]]              = 1;
                        output_infos->per_bank_core_req_pmask[core_req_bid[i]][0]  = 1;
                        output_infos->per_bank_core_req_wsel[core_req_bid[i]][0]   = core_req_wsel[i];
                        output_infos->per_bank_core_req_byteen[core_req_bid[i]][0] = requests_info->core_req_byteen[i];
                        output_infos->per_bank_core_req_rw[core_req_bid[i]]        = requests_info->core_req_rw[i];
                        if(requests_info->core_req_rw[i]){

                            output_infos->per_bank_core_req_data[core_req_bid[i]][0] = requests_info->core_req_data[i];
                        }

                        output_infos->per_bank_core_req_tid[core_req_bid[i]][0]     = i;
                        output_infos->per_bank_core_req_tag[core_req_bid[i]][0]     = requests_info->core_req_tag[i];
                        output_infos->per_bank_core_req_addr[core_req_bid[i]]       = core_req_line_addr[i];

                    }

            }


        }
    }else{

        if(NUM_BANKS > 1){

            output_infos->per_bank_valid[core_req_bid[0]]   = requests_info->core_req_valid[0];
            output_infos->per_bank_core_req_rw[core_req_bid[0]]  = requests_info->core_req_rw[0];
            output_infos->per_bank_core_req_addr[core_req_bid[0]] = core_req_line_addr[0];
            output_infos->per_bank_core_req_wsel[core_req_bid[0]][0] = core_req_wsel[0];
            output_infos->per_bank_core_req_byteen[core_req_bid[0]][0] = requests_info->core_req_byteen[0];
            output_infos->per_bank_core_req_data[core_req_bid[0]][0]  = requests_info->core_req_data[0];
            output_infos->per_bank_core_req_tag[core_req_bid[0]][0]   = requests_info->core_req_tag[0];
            output_infos->per_bank_core_req_tid[core_req_bid[0]][0]   = 0;
            output_infos->per_bank_core_req_pmask[core_req_bid[0]][0] = 1;

        }else{

            output_infos->per_bank_valid[0]   = requests_info->core_req_valid[0];
            output_infos->per_bank_core_req_rw[0]  = requests_info->core_req_rw[0];
            output_infos->per_bank_core_req_addr[0] = core_req_line_addr[0];
            output_infos->per_bank_core_req_wsel[0][0] = core_req_wsel[0];
            output_infos->per_bank_core_req_byteen[0][0] = requests_info->core_req_byteen[0];
            output_infos->per_bank_core_req_data[0][0]  = requests_info->core_req_data[0];
            output_infos->per_bank_core_req_tag[0][0]   = requests_info->core_req_tag[0];
            output_infos->per_bank_core_req_tid[0][0]   = 0;
            output_infos->per_bank_core_req_pmask[0][0] = 1;

        }
    }

}



void initialize_RAM_SP_random(uint8_t RAM_SP[64][64]) {
    srand(time(NULL)); // seed the random number generator with the current time

    for(int i = 0; i < 64; i++) {
        for(int j = 0; j < 64; j++) {
            RAM_SP[i][j] = rand() % 256; // generate a random number between 0 and 255 inclusive
        }
    }
}

void initialize_TAG_SP_random(uint64_t TAG_RAM[64]){

	srand(time(NULL)); // seed the random number generator with the current time

	for(int i = 0 ; i < 64 ; i++){

		TAG_RAM[i] = rand()%256;
	}
}

