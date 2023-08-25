#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "mshr.h"
#include "Bank.h"



int main(){
    
    
    uint64_t TAG_RAM[64];
    initialize_TAG_SP_random(TAG_RAM);


    uint8_t RAM_SP[64][64];
    initialize_RAM_SP_random(RAM_SP);

    Memory_Response *mem_rsp = (Memory_Response*)calloc(1 , sizeof(Memory_Response));

    bank_req_t bank_req;
    Init_bank_req(&bank_req);

    Core_Request core_req = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){0x01},
        .core_req_wsel   = (uint8_t[1]){0x08},
        .core_req_byteen = (uint8_t[1]){0x0F},
        .core_req_tid    = (uint8_t[1]){0x00},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){0x0},
        .core_req_rw     = false,
        .core_req_addr   = 0x80000000

    };

    Bank bank;
	Init_Bank(&bank);

    enqueue(bank.CoreRequests, &core_req);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);

    Mem_Request *mem_req = front(bank.MemRequest);


	if(!mem_req->mem_req_rw){

		mem_rsp->mem_rsp_valid   = true;
		mem_rsp->mem_rsp_id      = mem_req->mem_req_id;
		mem_rsp->mem_rsp_data    = (uint32_t[16]){0xf1450513,0x00002517,0x0005006b,0x00100513,0x128000ef,0x00b5106b,0x13458593,0x00000597,0xfc102573,0x0005006b,0x00100513,0x0f8000ef,0x00b5106b,0x10458593,0x00000597,0xfc102573};
		mem_rsp->mem_rsp_ready   = true;

	}


	dequeue(bank.MemRequest);
	enqueue(bank.MemReponse, mem_rsp);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
    printf("affter-schedule\n");
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);

    Core_Response *core_rsp = front(bank.CoreResponse);

    for (int i = 0; i < 16; i++)
    {
        printf("filldata=%lx\n" , core_rsp->core_rsp_data[i]);
    }


	dequeue(bank.CoreResponse);



    Core_Request core_req1 = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){0x01},
        .core_req_wsel   = (uint8_t[1]){0x07},
        .core_req_byteen = (uint8_t[1]){0x0F},
        .core_req_tid    = (uint8_t[1]){0x00},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){0x04},
        .core_req_rw     = false,
        .core_req_addr   = 0x80000000

    };

    enqueue(bank.CoreRequests, &core_req1);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
    core_rsp = front(bank.CoreResponse);
	dequeue(bank.CoreResponse);
    printf("responce data=%lx\n" , core_rsp->core_rsp_data[0]);




    Core_Request core_req2 = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){0x01},
        .core_req_wsel   = (uint8_t[1]){0x0D},
        .core_req_byteen = (uint8_t[1]){0x0F},
        .core_req_tid    = (uint8_t[1]){0x00},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){0x08},
        .core_req_rw     = false,
        .core_req_addr   = 0x80000000

    };

    enqueue(bank.CoreRequests, &core_req2);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
    core_rsp = front(bank.CoreResponse);
	dequeue(bank.CoreResponse);
    printf("responce data=%lx\n" , core_rsp->core_rsp_data[0]);


    Core_Request core_req3 = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){0x01},
        .core_req_wsel   = (uint8_t[1]){0x0C},
        .core_req_byteen = (uint8_t[1]){0x0F},
        .core_req_tid    = (uint8_t[1]){0x00},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){0x0C},
        .core_req_rw     = false,
        .core_req_addr   = 0x80000000

    };

    enqueue(bank.CoreRequests, &core_req3);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
    core_rsp = front(bank.CoreResponse);
	dequeue(bank.CoreResponse);
    printf("responce data=%lx\n" , core_rsp->core_rsp_data[0]);


   Core_Request core_req4 = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){0x01},
        .core_req_wsel   = (uint8_t[1]){0x0B},
        .core_req_byteen = (uint8_t[1]){0x0F},
        .core_req_tid    = (uint8_t[1]){0x00},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){0x0a},
        .core_req_rw     = false,
        .core_req_addr   = 0x80000000

    };

    enqueue(bank.CoreRequests, &core_req4);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
    core_rsp = front(bank.CoreResponse);
	dequeue(bank.CoreResponse);
    printf("responce data=%lx\n" , core_rsp->core_rsp_data[0]);


    Core_Request core_req5 = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){0x01},
        .core_req_wsel   = (uint8_t[1]){0x0D},
        .core_req_byteen = (uint8_t[1]){0x0F},
        .core_req_tid    = (uint8_t[1]){0x00},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){0x0F},
        .core_req_rw     = false,
        .core_req_addr   = 0x80000100

    };

    enqueue(bank.CoreRequests, &core_req5);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);

    mem_req = front(bank.MemRequest);


	if(!mem_req->mem_req_rw){

		mem_rsp->mem_rsp_valid   = true;
		mem_rsp->mem_rsp_id      = mem_req->mem_req_id;
		mem_rsp->mem_rsp_data    = (uint32_t[16]){0xfc027213,0x00a20233,0xedb20213,0x00002217,0x02b50533,0x00005934,0x0b101330,0x0a51593c,0xc102573f,0xf0001372,0x10181930,0x00002197,0x0005006b,0xfff00513,0x0005006b,0x00000513};
		mem_rsp->mem_rsp_ready   = true;

	}


	dequeue(bank.MemRequest);
	enqueue(bank.MemReponse, mem_rsp);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);

    core_rsp = front(bank.CoreResponse);

    for (int i = 0; i < 16; i++)
    {
        printf("data=%lx\n" , core_rsp->core_rsp_data[i]);
    }

	dequeue(bank.CoreResponse);


    Core_Request core_req6 = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){0x01},
        .core_req_wsel   = (uint8_t[1]){0x0D},
        .core_req_byteen = (uint8_t[1]){0x0F},
        .core_req_tid    = (uint8_t[1]){0x00},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){0x1a},
        .core_req_rw     = false,
        .core_req_addr   = 0x80000100

    };

    enqueue(bank.CoreRequests, &core_req6);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
    core_rsp = front(bank.CoreResponse);
	dequeue(bank.CoreResponse);
    printf("responce data=%lx\n" , core_rsp->core_rsp_data[0]);


    Core_Request core_req7 = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){0x01},
        .core_req_wsel   = (uint8_t[1]){0x0D},
        .core_req_byteen = (uint8_t[1]){0x0F},
        .core_req_tid    = (uint8_t[1]){0x00},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){0x1F},
        .core_req_rw     = false,
        .core_req_addr   = 0x80000100

    };

    enqueue(bank.CoreRequests, &core_req7);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
    core_rsp = front(bank.CoreResponse);
	dequeue(bank.CoreResponse);
    printf("responce data=%lx\n" , core_rsp->core_rsp_data[0]);


    Core_Request core_req8 = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){0x01},
        .core_req_wsel   = (uint8_t[1]){0x0D},
        .core_req_byteen = (uint8_t[1]){0x00},
        .core_req_tid    = (uint8_t[1]){0x00},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){0x20},
        .core_req_rw     = false,
        .core_req_addr   = 0x80000100

    };

    enqueue(bank.CoreRequests, &core_req8);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
    core_rsp = front(bank.CoreResponse);
	dequeue(bank.CoreResponse);
    printf("responce data=%lx\n" , core_rsp->core_rsp_data[0]);



     Core_Request core_req9 = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){0x01},
        .core_req_wsel   = (uint8_t[1]){0x0C},
        .core_req_byteen = (uint8_t[1]){0x0F},
        .core_req_tid    = (uint8_t[1]){0x00},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){0x25},
        .core_req_rw     = false,
        .core_req_addr   = 0x80000100

    };

    enqueue(bank.CoreRequests, &core_req9);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
    core_rsp = front(bank.CoreResponse);
	dequeue(bank.CoreResponse);
    printf("responce data=%lx\n" , core_rsp->core_rsp_data[0]);





    Core_Request core_req10 = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){0x01},
        .core_req_wsel   = (uint8_t[1]){0x0D},
        .core_req_byteen = (uint8_t[1]){0x0F},
        .core_req_tid    = (uint8_t[1]){0x00},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){0x0F},
        .core_req_rw     = false,
        .core_req_addr   = 0x80000140

    };

    enqueue(bank.CoreRequests, &core_req10);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);

    mem_req = front(bank.MemRequest);


	if(!mem_req->mem_req_rw){

		mem_rsp->mem_rsp_valid   = true;
		mem_rsp->mem_rsp_id      = mem_req->mem_req_id;
		mem_rsp->mem_rsp_data    = (uint32_t[16]){0xfc027213,0x00a20233,0xedb20213,0x00002217,0x02b50533,0x00005934,0x0b101330,0x0a51593c,0xc102573f,0xf0001372,0x10181930,0x00002197,0x0005006b,0xfff00513,0x0005006b,0x00000513};
		mem_rsp->mem_rsp_ready   = true;

	}


	dequeue(bank.MemRequest);
	enqueue(bank.MemReponse, mem_rsp);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);

    core_rsp = front(bank.CoreResponse);

    for (int i = 0; i < 16; i++)
    {
        printf("data=%lx\n" , core_rsp->core_rsp_data[i]);
    }

	dequeue(bank.CoreResponse);



    Core_Request core_req11 = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){0x01},
        .core_req_wsel   = (uint8_t[1]){0x0D},
        .core_req_byteen = (uint8_t[1]){0x0F},
        .core_req_tid    = (uint8_t[1]){0x00},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){0x218},
        .core_req_rw     = false,
        .core_req_addr   = 0x80000740

    };

    enqueue(bank.CoreRequests, &core_req11);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);

    mem_req = front(bank.MemRequest);


	if(!mem_req->mem_req_rw){

		mem_rsp->mem_rsp_valid   = true;
		mem_rsp->mem_rsp_id      = mem_req->mem_req_id;
		mem_rsp->mem_rsp_data    = (uint32_t[16]){0xfc027213,0x00a20233,0xedb20213,0x00002217,0x02b50533,0x00005934,0x0b101330,0x0a51593c,0xc102573f,0xf0001372,0x10181930,0x00002197,0x0005006b,0xfff00513,0x0005006b,0x00000513};
		mem_rsp->mem_rsp_ready   = true;

	}


	dequeue(bank.MemRequest);
	enqueue(bank.MemReponse, mem_rsp);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
    Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);

    core_rsp = front(bank.CoreResponse);

    for (int i = 0; i < 16; i++)
    {
        printf("data=%lx\n" , core_rsp->core_rsp_data[i]);
    }

	dequeue(bank.CoreResponse);


    printf("passed\n");


}