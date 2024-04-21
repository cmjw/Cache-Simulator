#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define L1_INSTRUCTION_CACHE_SIZE 32768  // 32KB
#define L1_DATA_CACHE_SIZE 32768         // 32KB
#define L2_CACHE_SIZE 262144             // 256KB
#define BLOCK_SIZE 64                    // Assuming cache block size of 64 bytes
#define L1_INSTRUCTION_NUM_BLOCKS (L1_INSTRUCTION_CACHE_SIZE / BLOCK_SIZE)
#define L1_DATA_NUM_BLOCKS (L1_DATA_CACHE_SIZE / BLOCK_SIZE)
#define L2_NUM_BLOCKS (L2_CACHE_SIZE / BLOCK_SIZE)
#define L2_ASSOCIATIVITY 4
#define NUM_SETS (L2_NUM_BLOCKS / L2_ASSOCIATIVITY)

typedef struct {
    int valid;
    int dirty;
    int tag;
    int data[BLOCK_SIZE / sizeof(int)]; // Assuming int is 4 bytes
} CacheBlock;

CacheBlock l1_instruction_cache[L1_INSTRUCTION_NUM_BLOCKS];
CacheBlock l1_data_cache[L1_DATA_NUM_BLOCKS];
CacheBlock l2_cache[NUM_SETS][L2_ASSOCIATIVITY];

// stats
unsigned long int l1_icache_misses = 0;
unsigned long int l1_icache_hits = 0;
unsigned long int l1_energy = 0;

unsigned long int l1_dcache_misses = 0;
unsigned long int l1_dcache_hits = 0;

unsigned long int l2_misses = 0;
unsigned long int l2_hits = 0;
unsigned long int l2_cache = 0;

unsigned long int total_mem_acces_time = 0;

void init_caches() {
    // Initialize caches to all zeros
    for (size_t i = 0; i < L1_INSTRUCTION_NUM_BLOCKS; i++) {
        l1_instruction_cache[i].valid = 0;
        l1_instruction_cache[i].dirty = 0;
        l1_instruction_cache[i].tag = -1;
        for (size_t j = 0; j < BLOCK_SIZE / sizeof(int); j++) {
            l1_instruction_cache[i].data[j] = 0;
        }
    }

    for (size_t i = 0; i < L1_DATA_NUM_BLOCKS; i++) {
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

// Cache access function for L1 instruction cache
void access_l1_instruction_cache(unsigned long int address) {
    // Calculate cache index and tag
    size_t index = (address / BLOCK_SIZE) % L1_INSTRUCTION_NUM_BLOCKS;
    int tag = address / (BLOCK_SIZE * L1_INSTRUCTION_NUM_BLOCKS);

    // Check if cache hit
    if (l1_instruction_cache[index].valid && l1_instruction_cache[index].tag == tag) {
        // Cache hit
        return;
    }

    // Cache miss
    // Randomly select a cache block to replace
    size_t random_index = rand() % L1_INSTRUCTION_NUM_BLOCKS;

    // If the cache block to be replaced is dirty, write it back to memory (not applicable for instruction cache)

    // Read data from memory and update cache
    // Simulate fetching data from memory and updating the cache
    printf("Fetching instruction from memory and updating L1 instruction cache\n");

    // Update cache with new instruction
    l1_instruction_cache[random_index].valid = 1;
    l1_instruction_cache[random_index].dirty = 0; // Instructions are not dirty
    l1_instruction_cache[random_index].tag = tag;
    // Assuming instruction is read from memory and updated in cache
    // For simplicity, we just print a message here
    printf("Instruction updated in L1 instruction cache\n");

    l1_instruction_cache_misses++; // Increment L1 instruction cache miss counter
}

// Cache access function for L1 data cache
void access_l1_data_cache(unsigned long int address) {
    // Calculate cache index and tag
    size_t index = (address / BLOCK_SIZE) % L1_DATA_NUM_BLOCKS;
    int tag = address / (BLOCK_SIZE * L1_DATA_NUM_BLOCKS);

    // Check if cache hit
    if (l1_data_cache[index].valid && l1_data_cache[index].tag == tag) {
        // Cache hit
        return;
    }

    // Cache miss
    // Randomly select a cache block to replace
    size_t random_index = rand() % L1_DATA_NUM_BLOCKS;

    // If the cache block to be replaced is dirty, write it back to memory (not applicable for instruction cache)

    // Read data from memory and update cache
    // Simulate fetching data from memory and updating the cache
    printf("Fetching data from memory and updating L1 data cache\n");

    // Update cache with new data
    l1_data_cache[random_index].valid = 1;
    l1_data_cache[random_index].dirty = 0; // Data fetched from memory is not dirty
    l1_data_cache[random_index].tag = tag;
    // Assuming data is read from memory and updated in cache
    // For simplicity, we just print a message here
    printf("Data updated in L1 data cache\n");

    l1_data_cache_misses++; // Increment L1 data cache miss counter
}

// Cache access function for L2 cache
void access_l2_cache(unsigned long int address) {
    // Cache access code remains the same

    if (!cache_hit) {
        l2_cache_misses++; // Increment L2 cache miss counter
    }

    return cache_hit;
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

int main(int argc, char *argv[]) {
    // Initialize caches
    init_caches();

    printf("Cache simulator\n");

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <trace_file.din>\n", argv[0]);
        return 1;
    }

    // Simulation code goes here

    // Process trace file
    process_trace_file("your_trace_file.din");

    return 0;

    // Process memory accesses (assuming address is read from command line arguments)
    unsigned long int address = strtol(argv[1], NULL, 16);
    access_l1_instruction_cache(address);
    access_l1_data_cache(address);

    // Print cache miss statistics
    printf("L1 Instruction Cache Misses: %lu\n", l1_instruction_cache_misses);
    printf("L1 Data Cache Misses: %lu\n", l1_data_cache_misses);
    printf("L2 Cache Misses: %lu\n", l2_cache_misses);

    return 0;
}
