/*
 * custom_memory_allocator.c
 * A custom memory allocator that manages a contiguous block of memory
 * with support for dynamic allocation and deallocation.
 * 
 * Features:
 * - Single malloc call for the entire memory pool
 * - Efficient gap management
 * - Memory alignment support
 * - Memory usage statistics
 * - Debug visualization
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

// Memory block header structure
struct memory_block {
    struct memory_node *first;    // First allocated block
    void *upper_limit;           // End of memory pool
    size_t total_size;           // Total size of memory pool
    size_t used_memory;          // Currently used memory
    size_t num_allocations;      // Number of active allocations
};

// Memory node structure for tracking allocations
struct memory_node {
    size_t size;                 // Size of allocated memory
    bool is_used;               // Whether this block is in use
    struct memory_node *prev;    // Previous node
    struct memory_node *next;    // Next node
    struct memory_block *pool;   // Reference to memory pool
};

// Constants for memory management
#define MIN_ALIGNMENT 8
#define MIN_BLOCK_SIZE (sizeof(struct memory_node) + MIN_ALIGNMENT)

// Create a new memory pool of specified size
struct memory_block *create_memory_pool(size_t size) {
    if (size < MIN_BLOCK_SIZE) {
        return NULL;
    }

    struct memory_block *pool = malloc(size);
    if (!pool) {
        return NULL;
    }

    // Initialize pool
    pool->first = NULL;
    pool->upper_limit = (void*)pool + size;
    pool->total_size = size;
    pool->used_memory = 0;
    pool->num_allocations = 0;

    // Initialize memory with pattern for debugging
    memset((void*)pool + sizeof(struct memory_block), 0xCC, 
           size - sizeof(struct memory_block));

    return pool;
}

// Align size to MIN_ALIGNMENT
static size_t align_size(size_t size) {
    return (size + (MIN_ALIGNMENT - 1)) & ~(MIN_ALIGNMENT - 1);
}

// Allocate memory from the pool
void *pool_alloc(struct memory_block *pool, size_t size) {
    assert(pool);
    
    // Align requested size
    size_t aligned_size = align_size(size);
    if (aligned_size == 0) {
        return NULL;
    }

    // Check if we have enough space
    if (aligned_size + sizeof(struct memory_node) > 
        pool->total_size - pool->used_memory) {
        return NULL;
    }

    void *pool_start = (void*)pool + sizeof(struct memory_block);
    
    // If pool is empty, allocate at start
    if (!pool->first) {
        struct memory_node *node = pool_start;
        node->size = aligned_size;
        node->is_used = true;
        node->prev = NULL;
        node->next = NULL;
        node->pool = pool;
        
        pool->first = node;
        pool->used_memory += aligned_size + sizeof(struct memory_node);
        pool->num_allocations++;
        
        return (void*)node + sizeof(struct memory_node);
    }

    // Check for space at the beginning
    size_t initial_gap = (void*)pool->first - pool_start;
    if (initial_gap >= aligned_size + sizeof(struct memory_node)) {
        struct memory_node *node = pool_start;
        node->size = aligned_size;
        node->is_used = true;
        node->prev = NULL;
        node->next = pool->first;
        node->pool = pool;
        
        pool->first->prev = node;
        pool->first = node;
        pool->used_memory += aligned_size + sizeof(struct memory_node);
        pool->num_allocations++;
        
        return (void*)node + sizeof(struct memory_node);
    }

    // Check for gaps between nodes
    struct memory_node *current = pool->first;
    while (current) {
        void *current_end = (void*)current + sizeof(struct memory_node) + current->size;
        size_t gap_size;
        
        if (current->next) {
            gap_size = (void*)current->next - current_end;
        } else {
            gap_size = pool->upper_limit - current_end;
        }

        if (gap_size >= aligned_size + sizeof(struct memory_node)) {
            struct memory_node *node = current_end;
            node->size = aligned_size;
            node->is_used = true;
            node->prev = current;
            node->next = current->next;
            node->pool = pool;
            
            if (current->next) {
                current->next->prev = node;
            }
            current->next = node;
            
            pool->used_memory += aligned_size + sizeof(struct memory_node);
            pool->num_allocations++;
            
            return (void*)node + sizeof(struct memory_node);
        }
        
        current = current->next;
    }
    
    return NULL;
}

// Free allocated memory
void pool_free(void *ptr) {
    if (!ptr) return;
    
    struct memory_node *node = (void*)ptr - sizeof(struct memory_node);
    assert(node->is_used);
    
    // Update pool statistics
    node->pool->used_memory -= node->size + sizeof(struct memory_node);
    node->pool->num_allocations--;
    
    // Mark as free
    node->is_used = false;
    
    // Update linked list
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        node->pool->first = node->next;
    }
    
    if (node->next) {
        node->next->prev = node->prev;
    }
}

// Get memory pool statistics
void get_pool_stats(struct memory_block *pool, 
                   size_t *total_size,
                   size_t *used_memory,
                   size_t *num_allocations) {
    assert(pool);
    if (total_size) *total_size = pool->total_size;
    if (used_memory) *used_memory = pool->used_memory;
    if (num_allocations) *num_allocations = pool->num_allocations;
}

// Visualize memory pool state
void visualize_pool(struct memory_block *pool) {
    assert(pool);
    printf("\nMemory Pool Visualization:\n");
    printf("Total Size: %zu bytes\n", pool->total_size);
    printf("Used Memory: %zu bytes\n", pool->used_memory);
    printf("Active Allocations: %zu\n", pool->num_allocations);
    printf("Memory Layout:\n");
    
    struct memory_node *node = pool->first;
    void *current = (void*)pool + sizeof(struct memory_block);
    
    while (node) {
        // Print gap if exists
        if ((void*)node > current) {
            printf("[GAP: %zu bytes]\n", (void*)node - current);
        }
        
        // Print node
        printf("[%s: %zu bytes]\n", 
               node->is_used ? "USED" : "FREE",
               node->size + sizeof(struct memory_node));
        
        current = (void*)node + sizeof(struct memory_node) + node->size;
        node = node->next;
    }
    
    // Print final gap if exists
    if (current < pool->upper_limit) {
        printf("[GAP: %zu bytes]\n", pool->upper_limit - current);
    }
    printf("\n");
}

// Clean up memory pool
void destroy_pool(struct memory_block *pool) {
    assert(pool);
    if (pool->num_allocations > 0) {
        printf("Warning: Destroying pool with %zu active allocations!\n", 
               pool->num_allocations);
    }
    free(pool);
}


