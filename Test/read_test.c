#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <stdint.h>
#include "mshr.h"
#include "Bank.h"

#define MAX_DATA_LEN 1024
#define ROWS_NUM     39

int main() {
    FILE *fp;
    char line[MAX_DATA_LEN];
    uint32_t data[ROWS_NUM][MAX_DATA_LEN];
    int data_len = 0;
    int j = 0; 

    fp = fopen("fill.txt", "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(1);
    }

    while (fgets(line, MAX_DATA_LEN, fp) != NULL) {
        // Find the position of the data field in the line
        char *data_start = strstr(line, "data=");
        data_len = 0;

        if (data_start != NULL) {
            // Extract the data field
            sscanf(data_start, "data=%s", line);
            int len = strlen(line);
            for (int i = 0; i < len; i += 8) {
                sscanf(&line[i], "%8x", &data[j][data_len++]);
            }
        }
        j++;
    }

    fclose(fp);


    
    /*for (int i = 0; i < data_len; i++)
    {
        printf(" line %d %02x\n" , i , data[1][i]);
    }*/






    FILE *fp1;
    char line1[MAX_DATA_LEN];
    uint64_t addr[MAX_DATA_LEN];
    uint64_t tag[MAX_DATA_LEN];
    uint8_t pmask[MAX_DATA_LEN];
    uint8_t tid[MAX_DATA_LEN];
    uint8_t wsel[MAX_DATA_LEN];
    int line_index = 0;

    fp1 = fopen("read_requests.txt", "r");
    if (fp1 == NULL) {
        printf("Error opening file\n");
        exit(1);
    }
    

    while (fgets(line1, MAX_DATA_LEN, fp1) != NULL) {
        // Extract the address, tag, pmask, and tid values from the line
        char *addr_start = strstr(line1, "addr=") + 5;
        char *tag_start = strstr(line1, "tag=") + 4;
        char *pmask_start = strstr(line1, "pmask=") + 6;
        char *tid_start = strstr(line1, "tid=") + 4;
        char *wsel_start = strstr(line1, "wsel=") + 5;

        sscanf(addr_start, "%x", &addr[line_index]);
        sscanf(tag_start, "%x", &tag[line_index]);
        sscanf(pmask_start, "%x", &pmask[line_index]);
        sscanf(tid_start, "%x", &tid[line_index]);
        sscanf(wsel_start, "%d", &wsel[line_index]);

        line_index++;
    }


    fclose(fp1);




    FILE *fp2;
    char line2[MAX_DATA_LEN];
    uint64_t rsp_addr[MAX_DATA_LEN];
    uint64_t rsp_tag[MAX_DATA_LEN];
    uint8_t rsp_pmask[MAX_DATA_LEN];
    uint8_t rsp_tid[MAX_DATA_LEN];
    uint32_t rsp_data[MAX_DATA_LEN];
    int line_index1 = 0;

    fp2 = fopen("coreresponse.txt", "r");
    if (fp2 == NULL) {
        printf("Error opening file\n");
        exit(1);
    }


    
    while (fgets(line2, MAX_DATA_LEN, fp2) != NULL) {
        // Extract the address, tag, pmask, tid, and data values from the line
        char *addr_start = strstr(line2, "addr=") + 5;
        char *tag_start = strstr(line2, "tag=") + 4;
        char *pmask_start = strstr(line2, "pmask=") + 6;
        char *tid_start = strstr(line2, "tid=") + 4;
        char *data_start = strstr(line2, "data=") + 5;

        sscanf(addr_start, "%x", &rsp_addr[line_index1]);
        sscanf(tag_start, "%x", &rsp_tag[line_index1]);
        sscanf(pmask_start, "%x", &rsp_pmask[line_index1]);
        sscanf(tid_start, "%x", &rsp_tid[line_index1]);
        sscanf(data_start, "%x", &rsp_data[line_index1]);

        line_index1++;
    }

    fclose(fp2);

    uint64_t TAG_RAM[64];
    initialize_TAG_SP_random(TAG_RAM);


    uint8_t RAM_SP[64][64];
    initialize_RAM_SP_random(RAM_SP);

    Memory_Response *mem_rsp  = (Memory_Response*)calloc(1 , sizeof(Memory_Response));
    Mem_Request     *mem_req  = (Mem_Request*)calloc(1 , sizeof(Mem_Request));
    Core_Response   *core_rsp = (Core_Response*)calloc(1 , sizeof(Core_Response));

    bank_req_t bank_req;
    Init_bank_req(&bank_req);

    Bank bank;
	Init_Bank(&bank);

    int idx = 0;
    int mem_idx = 0;

    //Core_Request *core_req = (Core_Request*)calloc(1 , sizeof(Core_Request));


    printf("beginning\n");
    while (idx < 51)
    {
        
        
    Core_Request core_req = {

        .core_req_valid  = true,
        .core_req_pmask  = (uint8_t[1]){pmask[idx]},
        .core_req_wsel   = (uint8_t[1]){wsel[idx]},
        .core_req_byteen = (uint8_t[1]){0xf},
        .core_req_tid    = (uint8_t[1]){tid[idx]},
        .core_req_data   = NULL,
        .core_req_tag    = (uint64_t[1]){tag[idx]},
        .core_req_rw     = false,
        .core_req_addr   = get_line_addr(bank.params , addr[idx])

    };


        //printf("assigment\n");

        enqueue(bank.CoreRequests, &core_req);
        Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);

        if(is_empty(bank.CoreResponse)){

            mem_req = front(bank.MemRequest);

            if(!mem_req->mem_req_rw){

                mem_rsp->mem_rsp_valid   = true;
                mem_rsp->mem_rsp_id      = mem_req->mem_req_id;
                mem_rsp->mem_rsp_data    = data[mem_idx];
                mem_rsp->mem_rsp_ready   = true;
                mem_idx++;
	        }

            dequeue(bank.MemRequest);
	        enqueue(bank.MemReponse, mem_rsp);

            Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
            Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);
            core_rsp = front(bank.CoreResponse);
            

            bool case1 = core_rsp->core_rsp_data[core_req.core_req_wsel[0]]  == rsp_data[idx];
            bool case2 = core_rsp->core_rsp_pmask[0] == rsp_pmask[idx];
            bool case3 = core_rsp->core_rsp_tag[0]   == rsp_tag[idx];
            bool case4 = core_rsp->core_rsp_tid[0]   == rsp_tid[idx];

            dequeue(bank.CoreResponse);

            if(case1 && case2 && case3 && case4){

                printf("test case %d is passed\n" , idx);
            }else{

                printf("test case %d is failed\n" , idx);
                break;
            }

        }else{


            core_rsp = front(bank.CoreResponse);


            bool case1 = core_rsp->core_rsp_data[0]  == rsp_data[idx];
            bool case2 = core_rsp->core_rsp_pmask[0] == rsp_pmask[idx];
            bool case3 = core_rsp->core_rsp_tag[0]   == rsp_tag[idx];
            bool case4 = core_rsp->core_rsp_tid[0]   == rsp_tid[idx];

            dequeue(bank.CoreResponse);

            if(case1 && case2 && case3 && case4){

                printf("test case %d is passed\n" , idx);
            }else{

                printf("test case %d is failed\n" , idx);
                break;
            }

        }

        idx++;

    }
    



    return 0;
}