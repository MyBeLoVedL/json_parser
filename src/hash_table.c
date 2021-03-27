
#include "datatype.h"
#include "hash_table.h"
#include <stdlib.h>
#include <string.h>
#include "xxhash.h"
#include <math.h>
#include <stdio.h>
#include <math.h>

#define is_set(bitmap, index) ((1 << (index % 8)) & *(bitmap + (u32)(index / 8)))
#define set_bitmap(bitmap, index) (*(bitmap + (u32)(index / 8)) |= ((u8)1 << index % 8))
#define unset_bitmap(bitmap, index) (*(bitmap + (u32)(index / 8)) &= ((i8)(-2) << index % 8))

#define U32_MAX 0xffffffff

#define INIT_HASH_TABLE_SIZE 16

#define assign_pair(src, target)              \
    do                                        \
    {                                         \
        src->k_start = target->k_start;       \
        src->k_size = target->k_size;         \
        src->v_start = target->v_start;       \
        src->v_size = target->v_size;         \
        src->pass_count = target->pass_count; \
    } while (0)

static u32 dup_index = U32_MAX;

void ht_resize(hash_table *table, u32 desired_size);

u32 get_bitmap_size(hash_table *table)
{
    return (u32)(table->size / 8);
}

pair *new_pair(void *k_start, u32 k_size, void *v_start, u32 v_size)
{
    pair *p = malloc(sizeof(pair));
    p->k_start = malloc(k_size);
    memcpy(p->k_start, k_start, k_size);
    p->k_size = k_size;

    p->v_start = malloc(v_size);
    memcpy(p->v_start, v_start, v_size);
    p->v_size = v_size;

    p->pass_count = 0;
    return p;
}

void free_pair(pair *p)
{
    free(p->k_start);
    free(p->v_start);
}

hash_table *new_sized_hash_table(u32 init_size)
{
    hash_table *ht = malloc(sizeof(hash_table));
    ht->size = init_size;
    ht->count = 0;
    ht->arr_start = calloc(ht->size, sizeof(pair));
    ht->bitmap = calloc(ht->size >> 3, 1);
    return ht;
}

hash_table *new_hash_table()
{
    return new_sized_hash_table(INIT_HASH_TABLE_SIZE);
}
void free_hash_table(hash_table *ht)
{
    for (int i = 0; i < ht->size; i++)
        if (is_set(ht->bitmap, i))
        {
            free_pair(ht->arr_start + i);
        }
    free(ht->arr_start);
    free(ht->bitmap);
    free(ht);
}

u32 get_hash(pair *p, const u32 attempt, const u32 bucket_size)
{
    u32 hash_1 = XXH64(p->k_start, p->k_size, 7) % bucket_size;
    u32 hash_2 = XXH64(p->k_start, p->k_size, 11) % bucket_size;
    return (hash_1 + attempt * (hash_2 + 1)) % bucket_size;
}

void put(hash_table *table, pair *p)
{
    u32 load_factor = table->count * 100 / table->size;
    if (load_factor >= 75)
        ht_resize(table, table->size << 1);
    int attempt = 0;
    u32 index = get_hash(p, attempt, table->size);
    area *tmp_a;
    if ((tmp_a = get(table, p->k_start, p->k_size)) != NULL)
    {
        free(tmp_a);
        if (dup_index != U32_MAX)
        {
            (table->arr_start + dup_index)->v_start = p->v_start;
            (table->arr_start + dup_index)->v_size = p->v_size;
            dup_index = U32_MAX;
            return;
        }
        else
        {
            perror("error when update pair with duplicate hash value");
            exit(EXIT_FAILURE);
        }
    }
    while (is_set(table->bitmap, index))
    {
        attempt++;
        (table->arr_start + index)->pass_count++;
        index = get_hash(p, attempt, table->size);
    }
    // assign_pair((table->arr_start + index), (p));
    p->pass_count = 0;
    memcpy((void *)(table->arr_start + index), (void *)p, sizeof(pair));
    set_bitmap(table->bitmap, index);
    table->count++;
}

area *get(hash_table *table, void *K, u32 k_size)
{
    int attempt = 0;
    pair *tmp_pair = new_pair(K, k_size, NULL, 0);
    u32 index = get_hash(tmp_pair, attempt, table->size);
    while (is_set(table->bitmap, index) || (!is_set(table->bitmap, index) && (table->arr_start + index)->pass_count > 0))
    {
        attempt++;
        if (!is_set(table->bitmap, index))
        {
            index = get_hash(tmp_pair, attempt, table->size);
            continue;
        }
        pair *t = table->arr_start + index;
        if (k_size == t->k_size && !memcmp(K, t->k_start, k_size))
        {
            area *res = malloc(sizeof(area));
            res->start = t->v_start;
            res->size = t->v_size;
            free(tmp_pair);
            dup_index = index;
            return res;
        }
        index = get_hash(tmp_pair, attempt, table->size);
    }
    free(tmp_pair);
    return NULL;
}

void dec_count(hash_table *table, void *K, u32 k_size, u32 until_index)
{
    int attempt = 0;
    pair *tmp_pair = new_pair(K, k_size, NULL, 0);
    u32 index = get_hash(tmp_pair, attempt, table->size);
    while (index != until_index)
    {
        pair *t = table->arr_start + index;
        t->pass_count--;
        attempt++;
        index = get_hash(tmp_pair, attempt, table->size);
    }
}

void del(hash_table *table, void *K, u32 k_size)
{
    u32 load_factor = table->count * 100 / table->size;
    if (load_factor < 15)
        ht_resize(table, table->size >> 1);
    int attempt = 0;
    pair *tmp_pair = new_pair(K, k_size, NULL, 0);
    u32 index = get_hash(tmp_pair, attempt, table->size);
    while (is_set(table->bitmap, index) || (!is_set(table->bitmap, index) && (table->arr_start + index)->pass_count > 0))
    {
        if (!is_set(table->bitmap, index))
        {
            attempt++;
            index = get_hash(tmp_pair, attempt, table->size);
            continue;
        }
        pair *t = table->arr_start + index;
        if (k_size == t->k_size && !memcmp(K, t->k_start, k_size))
        {
            dec_count(table, K, k_size, index);
            unset_bitmap(table->bitmap, index);
            free(t->k_start);
            free(t->v_start);
            free(tmp_pair);
            table->count--;
            return;
        }
    }
    free(tmp_pair);
}

void ht_resize(hash_table *table, u32 desired_size)
{
    if (desired_size < INIT_HASH_TABLE_SIZE)
        return;
    hash_table *new_ht = new_sized_hash_table(desired_size);
    for (int i = 0; i < table->size; i++)
    {
        pair *p = table->arr_start + i;
        if (is_set(table->bitmap, i))
        {
            put(new_ht, p);
        }
    }
    table->size = new_ht->size;
    free(table->bitmap);
    table->bitmap = malloc(new_ht->size / 8);
    memcpy(table->bitmap, new_ht->bitmap, new_ht->size / 8);
    free(table->arr_start);
    table->arr_start = new_ht->arr_start;
    free(new_ht);
}
