This folder contains the implementation and interface of
a tunable pool allocator. It also contains a c file to
test the implementation of the allocator and an executable
that carries out the tests.

The files are:
~ pool_alloc.c
~ pool_alloc_test.c
~ pool_alloc.h
~ pool_alloc_test

To compile pool_alloc_test:
gcc -Wall -g  pool_alloc.c pool_alloc_test.c -o pool_alloc_test


The allocator splits the pool heap into equally sized pools.
These pools contain blocks of sizes specified by the
pool_init function. These pools are then utilized
for memory allocation. The allocator is optimized for
blocks of theses sizes.

There can be a maximum of 4 pools created and a minimum
of 1.

The size of the cap on pools can be altered by changing the
defined parameter in pool_alloc.c called MAX_NUM_POOLS.

Due to the cap of 4 pools the time complexities of pool_init,
pool_malloc, and pool_free are O(1)


Potential existance of bytes that can't be utilized:

Depending on the size values in the block_sizes array (during pool
initiliazation) there may be bytes that will remain unused or wasted.
The only case where this won't happen is if 65536/block_size_count
is perfectly divisible by all the sizes in the block_sizes array.



