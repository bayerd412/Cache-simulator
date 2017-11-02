/** @file memory.c
 *  @brief Implements starting point for a memory hierarchy with caching and RAM.
 *  @see memory.h
 */

#include "memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

static unsigned long instr_count;
static unsigned long int current_set;
static unsigned long int current_tag;
static unsigned long int current_offset;

struct block;
typedef struct block block_t;
struct block{
  unsigned int tag;
  unsigned int age;
  unsigned int data;
  unsigned int dirty_bit;
  unsigned int valid_bit;
  
};

struct cache;
typedef struct cache cache_t;
struct cache {
  unsigned int cache_size;
  unsigned int cache_assoc;
  unsigned int rep_policy; //LRU = 0
  unsigned int block_size;
  unsigned int bus_width;
  unsigned int write_policy; // Write-Back = 0

  long int read_hit;
  long int read_miss;
  long int write_hit;
  long int write_miss;

  unsigned int num_sets;
  int block_num;

  block_t *blocks[];
};

static cache_t* L1I_Cache;
  //L1I_Cache = malloc(sizeof(cache_t));
static cache_t* L1D_Cache;
  //L1D_Cache = malloc(sizeof(cache_t));
static cache_t* L2_Cache;
  //L2_Cache = malloc(sizeof(cache_t));
  

//Some new.

//Create a new cache.
void
create_cache(cache_t *cache, unsigned int size, unsigned int assoc, unsigned int rp, unsigned int block_size, unsigned int bus, unsigned int wp);

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

int find_log(unsigned int x);


void
memory_init(void)
{
  instr_count = 0;
  
  L1I_Cache = malloc(sizeof(cache_t));
  L1D_Cache = malloc(sizeof(cache_t));
  L2_Cache = malloc(sizeof(cache_t));

  create_cache(L1I_Cache, 32768, 4, 0, 64, 64, 0);
  create_cache(L1D_Cache, 32768, 8, 0, 64, 64, 0);
  create_cache(L2_Cache, 262144, 8, 0, 64, 64, 0);


  
}

void
memory_fetch(unsigned int address, data_t *data)
{
  printf("memory: fetch 0x%08x\n", address);
  calc_address(L1I_Cache, address);
  int res1 = search_block(L1I_Cache);
  printf("%d\n", res1);
  return;
  //Searching Block is in L1I Cache.
  if (res1 != -1)
  {
    L1I_Cache->read_hit++;
    L1I_Cache->blocks[res1]->age = 0;
  }
  else // Searching Block doesnot exist in L1I Cache.
  {
    L1I_Cache->read_miss++;
    calc_address(L2_Cache, address);
    int res2 = search_block(L2_Cache);
    if (res2 != -1)
    {
      L2_Cache->read_hit;
      res1 = search_empty_block(L1I_Cache);
      if (res1 != -1)
      {
        change_block(L1I_Cache, res1, res2);
        L1I_Cache->blocks[res1]->age = 0;
      }
      else 
      {
        res1 = LRU_search(L1I_Cache);
        change_block(L1I_Cache, res1, res2);
        L1I_Cache->blocks[res1]->age = 0;
      }
    }
    else
    {
      L2_Cache->read_miss++;
      res1 = search_empty_block(L1I_Cache);
      if (res1 != -1)
      {
        L1I_Cache->blocks[res1]->tag = current_tag;
        L1I_Cache->blocks[res1]->age = 0;
        L1I_Cache->blocks[res1]->valid_bit = 1;
        L1I_Cache->blocks[res1]->dirty_bit = 0;
      }
      else
      {
        res1 = LRU_search(L1I_Cache);
        res2 = search_empty_block(L2_Cache);
        if (res2 != -1)
        {
          L2_Cache->blocks[res2]->tag = current_tag;
          L2_Cache->blocks[res2]->age = 0;
          L2_Cache->blocks[res2]->valid_bit = 1;
          L2_Cache->blocks[res2]->dirty_bit = 0;
          change_block(L1I_Cache, res1, res2);
        }
        else
        {
          res2 = LRU_search(L2_Cache);
          change_block(L1I_Cache, res1, res2);
          L1I_Cache->blocks[res1]->tag = current_tag;
          L1I_Cache->blocks[res1]->age = 0;
          L1I_Cache->blocks[res1]->valid_bit = 1;
          L1I_Cache->blocks[res1]->dirty_bit = 0;
          
        }
      }
    }
  }
  aging();
  instr_count++;
}

void
memory_read(unsigned int address, data_t *data)
{
  printf("memory: read 0x%08x\n", address);
  
  calc_address(L1D_Cache, address);
  int res1 = search_block(L1D_Cache);
  
  //Searching Block is in L1D Cache.
  if (res1 != -1)
  {
    L1D_Cache->read_hit++;
    L1D_Cache->blocks[res1]->age = 0;
  }
  else // Searching Block doesnot exist in L1I Cache.
  {
    L1D_Cache->read_miss++;
    calc_address(L2_Cache, address);
    int res2 = search_block(L2_Cache);
    if (res2 != -1)
    {
      L2_Cache->read_hit;
      res1 = search_empty_block(L1D_Cache);
      if (res1 != -1)
      {
        change_block(L1D_Cache, res1, res2);
        L1D_Cache->blocks[res1]->age = 0;
      }
      else 
      {
        res1 = LRU_search(L1D_Cache);
        change_block(L1D_Cache, res1, res2);
        L1D_Cache->blocks[res1]->age = 0;
      }
    }
    else
    {
      L2_Cache->read_miss++;
      res1 = search_empty_block(L1D_Cache);
      if (res1 != -1)
      {
        L1D_Cache->blocks[res1]->tag = current_tag;
        L1D_Cache->blocks[res1]->age = 0;
        L1D_Cache->blocks[res1]->valid_bit = 1;
        L1D_Cache->blocks[res1]->dirty_bit = 0;
      }
      else
      {
        res1 = LRU_search(L1D_Cache);
        res2 = search_empty_block(L2_Cache);
        if (res2 != -1)
        {
          L2_Cache->blocks[res2]->tag = current_tag;
          L2_Cache->blocks[res2]->age = 0;
          L2_Cache->blocks[res2]->valid_bit = 1;
          L2_Cache->blocks[res2]->dirty_bit = 0;
          change_block(L1D_Cache, res1, res2);
        }
        else
        {
          res2 = LRU_search(L2_Cache);
          change_block(L1D_Cache, res1, res2);
          L1D_Cache->blocks[res1]->tag = current_tag;
          L1D_Cache->blocks[res1]->age = 0;
          L1D_Cache->blocks[res1]->valid_bit = 1;
          L1D_Cache->blocks[res1]->dirty_bit = 0;
          
        }
      }
    }
  }

  aging();
  instr_count++;
}

void
memory_write(unsigned int address, data_t *data)
{
  printf("memory: write 0x%08x\n", address);
  
  calc_address(L1D_Cache, address);
  int res1 = search_block(L1D_Cache);

  if (res1 != -1)
  {
    L1D_Cache->write_hit++;
    L1D_Cache->blocks[res1]->dirty_bit = 1;
    L1D_Cache->blocks[res1]->age = 0;
  }
  else
  {
    L1D_Cache->write_miss++;
    int res2 = search_block(L2_Cache);
    if (res2 != -1)
    {
      L2_Cache->write_hit++;
      res1 = search_empty_block(L1D_Cache);
      if (res1 != -1)
      {
        change_block(L1D_Cache, res1, res2);
        L1D_Cache->blocks[res1]->dirty_bit = 1;
        L1D_Cache->blocks[res1]->age = 0;
      }
      else
      {
        res1 = LRU_search(L1D_Cache);
        if (L1D_Cache->blocks[res1]->dirty_bit == 1)
          L1D_Cache->blocks[res1]->dirty_bit = 0;
        change_block(L1D_Cache, res1, res2);
        L1D_Cache->blocks[res1]->dirty_bit = 1;
        L1D_Cache->blocks[res1]->age = 0;
      }
    }
    else
    {
      L2_Cache->write_miss++;
      res1 = search_empty_block(L1D_Cache);
      if (res1 != -1)
      {
        L1D_Cache->blocks[res1]->tag = current_tag;
        L1D_Cache->blocks[res1]->age = 0;
        L1D_Cache->blocks[res1]->valid_bit = 1;
        L1D_Cache->blocks[res1]->dirty_bit = 0;
      }
      else
      {
        res1 = LRU_search(L1D_Cache);
        if (L1D_Cache->blocks[res1]->dirty_bit == 1)
          L1D_Cache->blocks[res1]->dirty_bit = 0;
        res2 = search_empty_block(L2_Cache);
        if (res2 != -1)
        {
          change_block(L1D_Cache, res1, res2);
          L1D_Cache->blocks[res1]->tag = current_tag;
          L1D_Cache->blocks[res1]->age = 0;
          L1D_Cache->blocks[res1]->valid_bit = 1;
          L1D_Cache->blocks[res1]->dirty_bit = 0;
        }
        else
        {
          res2 = LRU_search(L2_Cache);
          change_block(L1D_Cache, res1, res2);
          L1D_Cache->blocks[res1]->tag = current_tag;
          L1D_Cache->blocks[res1]->age = 0;
          L1D_Cache->blocks[res1]->valid_bit = 1;
          L1D_Cache->blocks[res1]->dirty_bit = 0;
        }
      }
    }
  }


  instr_count++;
}

void
memory_finish(void)
{
  fprintf(stdout, "Executed %lu instructions.\n\n", instr_count);
  
  printf("L1 Instruction Cache read hits: %ld\n", L1I_Cache->read_hit);
  printf("L1 Instruction Cache read misses: %ld\n", L1I_Cache->read_miss);

  printf("L1 Data Cache read hits: %ld\n", L1D_Cache->read_hit);
  printf("L1 Data Cache read misses: %ld\n", L1D_Cache->read_miss);
  printf("L1 Data Cache write hits: %ld\n", L1D_Cache->write_hit);
  printf("L1 Data Cache write misses: %ld\n", L1D_Cache->write_miss);

  printf("L2 Cache read hits: %ld\n", L2_Cache->read_hit);
  printf("L2 Cache read misses: %ld\n", L2_Cache->read_miss);
  printf("L2 Cache write hits: %ld\n", L2_Cache->write_hit);
  printf("L2 Cache write misses: %ld\n", L2_Cache->write_miss);

  free(L1I_Cache);
  free(L1D_Cache);
  free(L2_Cache);
  printf("Everything is OK\n");
}

void
create_cache(cache_t *Cache, unsigned int size, unsigned int assoc, unsigned int rp, unsigned int block_size, unsigned int bus, unsigned int wp)
{
  Cache->cache_size = size;
  Cache->cache_assoc = assoc;
  Cache->rep_policy = rp;
  Cache->block_size = block_size;
  Cache->bus_width = bus;
  Cache->write_policy = wp;
  
  Cache->num_sets = Cache->cache_size / (Cache->cache_assoc * Cache->block_size);

  if (Cache->num_sets == 0 || (Cache->num_sets % 2 != 0)){
    printf("\nNumber of sets in one of the Cache is not a power of two!\n");
    return;
  }

  unsigned int tag_entry = Cache->num_sets * Cache->cache_assoc;

  Cache->block_num = Cache->cache_size / Cache->block_size;
  Cache->blocks[Cache->block_num] = malloc(sizeof(block_t)*Cache->block_num);
  Cache->blocks[0]->tag = 0;
  for (int i = 0; i < Cache->block_num; i++)
  {
    /*Cache->blocks[i]->tag = 0;
    Cache->blocks[i]->age = 0;
    Cache->blocks[i]->data = 0;
    Cache->blocks[i]->dirty_bit = 0;
    Cache->blocks[i]->valid_bit = 0;
  */}

  Cache->read_hit = 0;
  Cache->read_miss = 0;
  Cache->write_hit = 0; 
  Cache->write_miss = 0;
}

void calc_address(cache_t *Cache, unsigned int address)
{
  int x, y;
  y = find_log(Cache->block_size); //size of the offset in bits.
  x = find_log(Cache->num_sets); //size of index in bits.
  unsigned long int tag = address>>(x+y);
  unsigned long int index = (address-(tag<<(x+y)))>>y;
  unsigned long int offset = address-(tag<<(x+y))-(index<<y);
  
  current_set = index;
  current_tag = tag;
  current_offset = offset;
}

int search_block(cache_t *Cache)
{
  int i = 0;
  int j = 0;
  if (Cache != NULL)
  {
    j = Cache->block_num / Cache->num_sets; //number of blocks in each set.
    if (Cache->blocks[(current_set * j) + i] == NULL)
      printf("%d\n", j);
    for (i = 0; i < j; i++)
    {
      if (Cache->blocks[(current_set * j) + i]->tag == current_tag && Cache->blocks[(current_set * j) + i]->valid_bit == 1)
        return ((current_set * j) + i);
    }
  }
  return -1;
}

int search_empty_block(cache_t *Cache)
{
  int i = 0;
  int j = 0;
  if (Cache != NULL)
  {
    j = Cache->block_num / Cache->num_sets;
    for (i = 0; i < j; i++)
    {
      if (Cache->blocks[(current_set * j) + i]->valid_bit == 0)
        return ((current_set * j) + i);
    }
  }
  return -1;
}

void change_block(cache_t *Cache, int L1, int L2)
{
  if (Cache->blocks[L1]->dirty_bit == 1 && Cache->write_policy == 0)
  {
    Cache->blocks[L1]->dirty_bit = 0;
    unsigned int temp = L2_Cache->blocks[L2]->tag;
    L2_Cache->blocks[L2]->tag = Cache->blocks[L1]->tag;
    Cache->blocks[L1]->tag = temp;

    temp = L2_Cache->blocks[L2]->valid_bit;
    L2_Cache->blocks[L2]->valid_bit = Cache->blocks[L1]->valid_bit;
    Cache->blocks[L1]->valid_bit = temp;
    
    temp = L2_Cache->blocks[L2]->age;
    L2_Cache->blocks[L2]->age = Cache->blocks[L1]->age;
    Cache->blocks[L1]->age = temp;
  }
  return;
}

int LRU_search(cache_t *Cache)
{
  unsigned int max_age;
  int num;
  int j = 0;
  int i = 0;
  if (Cache != NULL)
  {
    j = Cache->block_num / Cache->num_sets;
    for (i = 0; i < j; i++)
    {
      if (Cache->blocks[(current_set * j) + i]->age >= max_age && Cache->blocks[(current_set * j) + i]->valid_bit == 1)
        max_age = Cache->blocks[(current_set * j) + i]->age;
    }
    return ((current_set * j) + i);
  }
  return -1;
}

void aging()
{
  int i;
  for (i = 0; i < (L1I_Cache->block_num); i++)
  {
    if (L1I_Cache->blocks[i]->valid_bit == 1)
      L1I_Cache->blocks[i]->age++;
  }
  for (i = 0; i < (L1D_Cache->block_num); i++)
  {
    if (L1D_Cache->blocks[i]->valid_bit == 1)
      L1D_Cache->blocks[i]->age++;
  }
  for (i = 0; i < (L2_Cache->block_num); i++)
  {
    if (L2_Cache->blocks[i]->valid_bit == 1)
      L2_Cache->blocks[i]->age++;
  }
}

int find_log(unsigned int x)
{
  int result = 0;
  while(x>>=1) result++;
  return result;
}