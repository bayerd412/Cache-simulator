/** @file memory.h
 *  @brief Public API of memory hierarchy.
 *  @see memory.c
 */

#ifndef MEMORY_H
#define MEMORY_H

/* data_t can be any 32-bit type, such as (unsigned long), (void *) or size_t
 * (on 32-bit x86).
 */
typedef unsigned long data_t;

/** Initialize memory hierarchy.
 */
void
memory_init(void);

/** Fetch instruction at given memory address.
 *
 *  @param[in] address Memory address of instruction.
 *  @param[out] data Instruction data returned by reference.
 */
void
memory_fetch(unsigned int address, data_t *data);

/** Read data from memory at given memory address.
 *
 *  @param[in] address Memory address to read from.
 *  @param[out] data Memory data returned by reference.
 */
void
memory_read(unsigned int address, data_t *data);

/** Write to memory at given memory address.
 *
 *  @param[in] address Memory address to write to.
 *  @param[in] data Data to write to memory.
 */
void
memory_write(unsigned int address, data_t *data);

/** Clean up and deinitialize memory hierarchy.
 */
void
memory_finish (void);

//Some new.

//Create a new cache.
void
create_cache(cache_t cache, unsigned int size, unsigned int assoc, unsigned int rp, unsigned int block_size, unsigned int bus, unsigned int wp);

//Transform address and find out tag, index and offset of this address.
void
calc_address(cache_t *Cache, unsigned int address);

//Search for block in cache.
int 
search_block(cache_t *Cache);

//Search for empty block in cache.
int 
search_empty_block(cache_t *Cache);

//Swap blocks between two caches (L1I and L2 or L1D and L2).
void 
change_block(cache_t *Cache, int L1, int L2);

//Find the block in set, which was the least used.
int 
LRU_search(cache_t *Cache);

//Update age of all valid blocks in all caches.
void 
aging();

#endif
