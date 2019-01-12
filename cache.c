
// -----------------------------------
// CSCI 340 - Operating Systems
// Fall 2018
// cache.c file
// 
// Homework 1
//
// -----------------------------------

#include "cache.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int initializeCache( unsigned int number_of_lines ) {

	int line, retVal;

	retVal = OK;

	cache = malloc( sizeof( cache_line* ) * number_of_lines );

	if ( cache != NULL ) {

		for ( line=0; line<number_of_lines; line++ ) {

			cache[line] = (cache_line*) malloc( sizeof( cache_line ) );
			cache[line]->tag = UNK;
			cache[line]->hit_count = ZERO;

		}

	} else
		retVal = FAIL;

	return retVal;

} // end initializeCache function


int cread( unsigned int cmf, unsigned int* hex_addr, unsigned int* found, unsigned int* replace ) {

	/* TODO: You complete */
	
	int lowestHit;
	int index;
	int tagVal;
	int blockLocation;
	int lineVal;
	int offsetVal;
	int TAG_ISOLATION_FA = 252; /* 1111 1100 */
	int TAG_ISOLATION_DM = 224; /* 1110 0000 */
	int LINE_ISOLATION = 28; /* 0001 1100 */
	int OFFSET_ISOLATION = 3; /* 0000 0011 */
	int stopFlag = 0; /* Boolean switch used in for-loop*/
	int replaceIndex = 0;
	int retVal = OK;

	if (*hex_addr > 255) return FAIL;
	
	if ( cmf == DM ) {
		
		/* This isolates the appropriate bits for manipulation */
		tagVal = *hex_addr & TAG_ISOLATION_DM;
		lineVal = *hex_addr & LINE_ISOLATION; 
		offsetVal = *hex_addr & OFFSET_ISOLATION;
		blockLocation = tagVal | lineVal; // Used to find the index of the block location

		/* This shifts the bits appropriately to find the actual value */
		blockLocation = blockLocation >> 2;
		tagVal = tagVal >> 5;
		lineVal = lineVal >> 2;

		if ( cache[lineVal]->tag == tagVal ) {

			(*found) = HIT;
			(*replace) = NO;
			cache[lineVal]->hit_count++;

		} else {
			
			(*found) = MISS;
			(*replace) = YES;
			cache[lineVal]->hit_count = 0;
			cache[lineVal]->tag = tagVal;

		}

		retVal = phy_memory[block_location[blockLocation] + offsetVal];

	} else if ( cmf == FA ) {

		/* This isolates the appropriate bits for manipulation */
		tagVal = *hex_addr & TAG_ISOLATION_FA;
		offsetVal = *hex_addr & OFFSET_ISOLATION;

		/* This shifts the bits appropriately to find the actual value */
		tagVal = tagVal >> 2;
		
		/* This sets the lowest hit count to the first line of cache*/
		lowestHit = cache[0]->hit_count;

		/* While iterating through the "cache" starting at the first line, 
		   check the line's tag and compare it to UNK (-1). If there is a match, 
		   update the line's tag with tagVal and exit the loop early. If the 
		   cache line's tag is equal to tagVal, record the appropriate results
		   (hit with no replacement, hit count) and exit the loop early. Finally, 
		   if there is no match, check the cache line's hit count and find the lowest
		   value hit count in the cache starting from the top. Store the value of the
		   index in the array in another variable which will be used in the LRU 
		   algorithm.  */
		for (index = 0; index < NUM_OF_LINES && !stopFlag; index++) {

			if (cache[index]->tag == UNK) {

				(*found) = MISS;
				(*replace) = YES;
				cache[index]->tag = tagVal;
				stopFlag = 1;
 
			} else if ( cache[index]->tag == tagVal) {

				(*found) = HIT;
				(*replace) = NO;	
				cache[index]->hit_count++;			
				stopFlag = 1;

			} else if (cache[index]->hit_count < lowestHit) {

				lowestHit = cache[index]->hit_count;
				replaceIndex = index;

			}
		}

		/* The stopFlag will be 0 if there is no tag match in the cache
		   and if there was no empty space in cache */
		if ( stopFlag == 0 ) {

			(*found) = MISS;
			(*replace) = YES;
			cache[replaceIndex]->hit_count = 0;
			cache[replaceIndex]->tag = tagVal;

		}

		retVal = phy_memory[block_location[tagVal] + offsetVal];

	}

	return retVal;

} // end cread function


void cprint() {

	unsigned int line;

	printf("\n---------------------------------------------\n");
	printf("line\ttag\tnum of hits\n");
	printf("---------------------------------------------\n");


	for ( line=0; line<NUM_OF_LINES; line++ ) { 

		if ( cache[line]->tag == UNK ) {
			
			printf("%d\t%d\t%d\n", line, cache[line]->tag, cache[line]->hit_count );

		} else {

			printf("%d\t%02X\t%d\n", line, cache[line]->tag, cache[line]->hit_count );

		}

	}

} // end cprint function
