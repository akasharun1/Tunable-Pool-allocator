/*
 * @file pool_alloc.c
 * @brief tunable pool based memory allocator
 *
 *
 * The allocator splits the pool heap into equally sized pools.
 * These pools contain blocks of sizes specified by the
 * pool_init function. These pools are then utilized
 * for memory allocation. The allocator is optimized for
 * blocks of theses sizes.
 *
 * There can be a maximum of 4 pools created and a minimum
 * of 1.
 * Read the data structures section for more on how blocks
 * and pools were implemented.
 *
 * The cap can be changed by altering MAX_NUM_POOLS
 * Due to the cap of 4 pools the time complexities of pool_init,
 * pool_malloc, and pool_free are O(1)
 *
 *
 * Potential existance of bytes that can't be utilized:
 *
 * Depending on the size values in the block_sizes array (during pool
 * initiliazation) there may be bytes that will remain unused or wasted.
 * The only case where this won't happen is if HEAP_SIZE/block_size_count
 * is perfectly divisible by all the sizes in the block_sizes array.
 *
 *
 * @author Akash Arun <akasha@andrew.cmu.edu>
*/


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#define HEAP_SIZE 65536
#define MAX_NUM_POOLS 4


static uint8_t g_pool_heap[HEAP_SIZE];

/* Data Structures */


/* A block is a data structure that makes up the pools of
 * the allocator and is made up of:
 * ~ A union which:
 *   - if the block is allocated contains the payload (has the data)
 *   - if the block is free contains either a pointer to the next free block
 *     in the pool or to NULL
 */

typedef struct block {
    union {
        struct block *next;
        char payload[0];
    };
} block_t;

/* A pool is a data structure that contains blocks and
 * consists of:
 * ~ A pointer to the first block of the pool
 * ~ A pointer to either:
 *   - the latest freed block of the pool
 *   - the first free block of the pool
 *   - a location outside of the pool (when the pool is full)
 * ~ A pointer to the last block of the pool
*/

typedef struct pool {

    block_t *pool_start;
    block_t *pool_curr;
    block_t *pool_end;

    size_t pool_block_size;
} pool_t;


/* Global Variables:
 * They are initialized by the pool_init function
*/

/*
Design decision: A max of 4 pools
i.e a max of 4 different block sizes
*/

static pool_t pools_list[MAX_NUM_POOLS];
static size_t num_pools = 0;


/* Helper Functions: */


/* @brief finds the next free block in a pool
 *
 * param[in] block: the address of the current block
 * param[in] size: the size of blocks in a specific pool
 *
 * returns the address of the next block in a pool
*/

block_t *find_next(block_t *block, size_t size)
{
    if (block->next == NULL) {
        // If the block has never been allocated before
        // ,i.e part of the unallocated chunk
        return (block_t *)((uint8_t *)block+size);
    }
    else {
        // If the block was allocated and then freed
        return block->next;
    }
}

/* @brief finds and returns a free block that is greater
 * than or equal to size; else returns NULL
 *
 * param[in] i: the index of the pool
 * param[in] size: size of the requested block
 *
 * returns the address of a block or NULL
*/

block_t *find_fit(size_t i, size_t size)
{
    block_t *block = pools_list[i].pool_curr;

    // returns NULL if pool is full, i.e if the block's address is
    // greater than the pool's last block, or if the requested size
    // is greater than the size of blocks in this pool
    if (block >  pools_list[i].pool_end ||
        size > pools_list[i].pool_block_size) {
        return NULL;
    }
    pools_list[i].pool_curr =
        find_next(block, pools_list[i].pool_block_size);
    return block;
}

/* @brief adds a certain block back to the pools current
 * pointer so that it is available for future allocation
 *
 * param[in] i: the index of the pool
 * param[in] block: address of the block that is being
 * added to the pool
*/

void add_to_pool(size_t i, block_t *block)
{
    block->next = pools_list[i].pool_curr;
    pools_list[i].pool_curr = block;

}


/* @brief Checks the parameters provided for initialization of the pools
 *
 * param[in] block_sizes: A list containing the payload sizes
 * of the blocks in each respective pool
 *
 * param[in] block_sizes_count: Number of differently sized blocks possible
 * returns true if parameters are appropriate, else returns false
 *
 * Pool initialization fails if:
 * ~ number of block sizes is < 1
 * ~ number of block sizes is > 4
 * ~ the list containing block sizes is NULL
 * ~ block sizes small enough that each pool can atleast store one block
 */

bool param_verif(const size_t *block_sizes,size_t block_size_count)
{
    if (block_size_count > MAX_NUM_POOLS || block_size_count == 0
        || block_sizes == NULL) {
        return false;
    }

    size_t max_pool_size  = HEAP_SIZE/block_size_count;

    for (size_t i = 0; i < block_size_count; i++) {
        // checks if atleast 1 block can fit in the pool
        if (max_pool_size/(block_sizes[i]) < 1) {
            return false;
        }
    }

    return true;
}

/* Main Functions */


/* @brief Initializes the pools based on input parameters
 *
 * param[in] block_sizes: A list containing the payload sizes
 * of the blocks in each respective pool
 * param[in] block_sizes_count: Number of differently sized blocks possible
 *
 * returns true if initialization is succesful
 * else returns false
 *
 * Precondition: len(block_sizes) == block_size_count
 *
 * Potential existance of bytes that can't be utilized:
 *
 * Depending on the size values in the block_sizes array there may be
 * bytes that will remain unused or wasted. The only case where
 * this won't happen is if HEAP_SIZE/block_size_count is perfectly
 * divisible by all the sizes in the block_sizes array.
 *
 * Time Complexity: O(1)
 *
 * */

bool pool_init(const size_t *block_sizes, size_t block_size_count)
{

    size_t index, end_index, block_count, space_wastage, max_pool_size;

    if (param_verif(block_sizes, block_size_count) == false) {
        return false;
    }

    num_pools = block_size_count;

    index = 0;
    max_pool_size = HEAP_SIZE/block_size_count;

    for (size_t i = 0; i < num_pools; i++) {
        pools_list[i].pool_block_size = block_sizes[i];

        // address of the first block of the pool
        pools_list[i].pool_start = (block_t *) &(g_pool_heap[index]);
        pools_list[i].pool_curr =  (block_t *) &(g_pool_heap[index]);

        // number of blocks of that size that fit in the pool
        block_count = max_pool_size/(block_sizes[i]);
        // number of unused bytes of space in a pool
        space_wastage = max_pool_size - (block_count * block_sizes[i]);
        // index of the last block of the pool
        end_index = index + max_pool_size - block_sizes[i] - space_wastage;

        // address of the last block of the pool
        pools_list[i].pool_end = (block_t *) &(g_pool_heap[end_index]);

        index += max_pool_size;
    }
    return true;
}

/* @brief allocates an object of size n on the g_pool_heap if
 * n is less than or equal to the size of blocks in a certain
 * pool and the pool has space. Else fails
 *
 *
 * param[in] n: size of the object that is to be allocated
 *
 * returns the address of the allocated memory or NULL
 *
 * Time Complexity: O(1)
*/

void *pool_malloc(size_t n)
{
    // returns NULL if n is greater than the size of blocks in the largest pool
    if (n > pools_list[num_pools-1].pool_block_size || n < 1) {
        return NULL;
    }

    for (size_t i = 0; i < num_pools; i++) {
        block_t *block = find_fit(i, n);
        if (block != NULL) {
            return (void *) block->payload;
        }
    }
    return NULL;
}

/* @brief frees an allocated object on the g_pool_heap so that
 * the memory can be re-used
 *
 * param[in] ptr: the address to allocated memory to be freed
 *
 * Time Complexity: O(1)
*/

void pool_free(void *ptr)
{
    block_t *block = (block_t *) ptr;

    // Cases for if block is NULL or outside of the heap
    if (block == NULL || block < pools_list[0].pool_start ||
        block > pools_list[num_pools-1].pool_end) {
        return;
    }

     for (size_t i = 0; i < num_pools; i++) {
        // checks if the block is between the first and last block of a certain
        // pool
        if (block >= pools_list[i].pool_start && block <= pools_list[i].pool_end) {
            add_to_pool(i, block);
            break;
        }
    }
}


