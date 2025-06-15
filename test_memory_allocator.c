/*
 * test_memory_allocator.c
 * Test program for custom memory allocator
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "custom_memory_allocator.h"

void test_memory_allocator(void) {
    // Create a memory pool
    struct memory_block *pool = create_memory_pool(150);
    assert(pool != NULL);
    visualize_pool(pool);

    // Test 1: Basic allocation and string storage
    char *str = pool_alloc(pool, 12);
    assert(str != NULL);
    strcpy(str, "Hello World");
    visualize_pool(pool);
    assert(!strcmp(str, "Hello World"));

    // Test 2: Multiple allocations
    char *buffer = pool_alloc(pool, 20);
    assert(buffer != NULL);
    buffer[0] = '!';
    for (int i = 1; i < 20; i++) {
        buffer[i] = 'X';
    }
    visualize_pool(pool);
    assert(buffer[0] == '!');

    // Test 3: Free and reuse
    pool_free(str);
    str = NULL;
    visualize_pool(pool);

    // Test 4: Integer allocation
    int *num = pool_alloc(pool, sizeof(int));
    assert(num != NULL);
    *num = 0x00EEFFC0;
    visualize_pool(pool);
    assert(*num == 0x00EEFFC0);

    // Test 5: Zero-size allocation
    void *empty = pool_alloc(pool, 0);
    assert(empty != NULL);
    visualize_pool(pool);

    // Test 6: Allocation failure
    void *too_large = pool_alloc(pool, 1000);
    assert(too_large == NULL);
    visualize_pool(pool);

    // Test 7: Memory statistics
    size_t total, used, allocations;
    get_pool_stats(pool, &total, &used, &allocations);
    assert(total == 150);
    assert(used > 0);
    assert(allocations == 3);  // buffer, num, and empty

    // Test 8: Free all allocations
    pool_free(buffer);
    pool_free(num);
    pool_free(empty);
    visualize_pool(pool);

    // Test 9: Cleanup
    printf("Cleaning up memory pool...\n");
    destroy_pool(pool);
}

int main(void) {
    test_memory_allocator();
    printf("All tests passed!\n");
    return 0;
}
