#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// system defs
#define L1_INSTRUCTION_CACHE_SIZE 32768  // 32KB
#define L1_DATA_CACHE_SIZE 32768         // 32KB
#define L2_CACHE_SIZE 262144             // 256KB
#define BLOCK_SIZE 64                    // cache block size of 64 bytes

#define L1_INSTRUCTION_NUM_BLOCKS (L1_INSTRUCTION_CACHE_SIZE / BLOCK_SIZE)
#define L1_DATA_NUM_BLOCKS (L1_DATA_CACHE_SIZE / BLOCK_SIZE)
#define L2_NUM_BLOCKS (L2_CACHE_SIZE / BLOCK_SIZE)
#define L2_ASSOCIATIVITY 4
#define NUM_SETS (L2_NUM_BLOCKS / L2_ASSOCIATIVITY)

// debug mode
#define DEBUG 0

// opcodes
#define MEMORY_READ  0
#define MEMORY_WRITE 1
#define INSTR_FETCH  2
#define IGNORE       3
#define FLUSH_CACHE  4

// cache block struct
typedef struct {
    int valid;
    int dirty;
    int tag;
    int unsigned long data[BLOCK_SIZE / sizeof(int)]; // int is 4 bytes
} CacheBlock;

// energy consumption
#define L1_RW_ENERGY 1
#define L2_RW_ENERGY 2
#define DRAM_RW_ENERGY 4
#define L1_IDLE_ENERGY 0.5
#define L2_IDLE_ENERGY 0.8
#define DRAM_IDLE_ENERGY 0.8