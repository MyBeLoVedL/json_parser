#pragma once

#include "datatype.h"
#include <math.h>

typedef struct
{
    void *start;
    u32 size;
} area;

typedef struct
{
    void *k_start;
    u32 k_size;
    void *v_start;
    u32 v_size;
    u32 pass_count;
} pair;

typedef struct
{
    u32 count;
    u32 size;
    pair *arr_start;
    u8 *bitmap;
} hash_table;

area *get(hash_table *table, void *K, u32 k_size);

void put(hash_table *table, pair *p);

void del(hash_table *table, void *K, u32 k_size);

pair *new_pair(void *k_start, u32 k_size, void *v_start, u32 v_size);

void free_pair(pair *p);

hash_table *new_hash_table();
void free_hash_table(hash_table *ht);

u32 get_bitmap_size(hash_table *table);
