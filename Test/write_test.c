#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "mshr.h"
#include "Bank.h"


#define MAX_NUM_LINES 60
#define MAX_LINE_LEN 200

int main() {
    FILE *fp;
    char line[MAX_LINE_LEN];
    uint64_t addr[MAX_NUM_LINES];
    uint64_t tag[MAX_NUM_LINES];
    uint8_t pmask[MAX_NUM_LINES];
    uint8_t tid[MAX_NUM_LINES];
    uint8_t byteen[MAX_NUM_LINES];
    uint8_t wsel[MAX_NUM_LINES];
    uint32_t data[MAX_NUM_LINES];
    int line_index = 0;

    fp = fopen("write_requests.txt", "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        exit(1);
    }

    while (fgets(line, MAX_LINE_LEN, fp) != NULL && line_index < MAX_NUM_LINES) {
        // Extract the address, tag, pmask, tid, and byteen values from the line
        char *addr_start = strstr(line, "addr=") + 5;
        char *tag_start = strstr(line, "tag=") + 4;
        char *pmask_start = strstr(line, "pmask=") + 6;
        char *tid_start = strstr(line, "tid=") + 4;
        char *byteen_start = strstr(line, "byteen=") + 7;
        char *data_start = strstr(line, "data=") + 5;
        char *wsel_start = strstr(line, "wsel=") + 5;

        sscanf(addr_start, "%x", &addr[line_index]);
        sscanf(tag_start, "%x", &tag[line_index]);
        sscanf(pmask_start, "%d", &pmask[line_index]);
        sscanf(tid_start, "%d", &tid[line_index]);
        //sscanf(byteen_start, "%x", &byteen[line_index]);

        char byteen_str[5];
        strncpy(byteen_str, byteen_start, 4);
        byteen_str[4] = '\0';
        byteen[line_index] = strtol(byteen_str, NULL, 2);

        sscanf(data_start, "%x", &data[line_index]);
        sscanf(wsel_start, "%d", &wsel[line_index]);

        line_index++;
    }

    fclose(fp);

    // Print the extracted values
    /*for (int i = 0; i < line_index; i++) {
        printf("Line %d: addr=%08x, tag=%02x, pmask=%d, tid=%d, byteen=%d, data=%08x, wsel=%d\n",
               i, addr[i], tag[i], pmask[i], tid[i], byteen[i], data[i], wsel[i]);
    }*/
    

    FILE *fp1;
    char line1[MAX_LINE_LEN];
    uint64_t write_back_addr[MAX_NUM_LINES];
    uint8_t write_back_byteen[MAX_NUM_LINES];
    uint32_t write_back_data[MAX_NUM_LINES];
    int line_index1 = 0;

    fp1 = fopen("write_back.txt", "r");
    if (fp1 == NULL) {
        printf("Error opening file\n");
        exit(1);
    }


    while (fgets(line1, MAX_LINE_LEN, fp1) != NULL && line_index1 < MAX_NUM_LINES) {
        // Extract the address, byteen, and data values from the line
        char *addr_start = strstr(line1, "addr=") + 5;
        char *byteen_start = strstr(line1, "byteen=") + 7;
        char *data_start = strstr(line1, "data=") + 5;

        sscanf(addr_start, "%x", &write_back_addr[line_index1]);

        // Read byteen as a binary string and convert it to an integer
        char byteen_str[5];
        strncpy(byteen_str, byteen_start, 4);
        byteen_str[4] = '\0';
        write_back_byteen[line_index1] = strtol(byteen_str, NULL, 2);

        sscanf(data_start, "%x", &write_back_data[line_index1]);

        line_index1++;
    }

    fclose(fp1);

    // Print the extracted values
    /*for (int i = 0; i < line_index1; i++) {
        printf("Line %d: addr=%08x, byteen=%04x (%d%d%d%d), data=%08x\n",
               i, write_back_addr[i], write_back_byteen[i], (write_back_byteen[i] >> 3) & 1, (write_back_byteen[i] >> 2) & 1,
               (write_back_byteen[i] >> 1) & 1, write_back_byteen[i] & 1, write_back_data[i]);
    }*/


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

    int idx = 0;


    while (idx < 60)
    {
        
        Core_Request core_req = {

            .core_req_valid  = true,
            .core_req_pmask  = (uint8_t[1]){pmask[idx]},
            .core_req_wsel   = (uint8_t[1]){wsel[idx]},
            .core_req_byteen = (uint8_t[1]){byteen[idx]},
            .core_req_tid    = (uint8_t[1]){tid[idx]},
            .core_req_data   = (uint32_t[1]){data[idx]},
            .core_req_tag    = (uint64_t[1]){tag[idx]},
            .core_req_rw     = true,
            .core_req_addr   = get_line_addr(bank.params , addr[idx])

        };



        enqueue(bank.CoreRequests, &core_req);
        Schedule(&bank, RAM_SP, TAG_RAM , &bank_req);

        mem_req = front(bank.MemRequest);

        /*printf("address is %lx\n",mem_req->mem_req_addr);
        printf("address is %lx\n",write_back_addr[idx]);
        printf("mem-data=%lx\n" , mem_req->mem_req_data[0]);
        printf("write-back-data=%lx\n" , write_back_data[idx]);*/



        uint32_t memory_address = mem_req->mem_req_addr;
        uint32_t write_back_address = get_line_addr(bank.params , write_back_addr[idx]);
        uint32_t data1 = mem_req->mem_req_data[0];
        uint32_t data2 =  write_back_data[idx];
        printf("address is %lx\n",memory_address);
        printf("address is %lx\n",write_back_address);
        printf("mem-data=%lx\n" , data1);
        printf("write-back-data=%lx\n" , data2);


        bool case1 = memory_address == write_back_address;
        bool case2 = data1 == data2;
        bool case3 = mem_req->mem_req_byteen[0]  == write_back_byteen[idx];


        dequeue(bank.CoreResponse);
        dequeue(bank.MemRequest);


        if(case1 && case2 && case3){

            printf("test case %d is passed\n" , idx);
        }else{

            printf("test case %d is failed\n" , idx);
            printf("test case 1 = %x\n" , case1);
            printf("test case 2 = %x\n" , case2);
            printf("test case 3 = %x\n" , case3);
            break;
        }

        idx++;

    }
    

    return 0;
}