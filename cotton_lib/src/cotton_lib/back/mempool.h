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

#pragma once
#include <cstddef>
#include <cstdlib>

namespace Cotton{
/**
 * A chunk within a larger block.
 */
struct Chunk {
    /**
     * When a chunk is free, the `next` contains the
     * address of the next chunk in a list.
     *
     * When it's allocated, this space is used by
     * the user.
     */
    Chunk *next;
};

/**
 * The allocator class.
 *
 * Features:
 *
 *   - Parametrized by number of chunks per block
 *   - Keeps track of the allocation pointer
 *   - Bump-allocates chunks
 *   - Requests a new larger block when needed
 *
 */
class PoolAllocator {
public:
    PoolAllocator(size_t chunksPerBlock);

    void *allocate(size_t size);
    void  deallocate(void *ptr, size_t size);

private:
    /**
     * Number of chunks per larger block.
     */
    size_t mChunksPerBlock;

    /**
     * Allocation pointer.
     */
    Chunk *mAlloc = nullptr;

    /**
     * Allocates a larger block (pool) for chunks.
     */
    Chunk *allocateBlock(size_t chunkSize);
};

// -----------------------------------------------------------
}