/** @file memory.c
 *  @brief Implements starting point for a memory hierarchy with caching and RAM.
 *  @see memory.h
 */

#include "memory.h"

#include <stdio.h>

static unsigned long instr_count;
static unsigned long L1I_num_tags;
static unsigned long L1D_num_tags;

struct cache {
  unsigned int cache_size;
  unsigned int cache_assoc;
  unsigned int rep_policy;
  unsigned int block_size;
  unsigned int bus_width;
  unsigned int write_policy;

  unsigned int read_hit;
  unsigned int read_miss;
  unsigned int write_hit;
  unsigned int write_miss;

  unsigned int num_sets;
  unsigned int cache_tags;
  unsigned int dirty_bit;
  unsigned int valid_bit;

  int cache_LRU;
  
  struct *cache nextCache;
};

void
memory_init(void)
{
  struct cache* L1I_Cache = (struct cache*)malloc(sizeof(struct cache));
  struct cache* L1D_Cache = (struct cache*)malloc(sizeof(struct cache));
  struct cache* L2_Cache = (struct cache*)malloc(sizeof(struct cache));
  
  instr_count = 0;
  L1I_num_tags = 0;
  L1D_num_tags = 0;

//**************** L1 Instruction Cache ******************
  L1I_Cache->cache_size = 32768;
  L1I_Cache->cache_assoc = 4;
  L1I_Cache->block_size = 64;
  
  L1I_Cache->num_sets = L1I_Cache->cache_size / (L1I_Cache->cache_assoc * L1I_Cache->block_size);

  if (L1I_Cache->num_sets == 0 || (L1I_Cache->num_sets % 2 != 0)){
    printf("\nNumber of sets in L1 Instruction Cache isnot a power of two!\n");
    return (0);
  }

  L1I_num_tags = L1I_Cache->num_sets * L1I_Cache->cache_assoc;

  L1I_Cache->cache_tags = (unsigned int*)malloc((L1I_num_tags*sizeof(unsigned int)));
  L1I_Cache->dirty_bit = (unsigned int*)malloc((L1I_num_tags*sizeof(unsigned int)));
  L1I_Cache->valid_bit = (unsigned int*)malloc((L1I_num_tags*sizeof(unsigned int)));
  L1I_Cache->cache_LRU = (int*)malloc((L1I_num_tags*sizeof(int)));

  memset(L1I_Cache->cache_tags, 0, (sizeof(L1I_Cache->cache_tags[0])*L1I_num_tags));
  memset(L1I_Cache->dirty_bit, 0, (sizeof(L1I_Cache->dirty_bit[0])*L1I_num_tags));
  memset(L1I_Cache->valid_bit, 0, (sizeof(L1I_Cache->valid_bit[0])*L1I_num_tags));
  memset(L1I_Cache->cache_LRU, 0, (sizeof(L1I_Cache->cache_LRU[0])*L1I_num_tags));

  L1I_Cache->read_hit = 0;
  L1I_Cache->read_miss = 0;
  L1I_Cache->write_hit = 0; //Useless, as we are not going to
  L1I_Cache->write_miss = 0; //write anything in this cache.
  L1I_Cache->nextCache = NULL;

//**************** L1 Data Cache *******************
  L1D_Cache->cache_size = 32768;
  L1D_Cache->cache_assoc = 8;
  L1D_Cache->block_size = 64;
  
  L1D_Cache->num_sets = L1D_Cache->cache_size / (L1D_Cache->cache_assoc * L1D_Cache->block_size);

  if (L1D_Cache->num_sets == 0 || (L1D_Cache->num_sets % 2 != 0)){
    printf("\nNumber of sets in L1 Data Cache isnot a power of two!\n");
    return (0);
  }

  L1D_num_tags = L1D_Cache->num_sets * L1D_Cache->cache_assoc;

  L1D_Cache->cache_tags = (unsigned int*)malloc((L1D_num_tags*sizeof(unsigned int)));
  L1D_Cache->dirty_bit = (unsigned int*)malloc((L1D_num_tags*sizeof(unsigned int)));
  L1D_Cache->valid_bit = (unsigned int*)malloc((L1D_num_tags*sizeof(unsigned int)));
  L1D_Cache->cache_LRU = (int*)malloc((L1D_num_tags*sizeof(int)));

  memset(L1D_Cache->cache_tags, 0, (sizeof(L1D_Cache->cache_tags[0])*L1D_num_tags));
  memset(L1D_Cache->dirty_bit, 0, (sizeof(L1D_Cache->dirty_bit[0])*L1D_num_tags));
  memset(L1D_Cache->valid_bit, 0, (sizeof(L1D_Cache->valid_bit[0])*L1D_num_tags));
  memset(L1D_Cache->cache_LRU, 0, (sizeof(L1D_Cache->cache_LRU[0])*L1D_num_tags));

  L1D_Cache->read_hit = 0;
  L1D_Cache->read_miss = 0;
  L1D_Cache->write_hit = 0;
  L1D_Cache->write_miss = 0;
  L1D_Cache->nextCache = NULL;
  
//**************** L2 Unified Cache ******************
  L2_Cache->cache_size =; 262144;
  L2_Cache->cache_assoc = 8;
  L2_Cache->block_size = 64;
  
  L2_Cache->num_sets = L2_Cache->cache_size / (L2_Cache->cache_assoc * L2_Cache->block_size);

  if (L2_Cache->num_sets == 0 || (L2_Cache->num_sets % 2 != 0)){
    printf("\nNumber of sets in L1 Data Cache isnot a power of two!\n");
    return (0);
  }

  L2_num_tags = L2_Cache->num_sets * L2_Cache->cache_assoc;

  L2_Cache->cache_tags = (unsigned int*)malloc((L2_num_tags*sizeof(unsigned int)));
  L2_Cache->dirty_bit = (unsigned int*)malloc((L2_num_tags*sizeof(unsigned int)));
  L2_Cache->valid_bit = (unsigned int*)malloc((L2_num_tags*sizeof(unsigned int)));
  L2_Cache->cache_LRU = (int*)malloc((L2_num_tags*sizeof(int)));

  memset(L2_Cache->cache_tags, 0, (sizeof(L2_Cache->cache_tags[0])*L2_num_tags));
  memset(L2_Cache->dirty_bit, 0, (sizeof(L2_Cache->dirty_bit[0])*L2_num_tags));
  memset(L2_Cache->valid_bit, 0, (sizeof(L2_Cache->valid_bit[0])*L2_num_tags));
  memset(L2_Cache->cache_LRU, 0, (sizeof(L2_Cache->cache_LRU[0])*L2_num_tags));

  L2_Cache->read_hit = 0;
  L2_Cache->read_miss = 0;
  L2_Cache->write_hit = 0;
  L2_Cache->write_miss = 0;
  L2_Cache->nextCache = NULL;
  L1D_Cache->nextCache = L2_Cache;
  L1I_Cache->nextCache = L2_Cache;


}

void
memory_fetch(unsigned int address, data_t *data)
{
  printf("memory: fetch 0x%08x\n", address);
  if (data)
    *data = (data_t) 0;
  
  instr_count++;
}

void
memory_read(unsigned int address, data_t *data)
{
  printf("memory: read 0x%08x\n", address);
  if (data)
    *data = (data_t) 0;
  
  instr_count++;
}

void
memory_write(unsigned int address, data_t *data)
{
  printf("memory: write 0x%08x\n", address);
  
  instr_count++;
}

void
memory_finish(void)
{
  fprintf(stdout, "Executed %lu instructions.\n\n", instr_count);
  
  /* Deinitialize memory subsystem here */
}
