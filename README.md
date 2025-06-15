# Custom Memory Allocator – Advanced Memory Management System

This project implements a sophisticated custom memory management system in C, providing a robust alternative to standard malloc and free. The allocator operates on a single contiguous block of memory, featuring advanced memory tracking, alignment, and visualization capabilities.

## 📦 Features

- **Memory Pool Management**: Creates and manages a fixed-size memory pool with efficient allocation strategies
- **Memory Alignment**: Ensures proper memory alignment for optimal performance
- **Advanced Memory Tracking**: 
  - Tracks total memory usage
  - Monitors number of active allocations
  - Provides detailed memory statistics
- **Fragmentation Handling**: 
  - Efficient gap management
  - Smart placement of new allocations
  - Memory reuse optimization
- **Debugging & Visualization**: 
  - Detailed memory layout visualization
  - Memory usage statistics
  - Pattern initialization for debugging
  - Gap size reporting

## 🧠 Core Concepts

- **Memory Pool**: A contiguous block of memory managed by the allocator
- **Memory Nodes**: Metadata blocks tracking allocated memory chunks
- **Memory Statistics**: Real-time tracking of memory usage and allocations
- **Memory Visualization**: ASCII-based visualization of memory layout

## 🔧 API

```c
// Create a new memory pool
struct memory_block *create_memory_pool(size_t size);

// Allocate memory from the pool
void *pool_alloc(struct memory_block *pool, size_t size);

// Free allocated memory
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
```

## 📌 Implementation Details

- **Memory Safety**: 
  - Proper NULL pointer handling
  - Size validation
  - Memory alignment enforcement
  - Bounds checking
- **Performance Optimizations**:
  - Efficient gap management
  - Smart allocation placement
  - Minimal metadata overhead
- **Debug Features**:
  - Memory pattern initialization (0xCC)
  - Detailed visualization
  - Usage statistics
  - Warning for non-empty pool destruction

## 🚀 Usage Example

```c
// Create a memory pool
struct memory_block *pool = create_memory_pool(1024);

// Allocate memory
char *str = pool_alloc(pool, 100);
strcpy(str, "Hello, Memory Pool!");

// Get statistics
size_t total, used, allocations;
get_pool_stats(pool, &total, &used, &allocations);

// Visualize memory layout
visualize_pool(pool);

// Free memory
pool_free(str);

// Cleanup
destroy_pool(pool);
```

## 🔍 Memory Layout

The allocator maintains a doubly-linked list of memory nodes, each containing:
- Size of allocated memory
- Usage status
- Previous and next node pointers
- Reference to the memory pool

Memory is allocated in aligned chunks, with metadata stored in a header before each allocation.

## ⚠️ Notes

- Memory pool size must be at least `MIN_BLOCK_SIZE` (16 bytes)
- Allocations are aligned to 8 bytes for optimal performance
- Memory patterns are initialized to 0xCC for debugging
- The system provides warnings when destroying non-empty pools