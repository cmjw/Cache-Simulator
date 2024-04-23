#include "./cache_simulator.h"


// cache data
CacheBlock l1_instruction_cache [L1_INSTRUCTION_NUM_BLOCKS];
CacheBlock l1_data_cache        [L1_DATA_NUM_BLOCKS];
CacheBlock l2_cache             [NUM_SETS][DEFAULT_ASSOCIATIVITY];


unsigned long int SET_ASSOCIATIVITY = -1;

// stats
unsigned long int l1_icache_misses = 0;
unsigned long int l1_dcache_misses = 0;
unsigned long int l2_misses = 0;

unsigned long int l1_icache_hits = 0;
unsigned long int l1_dcache_hits = 0;
unsigned long int l2_hits = 0;
unsigned long int dram_hits = 0;

double l1i_energy = 0;
double l1d_energy = 0;
double l2_energy = 0;
double dram_energy = 0;

unsigned char dram[DRAM_SIZE];

double l1i_static_energy = 0;
double l1d_static_energy = 0;
double l2_static_energy = 0;
double dram_static_energy = 0;

unsigned long int total_mem_acces_time = 0;

// clock
double simulation_clock = 0;

// function declarations
void print_title();
void print_stats();
void init_caches();
void process_trace_file(const char* filename);
void process_dinero_trace(const char* filename);

// simulated accesses
unsigned long int* read_l1_icache(unsigned long int address);
unsigned long int* read_l1_dcache(unsigned long int address);
unsigned long int* read_l2_cache(unsigned long int address);
unsigned long int* read_dram(unsigned long int address);

// simulated writes
void write_l1_icache(unsigned long int address, unsigned long int* data);
void write_l1_dcache(unsigned long int address, unsigned long int* data);
void write_l2_cache(unsigned long int address, unsigned long int* data);
void write_dram(unsigned long int address, unsigned long int* data);

// op codes
void do_memory_read(unsigned long int address);
void do_memory_write(unsigned long int address, unsigned long int* data);
void do_instruction_fetch(unsigned long int address, unsigned long int value);
void do_ignore();
void do_cache_flush();

// energy sim
void l1i_idle_energy();
void l1d_idle_energy();
void l2_idle_energy();
void dram_idle_energy();
void l1i_active_energy();
void l1d_active_energy();
void l2_active_energy();
void dram_active_energy();



/**************************************
 * Main entry point
***************************************/
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <trace_file.din> <-n> <-a> <associativity>\n", argv[0]);
        return 1;
    }

    if (argc == 2) {
        print_title();
    }

    // Initialize caches
    SET_ASSOCIATIVITY = DEFAULT_ASSOCIATIVITY;

    // set associativity
    if (argc > 3) {
        int associativity = atoi(argv[4]);
        if (associativity % 2 != 0) {
            fprintf(stderr, "Invalid associativity\n");
            exit(1);
        }
        SET_ASSOCIATIVITY = associativity;
    }

    simulation_clock = 0.0;
    init_caches();

    // print args
    printf("File: %s\n\n", argv[1]);

    // simulation
    process_dinero_trace(argv[1]);

    // stats
    print_stats();

    printf("==========================\n");

    return 0;
}


/**
 * Print a lil title
*/
void print_title() {
    printf("_________               .__               _________.__              .__          __                \n");
    printf("\\_   ___ \\_____    ____ |  |__   ____    /   _____/|__| _____  __ __|  | _____ _/  |_  ___________ \n");
    printf("/    \\  \\/\\__  \\ _/ ___\\|  |  \\_/ __ \\   \\_____  \\ |  |/     \\|  |  \\  | \\__  \\   __\\/  _  \\_  __ \n");
    printf("\\     \\____/ __ \\\\  \\___|   Y  \\  ___/   /        \\|  |  Y Y  \\  |  /  |__/ __ \\|  | (  <_> )  | \\/\n");
    printf(" \\______  (____  /\\___  >___|  /\\___  > /_______  /|__|__|_|  /____/|____(____  /__|  \\____/|__|  \n");
    printf("        \\/     \\/     \\/     \\/     \\/          \\/          \\/                \\/                   \n");
    printf("_________   _________ _________________  ______ \n");
    printf("\\_   ___ \\ /   _____/ \\_____  \\______  \\/  __  \\ \n");
    printf("/    \\  \\/ \\_____  \\    _(__  <   /    />      < \n");
    printf("\\     \\____/        \\  /       \\ /    //   --   \\\n");
    printf(" \\______  /_______  / /______  //____/ \\______  / \n");
    printf("        \\/        \\/         \\/               \\/  \n\n");
}


/**
 * Print Stats
*/
void print_stats() {
    printf("\nStatistics: \n");

    printf("Set Associativity: %lu\n\n", SET_ASSOCIATIVITY);

    // total access time and energy
    printf("Total Access Time and Energy:\n");
    printf("Total Access Time      | Total Dynamic Energy (W) | Total Static Energy (pJ) \n");
    printf("-----------------------|--------------------------|-------------------------\n");
    printf("%-15.2f        | %f           | %f\n", simulation_clock, l1i_energy+l1d_energy+l2_energy+dram_energy,
        l1i_static_energy+l1d_static_energy+l2_static_energy+dram_static_energy);
   printf("\n");

        // L1 cache statistics
    printf("L1 Cache Statistics:\n");
    printf("Component | # Access    | # Misses    | Dyn. Energy | Static Energy (pJ) \n");
    printf("----------|-------------|-------------|-------------|---------------\n");
    printf("L1 icache | %-9lu   | %-9lu   | %-9.2f   | %-9.2f\n",
        l1_icache_hits, l1_icache_misses, l1i_energy, l1i_static_energy);
    printf("L1 dcache | %-9lu   | %-9lu   | %-9.2f   | %-9.2f\n",
        l1_dcache_hits, l1_dcache_misses, l1d_energy, l1d_static_energy);
   printf("\n");

    // L2 cache statistics
    printf("L2 Cache Statistics:\n");
    printf("Component | # Access    | # Misses    | Dyn. Energy | Static Energy (pJ) \n");
    printf("----------|-------------|-------------|-------------|---------------\n");
    printf("L2        | %-9lu   | %-9lu   | %-9.2f  | %-9.2f\n",
        l2_hits, l2_misses, l2_energy, l2_static_energy);
    printf("\n");

    // DRAM stats
    printf("DRAM Statistics:\n");
    printf("Component | # Access    | # Misses    | Dyn. Energy | Static Energy (pJ) \n");
    printf("----------|-------------|-------------|-------------|---------------\n");
    printf("DRAM      | %-9lu   | N/A         | %-9.2f   | %-9.2f\n",
        dram_hits, dram_energy, dram_static_energy);
   printf("\n");
}


/**
 * Process File
*/
void process_dinero_trace(const char* filename) {
    // Process trace file
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open trace file. Is it in Dinero 3 .din format?\n");
        exit(1);
    }

    char operation;
    int opcode;
    unsigned long int address, value;

    printf("Running simulation ...\n");

    while (fscanf(file, "%c %lx %lx\n", &operation, &address, &value) == 3) {
        opcode = operation - '0';


        // memory read
        if (opcode == MEMORY_READ && value == 0) {
            do_memory_read(address);
        }
        // memory write
        else if (opcode == MEMORY_WRITE && value == 0) {
            // what do we write to??
            // just acces DRAM for time?
            do_memory_write(address, &value);
        }
        // instruction fetch
        else if (opcode == INSTR_FETCH) {
            do_instruction_fetch(address, &value);
        }
        // ignore
        else if (opcode == IGNORE && address == 0) {
            void do_ignore();
        }
        // flush cache
        else if (opcode == FLUSH_CACHE && address == 0) {
            void do_cache_flush();
        }
        else {
            printf("Operation: %c, Address: 0x%lx, Value: 0x%lx\n", operation, address, value);
            fprintf(stderr, "Error: Invalid operation code or arguments.\n");
            exit(1);
        }
    }
    fclose(file);
    printf("Simulation Complete.\n\n");
}


/**
 * Initialize all caches
*/
void init_caches() {
    // L1 icache
    for (size_t i = 0; i < L1_INSTRUCTION_NUM_BLOCKS; i++) {
        l1_instruction_cache[i].valid = 0;
        l1_instruction_cache[i].dirty = 0;
        l1_instruction_cache[i].tag = -1;
        for (size_t j = 0; j < BLOCK_SIZE / sizeof(int); j++) {
            l1_instruction_cache[i].data[j] = 0;
        }
    }

    // L1 dcache
    for (size_t i = 0; i < L1_DATA_NUM_BLOCKS; i++) {
        l1_data_cache[i].valid = 0;
        l1_data_cache[i].dirty = 0;
        l1_data_cache[i].tag = -1;
        for (size_t j = 0; j < BLOCK_SIZE / sizeof(int); j++) {
            l1_data_cache[i].data[j] = 0;
        }
    }

    // L2 cache
    for (size_t i = 0; i < NUM_SETS; i++) {
        for (size_t j = 0; j < SET_ASSOCIATIVITY; j++) {
            l2_cache[i][j].valid = 0;
            l2_cache[i][j].dirty = 0;
            l2_cache[i][j].tag = -1;
            for (size_t k = 0; k < BLOCK_SIZE / sizeof(int); k++) {
                l2_cache[i][j].data[k] = 0;
            }
        }
    }
}


/** +++++++++++++++++++++++++++++++++++++++++++
 * Operation wrapper functions
*/


/**
 * Do a memory read.
*/
void do_memory_read(unsigned long int address) {
    read_l1_dcache(address);
}


/**
 * Do a memory write.
*/
void do_memory_write(unsigned long int address, unsigned long int* data) {
    write_l1_dcache(address, data);
}


/**
 * Do an instruction fetch.
*/
void do_instruction_fetch(unsigned long int address, unsigned long int value) {
    read_l1_icache(address);
}


/**
 * Do an ignore.
*/
void do_ignore() {
    // idle energy consumption
    l1i_idle_energy();
    l1d_idle_energy();
    l2_idle_energy();
    dram_idle_energy();

    simulation_clock += ONE_CYCLE;
}


/**
 * Do a cache flush.
 * This won't actually be tested in the trace test cases.
 * But just in case.
*/
void do_cache_flush() {
    init_caches();
    do_ignore(); 
}


/** ++++++++++++++++++++++++
 * Internal cache operations
   +++++++++++++++++++++++++  */


/**
 * Read L1 Instruction Cache
*/
unsigned long int* read_l1_icache(unsigned long int address) {
    l1i_active_energy();
    l1d_idle_energy();
    l2_idle_energy();
    dram_idle_energy();

    // Calculate cache index and tag from the address
    size_t index = (address / BLOCK_SIZE) % L1_INSTRUCTION_NUM_BLOCKS;
    int tag = address / (BLOCK_SIZE * L1_INSTRUCTION_NUM_BLOCKS);

    // Cache hit
    if (l1_instruction_cache[index].valid && l1_instruction_cache[index].tag == tag) {
        l1_icache_hits++;
        simulation_clock += L1_ACCESS_TIME;
        return l1_instruction_cache[index].data;
    }

    // cache miss
    l1_icache_misses++;
    l2_static_energy += 5;
    simulation_clock += L1_ACCESS_TIME;

    // Cache miss, access L2 cache to fetch data
    unsigned long int* data = read_l2_cache(address);

    // Update L1 instruction cache with fetched data
    l1_instruction_cache[index].valid = 1;
    l1_instruction_cache[index].tag = tag;
    memcpy(l1_instruction_cache[index].data, data, BLOCK_SIZE);

    return data;
}


/**
 * Read L1 Data Cache
*/
unsigned long int* read_l1_dcache(unsigned long int address) {
    l1d_active_energy();
    l1i_idle_energy();
    l2_idle_energy();
    dram_idle_energy();

    simulation_clock += L1_ACCESS_TIME;

    size_t index = (address / BLOCK_SIZE) % L1_DATA_NUM_BLOCKS;
    int tag = address / (BLOCK_SIZE * L1_DATA_NUM_BLOCKS);

    if (l1_data_cache[index].valid && l1_data_cache[index].tag == tag) {
        // Cache hit
        l1_dcache_hits++;
        return l1_data_cache[index].data;
    }

    // Cache miss
    l1_dcache_misses++;

    // seg fault
    long unsigned int* data = read_l2_cache(address);

    // Update L1 data cache with fetched data
    l1_data_cache[index].valid = 1;
    l1_data_cache[index].tag = tag;
    l1_data_cache[index].dirty = 0;

    memcpy(l1_data_cache[index].data, data, BLOCK_SIZE);

    // Return pointer to the data
    return l1_data_cache[index].data;
}


/**
 * Read L2 Cache
*/
unsigned long int* read_l2_cache(unsigned long int address) {
    l2_active_energy();
    l1i_idle_energy();
    l1d_idle_energy();
    dram_idle_energy();

    simulation_clock += L2_ACCESS_TIME;
   
    // Calculate set index and tag from the address
    size_t setIndex = (address / BLOCK_SIZE) % NUM_SETS;
    int tag = address / (BLOCK_SIZE * NUM_SETS);

    // find block in the set
    for (size_t i = 0; i < SET_ASSOCIATIVITY; i++) {
        if (l2_cache[setIndex][i].valid && l2_cache[setIndex][i].tag == tag) {
            // Cache hit
            l2_hits++;

            return l2_cache[setIndex][i].data;
        }
    }

    // cache miss
    l2_misses++;
    dram_static_energy += 640;

    // Simulate data fetching from memory
    unsigned long int* data = read_dram(address);

    // Random replacement policy
    int replacementIndex = rand() % SET_ASSOCIATIVITY;


    // Update block with fetched data
    l2_cache[setIndex][replacementIndex].valid = 1;
    l2_cache[setIndex][replacementIndex].tag = tag;
    memcpy(l2_cache[setIndex][replacementIndex].data, data, BLOCK_SIZE);

    return data;
}


/**
 * Access DRAM
 * Simulate only time and energy
*/
unsigned long int* read_dram(unsigned long int address) {
    dram_active_energy();
    l1i_idle_energy();
    l1d_idle_energy();
    l2_idle_energy();

    int* dummy_data = (int*) malloc(BLOCK_SIZE);
    if (!dummy_data) {
        fprintf(stderr, "Malloc failed\n");
        exit(1);
    }

    srand(time(NULL));
    for (size_t i = 0; i < BLOCK_SIZE / sizeof(int); i++) {
        dummy_data[i] = rand();
    }

    dram_hits++;
    simulation_clock += DRAM_ACCESS_TIME; // incurred time should be 50ns

    return dummy_data;
}


/**
 * Write L1 Instruction Cache
*/
void write_l1_icache(unsigned long int address, unsigned long int* data) {
    l1i_active_energy();
    l1d_idle_energy();
    l2_idle_energy();
    dram_idle_energy();

    // ed discussion project clarification:
    // writes are 5ns because only writes to l1,l2 are synchronous
    simulation_clock += WRITE_TIME;

    // Calculate cache index and tag from the address
    size_t index = (address / BLOCK_SIZE) % L1_INSTRUCTION_NUM_BLOCKS;
    int tag = address / (BLOCK_SIZE * L1_INSTRUCTION_NUM_BLOCKS);

    l1_data_cache[index].valid = 1;
    l1_data_cache[index].tag = tag;
    l1_data_cache[index].dirty = 1;

    memcpy(l1_instruction_cache[index].data, data, BLOCK_SIZE);
}


/**
 * Write to the L1 data cache
*/
void write_l1_dcache(unsigned long int address, unsigned long int* data) {
    l1d_active_energy();
    l1i_idle_energy();
    l2_idle_energy();
    dram_idle_energy();
  
    // writes are 5ns because only writes to l1,l2 are synchronous
    simulation_clock += L1_ACCESS_TIME;

    // Calculate cache index and tag from the address
    size_t index = (address / BLOCK_SIZE) % L1_DATA_NUM_BLOCKS;
    int tag = address / (BLOCK_SIZE * L1_DATA_NUM_BLOCKS);

    // Check if the cache line is present
    if (l1_data_cache[index].valid && l1_data_cache[index].tag == tag) {
        l1_dcache_hits++;

        // Check if the cache line is dirty
        if (l1_data_cache[index].dirty) {
            // Write back the modified data to L2 cache or DRAM
            write_l2_cache(l1_data_cache[index].tag, l1_data_cache[index].data);
        }
    } else {
        l1_dcache_misses++;
        simulation_clock += L2_ACCESS_TIME; // l1 miss, l2 miss: 5ns 
    }

    l1_data_cache[index].valid = 1;
    l1_data_cache[index].tag = tag;
    l1_data_cache[index].dirty = 1;

    memcpy(l1_data_cache[index].data, data, BLOCK_SIZE);
}


/**
 * Write to the L2 cache
*/
void write_l2_cache(unsigned long int address, unsigned long int* data) {
    l2_active_energy();
    l1i_idle_energy();
    l1d_idle_energy();
    dram_idle_energy();

    simulation_clock += L2_ACCESS_TIME; // incurred time should be 5ns

    size_t setIndex = (address / BLOCK_SIZE) % NUM_SETS;
    int tag = address / (BLOCK_SIZE * NUM_SETS);

    // Check if block is already present
    for (size_t i = 0; i < SET_ASSOCIATIVITY; i++) {
        if (l2_cache[setIndex][i].valid && l2_cache[setIndex][i].tag == tag) {
            // cache hit
            l2_hits++;
            memcpy(l2_cache[setIndex][i].data, data, BLOCK_SIZE);
            l2_cache[setIndex][i].dirty = 1;

            // hit, dont write back
            l2_hits++;
            return;
        }
    }

    // cache miss
    l2_misses++;
    int replacementIndex = rand() % SET_ASSOCIATIVITY;

    if (l2_cache[setIndex][replacementIndex].dirty) {
        // if evicting block, "write" it back to DRAM
        write_dram(address, l2_cache[setIndex][replacementIndex].data);
    }

    // Update the cache block
    l2_cache[setIndex][replacementIndex].valid = 1;
    l2_cache[setIndex][replacementIndex].tag = tag;
    l2_cache[setIndex][replacementIndex].dirty = 1;
    memcpy(l2_cache[setIndex][replacementIndex].data, data, BLOCK_SIZE);
}


/**
 * "Write" to DRAM
*/
void write_dram(unsigned long int address, unsigned long int* data) {  
    dram_active_energy();
    l1i_idle_energy();
    l1d_idle_energy();
    l2_idle_energy();

    // using a dummy write
    memcpy(dram + address, data, BLOCK_SIZE);

    dram_hits++; 
    // no time incurred for dram write
}

/**
 * Simulate idle L1 icache
*/
void l1i_idle_energy() {
    l1i_energy += L1_IDLE_ENERGY;
}

/**
 * Simulate idle L1 dcache
*/
void l1d_idle_energy() {
    l1d_energy += L1_IDLE_ENERGY;
}


/**
 * Simulate idle L2 cache
*/
void l2_idle_energy() {
    l2_energy += L2_IDLE_ENERGY;
}


/**
 * Simulate idle DRAM
*/
void dram_idle_energy() {
    dram_energy += DRAM_IDLE_ENERGY;
}


/**
 * Simulate energy consumption of L1 icache
*/
void l1i_active_energy() {
    l1i_energy += L1_RW_ENERGY;
}

/**
 * Simulate energy consumption of L1 dcache
*/
void l1d_active_energy() {
    l1d_energy += L1_RW_ENERGY;
}


/**
 * Simulate energy consumption of L2 cache
*/
void l2_active_energy() {
    l2_energy += L2_RW_ENERGY;
}


/**
 * Simulate energy consumption of DRAM
*/
void dram_active_energy() {
    dram_energy += DRAM_RW_ENERGY;
}


/**
 * Proccess input trace file in Din 3 format
*/
void process_trace_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open trace file\n");
        exit(1);
    }

    char operation;
    unsigned long int address;


    while (fscanf(file, "%c %lx\n", &operation, &address) == 2) {
        // ex
        printf("Operation: %c, Address: 0x%lx\n", operation, address);
    }

    fclose(file);
}

