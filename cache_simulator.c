#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define L1_INSTRUCTION_CACHE_SIZE 32768  // 32KB
#define L1_DATA_CACHE_SIZE 32768         // 32KB
#define L2_CACHE_SIZE 262144             // 256KB
#define BLOCK_SIZE 64                    // Assuming cache block size of 64 bytes
#define L1_NUM_BLOCKS ((size_t)L1_INSTRUCTION_CACHE_SIZE / BLOCK_SIZE)
#define L2_NUM_BLOCKS ((size_t)L2_CACHE_SIZE / BLOCK_SIZE)
#define L2_ASSOCIATIVITY 4
#define NUM_PROCESSORS 2
#define NUM_SETS (L2_NUM_BLOCKS / L2_ASSOCIATIVITY)

typedef struct {
    int valid;
    int dirty;
    int tag;
    int data[BLOCK_SIZE / sizeof(int)]; // Assuming int is 4 bytes
} CacheBlock;

CacheBlock l1_instruction_cache[L1_NUM_BLOCKS];
CacheBlock l1_data_cache[L1_NUM_BLOCKS];
CacheBlock l2_cache[NUM_SETS][L2_ASSOCIATIVITY];

int main() {
    printf("Cache simulator\n");
    // Initialize cache
    init_cache();

    // Simulation code goes here

    // Process trace file
    process_trace_file("your_trace_file.din");

    return 0;
}

void init_cache() {
    for (size_t i = 0; i < L1_NUM_BLOCKS; i++) {
        l1_instruction_cache[i].valid = 0;
        l1_instruction_cache[i].dirty = 0;
        l1_instruction_cache[i].tag = -1;
        for (size_t j = 0; j < BLOCK_SIZE / sizeof(int); j++) {
            l1_instruction_cache[i].data[j] = 0;
        }

        l1_data_cache[i].valid = 0;
        l1_data_cache[i].dirty = 0;
        l1_data_cache[i].tag = -1;
        for (size_t j = 0; j < BLOCK_SIZE / sizeof(int); j++) {
            l1_data_cache[i].data[j] = 0;
        }
    }

    for (size_t i = 0; i < NUM_SETS; i++) {
        for (size_t j = 0; j < L2_ASSOCIATIVITY; j++) {
            l2_cache[i][j].valid = 0;
            l2_cache[i][j].dirty = 0;
            l2_cache[i][j].tag = -1;
            for (size_t k = 0; k < BLOCK_SIZE / sizeof(int); k++) {
                l2_cache[i][j].data[k] = 0;
            }
        }
    }
}

void process_trace_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open trace file\n");
        exit(1);
    }

    char operation;
    unsigned long int address;

    while (fscanf(file, "%c %lx\n", &operation, &address) == 2) {
        // Process each memory access operation (operation and address)
        // Here, you would simulate cache accesses based on the trace data
        // For simplicity, let's just print each operation and address
        printf("Operation: %c, Address: 0x%lx\n", operation, address);
    }

    fclose(file);
}