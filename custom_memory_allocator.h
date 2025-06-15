/*
 * custom_memory_allocator.h
 * Header file for custom memory allocator implementation
 */

#ifndef CUSTOM_MEMORY_ALLOCATOR_H
#define CUSTOM_MEMORY_ALLOCATOR_H

#include <stddef.h>

// Memory block structure (opaque to users)
struct memory_block;

// Create a new memory pool of specified size
// Returns NULL if size is too small or allocation fails
struct memory_block *create_memory_pool(size_t size);

// Allocate memory from the pool
// Returns NULL if allocation fails
void *pool_alloc(struct memory_block *pool, size_t size);

// Free previously allocated memory
void pool_free(void *ptr);

// Get memory pool statistics
void get_pool_stats(struct memory_block *pool, 
                   size_t *total_size,
                   size_t *used_memory,
                   size_t *num_allocations);

// Visualize memory pool state
void visualize_pool(struct memory_block *pool);

// Clean up memory pool
void destroy_pool(struct memory_block *pool);

#endif // CUSTOM_MEMORY_ALLOCATOR_H
