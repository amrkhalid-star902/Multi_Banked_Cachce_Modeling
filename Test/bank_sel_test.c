#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "mshr.h"
#include "Bank.h"



int main(){


	Core_requests_info requests_info = {

	  .core_req_rw = (uint8_t[4]){1 , 1 , 1 , 1},
	  .core_req_byteen = (uint8_t[4]){0x1 , 0x7 , 0xe , 0xf},
	  .core_req_data   = (uint32_t[4]){0x12340000 , 0x80001234 , 0x00 , 0x23455611},
	  .core_req_addr   = (uint64_t[4]){0x80000100 , 0x80000110 , 0x8000020 , 0x80000730},
	  .core_req_tag    = (uint64_t[4]){0x212 , 0x814 , 0x111 , 0x321},
	  .core_req_valid  = (uint8_t[4]){1 , 1 , 1 , 1}


	};



	Core_requests_out_infos core_requests_out;

	core_req_bank_sel(&core_requests_out, &requests_info);


    int i = 0;

    uint64_t TAG_RAM[64];
    initialize_TAG_SP_random(TAG_RAM);


    uint8_t RAM_SP[64][64];
    initialize_RAM_SP_random(RAM_SP);

    Mem_Request     *mem_req  = (Mem_Request*)calloc(1 , sizeof(Mem_Request));
    Core_Response   *core_rsp = (Core_Response*)calloc(1 , sizeof(Core_Response));

    bank_req_t bank_req;
    Init_bank_req(&bank_req);

    Bank bank;
	Init_Bank(&bank);

    while (i < 4)
    {
        

        
        Core_Request core_req = {

            .core_req_valid  = core_requests_out.per_bank_valid[i],
            .core_req_pmask  = (uint8_t[1]){core_requests_out.per_bank_core_req_pmask[i][0]},
            .core_req_wsel   = (uint8_t[1]){ core_requests_out.per_bank_core_req_wsel[i][0]},
            .core_req_byteen = (uint8_t[1]){core_requests_out.per_bank_core_req_byteen[i][0]},
            .core_req_tid    = (uint8_t[1]){ core_requests_out.per_bank_core_req_tid[i][0]},
            .core_req_data   = (uint32_t[1]){core_requests_out.per_bank_core_req_data[i][0]},
            .core_req_tag    = (uint64_t[1]){core_requests_out.per_bank_core_req_tag[i][0]},
            .core_req_rw     = core_requests_out.per_bank_core_req_rw[i],
            .core_req_addr   = core_requests_out.per_bank_core_req_addr[i]

        };


        enqueue(bank.CoreRequests, &core_req);
        Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);

        mem_req = front(bank.MemRequest);

        /*printf("address is %lx\n",mem_req->mem_req_addr);
        printf("address is %lx\n",write_back_addr[idx]);
        printf("mem-data=%lx\n" , mem_req->mem_req_data[0]);
        printf("write-back-data=%lx\n" , write_back_data[idx]);*/



        uint32_t memory_address = mem_req->mem_req_addr;
        uint32_t write_back_address = core_requests_out.per_bank_core_req_addr[i];
        uint32_t data1 = mem_req->mem_req_data[0];
        uint32_t data2 =  core_requests_out.per_bank_core_req_data[i][0];
        uint8_t byteen1 = mem_req->mem_req_byteen[0] ;
        uint8_t byteen2 = core_requests_out.per_bank_core_req_byteen[i][0];
        printf("address is %lx\n",memory_address);
        printf("address is %lx\n",write_back_address);
        printf("mem-data=%lx\n" , data1);
        printf("write-back-data=%lx\n" , data2);
        printf("byteen1=%u\n" , byteen1);
        printf("byteen2=%u\n" , byteen2);


        bool case1 = memory_address == write_back_address;
        bool case2 = data1 == data2;
        
        bool case3 = byteen1 ==byteen2;
        printf("case3\n");

        dequeue(bank.CoreResponse);
        dequeue(bank.MemRequest);


        if(case1 && case2 && case3){

            printf("test case %d is passed\n" , i);
        }else{

            printf("test case %d is failed\n" , i);
            printf("test case 1 = %x\n" , case1);
            printf("test case 2 = %x\n" , case2);
            printf("test case 3 = %x\n" , case3);
            //break;
        }


        i++;

    }
    



}