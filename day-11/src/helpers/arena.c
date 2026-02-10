#include "./arena.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_power_of_2(int x) { return (x != 0) && ((x & (x - 1)) == 0); }

Arena *arena_init(size_t num_bytes) {

    Arena *arena = malloc(sizeof(*arena));
    if (arena == NULL) {
        printf("Error while trying to allocate the arena struct\n");
        exit(EXIT_FAILURE);
    }

    arena->buffer = malloc(num_bytes);
    if (arena->buffer == NULL) {
        printf("Error while trying to allocate %lu bytes for the buffer\n", num_bytes);
        exit(EXIT_FAILURE);
    }

    arena->len_buffer = num_bytes;
    arena->current_offset = 0;
    return arena;
}

void *arena_alloc_align(Arena *arena, size_t size, uint32_t alignment) {
    void *allocated_ptr;

    // check that alignment is correct
    assert(is_power_of_2(alignment));

    // calculate next adress and align it
    uintptr_t current_alloc_addr = (uintptr_t)(arena->buffer) + (uintptr_t)arena->current_offset;
    if (current_alloc_addr % alignment != 0) {
        current_alloc_addr += (uintptr_t)alignment - current_alloc_addr % alignment;
    }
    uintptr_t new_offset = current_alloc_addr - (uintptr_t)(arena->buffer) + size;

    // check that we have enough memory left and assign
    if (new_offset < arena->len_buffer) {
        arena->current_offset = (size_t)new_offset;
        allocated_ptr = (void *)current_alloc_addr;

        // set the memory to 0 for the allocated block
        memset(allocated_ptr, 0, size);
        return allocated_ptr;
    }
    return NULL;
}

void arena_reset(Arena *arena) { arena->current_offset = 0; }

void free_arena(Arena *arena) {
    free(arena->buffer);
    free(arena);
}
