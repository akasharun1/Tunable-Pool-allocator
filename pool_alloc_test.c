/*
 * @file pool_alloc_test.c
 * @brief test cases for pool_alloc.c
 *
 * @author Akash Arun <akasha@andrew.cmu.edu>
*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "pool_alloc.h"

int main() {

    // pool_init test cases:
    printf("Testing pool_init:\n");


    printf("\n1. Testing if false when block size is < 1 ");

    size_t test1[2];
    test1[0] = 1;
    test1[1] = 32;

    if (pool_init(test1, (size_t) 0)) {
        printf("........Failed");
        return 0;
    }
    else {
        printf("........Passed");
    }

    printf("\n2. Testing if false when block size is > 4 ");


    if (pool_init(test1, 8)) {
        printf("........Failed");
        return 0;
    }
    else {
        printf("........Passed");
    }


    printf("\n3. Testing if false when block sizes list is NULL ");


    if (pool_init(NULL, 2)) {
        printf("........Failed");
        return 0;
    }
    else {
        printf("........Passed");
    }

    printf("\n4. Testing if false when the block sizes cannot fit in the heap\n "
            "  (i.e if there is a block size such that there can't be\n "
            "  atleast 1 block in the pool) ");

    test1[0] = 85536;

    if (pool_init(test1, 2)) {
        printf("........Failed");
        return 0;
    }
    else {
        printf("........Passed");
    }

    size_t test2[4];

    test2[0] = 32;
    test2[1] = 64;
    test2[2] = 547;
    test2[3] = 1238;



    printf("\n5. Testing if true with normal parameters ");

    if (pool_init(test2, 4)) {
        printf("........Passed");
    }
    else {
        printf("........Failed");
        return 0;
    }


    printf("\n\n");

    // pool_malloc test cases:

    printf("Testing pool_malloc:\n");


    printf("\n1. Testing if NULL when n is 0 ");

    if (pool_malloc(0) == NULL) {
        printf("........Passed");
    }
    else {
        printf("........Failed");
        return 0;
    }


    printf("\n2. Testing if NULL when n is > largest block ");

    if (pool_malloc(5000) == NULL) {
        printf("........Passed");
    }
    else {
        printf("........Failed");
        return 0;
    }


    printf("\n3. Testing if works when n is a normal size ");

    int* testi = pool_malloc(sizeof(int));
    int* testj = pool_malloc(sizeof(int));

    if (testi == NULL) {
        printf("........Failed");
        return 0;
    }
    else {
        printf("........Passed");
    }

    printf("\n4. Testing if allocated memory works at storing usable data ");

    *testi = 4;
    *testj = 5;

    if (*testi != 4) {
        printf("........Failed");
        return 0;
    }
    if (*testj != 5) {
        printf("........Failed");
        return 0;
    }

    *testi = 1928311;

    if (*testi != 1928311) {
        printf("........Failed");
        return 0;
    }

    printf("........Passed");

    pool_free(testi);

    pool_free(testj);

    printf("\n5. Testing if NULL if largest pool is full and largest "
            "block is requested ");



    // 13 since (65536/4)/1238 is 13
    // So a max of 13 blocks can be allocated
    // until the pool is full


    for (size_t i= 0; i<13; i++) {

        if (pool_malloc(1238) == NULL) {
            printf("........Failed");
            return 0;
        }
    }

    if (pool_malloc(1238) != NULL) {
        printf("........Failed");
        return 0;
    }

    printf("........Passed");

    printf("\n6. Testing if sizes less than the largest block"
            " but not a perfect fit work ");

    if (pool_malloc(200) == NULL) {
        printf("........Failed");
        return 0;
    }

    if (pool_malloc(34) == NULL) {
        printf("........Failed");
        return 0;
    }


    printf("........Passed");

    printf("\n6. Testing if allocation still works when a block is requested\n"
            "   even if the normal pool for that size is fully allocated, if\n"
            "   pools of larger block sizes exist ");

    // 512 since (65536/4)/32 is 512
    // So a max of 512 blocks can be allocated
    // until the pool is full


    for (size_t i= 0; i<512; i++) {

        if (pool_malloc(32) == NULL) {
            printf("........Failed");
            return 0;
        }
    }

    if (pool_malloc(32) == NULL) {
        printf("........Failed");
        return 0;
    }

    printf("........Passed");


    printf("\n\n");

    // pool_free test cases:

    printf("Testing pool_free:\n");


    printf("\n1. Testing if no errors when invalid addresses are provided ");


    pool_free(NULL);

    pool_free((char *)5);


    printf("........Passed");


    printf("\n2. Testing if free works as intended, i.e address of\n"
            "   next malloc after free is as intended ");


    long *testa = pool_malloc(sizeof(long));



    long *tesla = pool_malloc(sizeof(long));
    pool_free(tesla);

    pool_free(testa);

    long *testy = pool_malloc(sizeof(long));

    if (testy != testa){

        printf("........Failed");
        return 0;
    }
    pool_free(testy);
    testa = pool_malloc(sizeof(long));
    tesla = pool_malloc(sizeof(long));

    // order of freeing changed
    // checking if it has expected outcome
    // on behaviour of future allocation

    pool_free(testa);

    pool_free(tesla);

    testy = pool_malloc(sizeof(long));


    if (testy != tesla){

        printf("........Failed");
        return 0;
    }
    pool_free(testy);


    printf("........Passed");
    printf("\n");
    printf("\n");

    printf("All test passed!\n");



}









