#ifndef MY_ARENA_H
#define MY_ARENA_H

#include <stdint.h>
#include <stdlib.h>

typedef struct Arena {
    void *buffer;
    size_t len_buffer;
    uintptr_t current_offset;
} Arena;

Arena *arena_init(size_t num_bytes);
void *arena_alloc_align(Arena *arena, size_t size, uint32_t alignment);
void arena_reset(Arena *arena);
void free_arena(Arena *arena);
#endif
