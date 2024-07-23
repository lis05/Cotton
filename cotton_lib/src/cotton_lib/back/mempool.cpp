/**
 * Pool-allocator.
 *
 * Details: http://dmitrysoshnikov.com/compilers/writing-a-pool-allocator/
 *
 * Allocates a larger block using `malloc`.
 *
 * Splits the large block into smaller chunks
 * of equal size.
 *
 * Uses bump-allocated per chunk.
 *
 * by Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 * MIT Style License, 2019
 */

#include "mempool.h"

namespace Cotton {

/**
 * Allocates a new block from OS.
 *
 * Returns a Chunk pointer set to the beginning of the block.
 */
Chunk *PoolAllocator::allocateBlock(size_t chunkSize) {
    size_t blockSize = mChunksPerBlock * chunkSize;

    // The first chunk of the new block.
    Chunk *blockBegin = reinterpret_cast<Chunk *>(malloc(blockSize));

    // Once the block is allocated, we need to chain all
    // the chunks in this block:

    Chunk *chunk = blockBegin;

    for (int i = 0; i < mChunksPerBlock - 1; ++i) {
        chunk->next = reinterpret_cast<Chunk *>(reinterpret_cast<char *>(chunk) + chunkSize);
        chunk       = chunk->next;
    }

    chunk->next = nullptr;

    return blockBegin;
}

/**
 * Returns the first free chunk in the block.
 *
 * If there are no chunks left in the block,
 * allocates a new block.
 */
PoolAllocator::PoolAllocator(size_t chunksPerBlock) {
    mChunksPerBlock = chunksPerBlock;
}

void *PoolAllocator::allocate(size_t size) {
    return malloc(size);
}

/**
 * Puts the chunk into the front of the chunks list.
 */
void PoolAllocator::deallocate(void *chunk, size_t size) {
    free(chunk);
}

}    // namespace Cotton
