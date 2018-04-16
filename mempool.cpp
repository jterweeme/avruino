#include "mempool.h"

//#define NULL 0

static const uint8_t POOLOFFSET = 1;

struct memblock MemoryPool::blocks[MEMPOOL_NUM_MEMBLOCKS+1];

static inline void *myMemset(void *s, int c, size_t n)
{
    uint8_t *p = (uint8_t *)s;

    while (n--)
        *p++ = (uint8_t)c;

    return s;
}

void MemoryPool::init()
{
    myMemset(&blocks[0], 0, sizeof(blocks));
    blocks[POOLSTART].begin = MEMPOOL_STARTADDRESS;
    blocks[POOLSTART].size = 0;
    blocks[POOLSTART].nextblock = NOBLOCK;
}

memhandle MemoryPool::allocBlock(memaddress size)
{
    memblock* best = 0;
    memhandle cur = POOLSTART;
    memblock* block = &blocks[POOLSTART];
    memaddress bestsize = MEMPOOL_SIZE + 1;

    do
    {
      memhandle next = block->nextblock;
      memaddress freesize = (next == NOBLOCK ? blocks[POOLSTART].begin + MEMPOOL_SIZE : blocks[next].begin) - block->begin - block->size;
      if (freesize == size)
        {
          best = &blocks[cur];
          goto found;
        }
      if (freesize > size && freesize < bestsize)
        {
          bestsize = freesize;
          best = &blocks[cur];
        }
      if (next == NOBLOCK)
        {
          if (best)
            goto found;
          else
            goto collect;
        }
      block = &blocks[next];
      cur = next;
    }
  while (true);
  collect:
    {
      cur = POOLSTART;
      block = &blocks[POOLSTART];
      memhandle next;
      while ((next = block->nextblock) != NOBLOCK)
        {
          memaddress dest = block->begin + block->size;
          memblock* nextblock = &blocks[next];
          memaddress* src = &nextblock->begin;
          if (dest != *src)
            {
#ifdef MEMPOOL_MEMBLOCK_MV
              MEMPOOL_MEMBLOCK_MV(dest,*src,nextblock->size);
#endif
              *src = dest;
            }
          block = nextblock;
        }
      if (blocks[POOLSTART].begin + MEMPOOL_SIZE - block->begin - block->size >= size)
        best = block;
      else
        goto notfound;
    }
found:
    {
      block = &blocks[POOLOFFSET];
      for (cur = POOLOFFSET; cur < MEMPOOL_NUM_MEMBLOCKS + POOLOFFSET; cur++)
        {
          if (block->size)
            {
              block++;
              continue;
            }
          memaddress address = best->begin + best->size;
#ifdef MEMBLOCK_ALLOC
          MEMBLOCK_ALLOC(address,size);
#endif
          block->begin = address;
          block->size = size;
          block->nextblock = best->nextblock;
          best->nextblock = cur;
          return cur;
        }
    }

notfound:
    return NOBLOCK;
}

void MemoryPool::freeBlock(memhandle handle)
{
    if (handle == NOBLOCK)
        return;

    memblock *b = &blocks[POOLSTART];

    do
    {
      memhandle next = b->nextblock;
      if (next == handle)
        {
          memblock *f = &blocks[next];
#ifdef MEMBLOCK_FREE
          MEMBLOCK_FREE(f->begin,f->size);
#endif
          b->nextblock = f->nextblock;
          f->size = 0;
          f->nextblock = NOBLOCK;
          return;
        }
      if (next == NOBLOCK)
        return;
      b = &blocks[next];
    }
  while (true);
}

void MemoryPool::resizeBlock(memhandle handle, memaddress position)
{
    memblock * block = &blocks[handle];
    block->begin += position;
    block->size -= position;
}

void MemoryPool::resizeBlock(memhandle handle, memaddress position, memaddress size)
{
    memblock *block = &blocks[handle];
    block->begin += position;
    block->size = size;
}

memaddress MemoryPool::blockSize(memhandle handle)
{   
    return blocks[handle].size;
}

