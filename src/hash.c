/*
  _               _
 | |             | |
 | |__   __ _ ___| |__    ___
 | '_ \ / _` / __| '_ \  / __|
 | | | | (_| \__ \ | | || (__
 |_| |_|\__,_|___/_| |_(_)___|

 hash.c: Hash table implementation

 */


#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"


#define HT_PRIME_ONE 1741
#define HT_PRIME_TWO 3539

static ht_item_T* hti_new     (const char* k, const char* v);
static void       hti_del     (ht_item_T* i);
static size_t     ht_hash     (const char* s, const long prime, const size_t b_len);
static size_t     ht_get_hidx (const char* s, const size_t b_len, unsigned int attempt);

ht_index_T* ht_new();


struct ht_item_S
{
    char* key;
    char* val;
};


struct ht_index_S
{
    uint32_t size;
    size_t counter;

    ht_item_T** items;
};


static ht_item_T HT_DELETED_ITEM = {NULL, NULL};


char*
dupstr(const char* s)
{
    size_t len = (strlen(s) + 1);
    char* p = malloc(len);

    return p ? memcpy(p, s, len) : NULL;
}


static size_t
ht_hash(const char* s, const long prime, const size_t b_len)
{
    long hash = 0;
    const size_t v_len = strlen(s);

    for(size_t i = 0; i < v_len; i++)
    {
        hash += (long)pow(prime, v_len - (i + 1)) * s[i];
        hash %= b_len;
    }

    return (size_t)hash;
}


static size_t
ht_get_hidx(const char* s, const size_t b_len, unsigned int attempt)
{
    const size_t ha = ht_hash(s, HT_PRIME_ONE, b_len);
    const size_t hb = ht_hash(s, HT_PRIME_TWO, b_len);

    return (ha + (attempt * (hb + 1))) % b_len;
}


static ht_item_T*
hti_new(const char* k, const char* v)
{
    ht_item_T* i = malloc(sizeof(struct ht_item_S));
    i->key = dupstr(k);
    i->val = dupstr(v);

    return i;
}


static void
hti_del(ht_item_T* i)
{
    free(i->key);
    free(i->val);
    free(i);
}


ht_index_T*
ht_new()
{
    ht_index_T* ht = malloc(sizeof(struct ht_index_S));

    ht->size    = 53;
    ht->counter = 0;
    ht->items   = calloc((size_t)ht->size, sizeof(ht_item_T*));

    return ht;
}


void
ht_destroy(ht_index_T* t)
{
    for(size_t i = 0; i < t->size; i++)
    {
        ht_item_T* item = t->items[i];
        if(item != NULL)
            hti_del(item);
    }

    free(t->items);
    free(t);
}


void
ht_insert(ht_index_T* t, const char* k, const char* v)
{
    ht_item_T* i = hti_new(k, v);

    size_t idx = ht_get_hidx(i->key, t->size, 0);
    ht_item_T* cur_i = t->items[idx];

    size_t iter = 1;
    while(cur_i != NULL)
    {
        if(cur_i != &HT_DELETED_ITEM && strcmp(cur_i->key, k) == 0)
        {
            hti_del(cur_i);
            t->items[idx] = i;
            return;
        }

        idx = ht_get_hidx(i->key, t->size, iter);
        cur_i = t->items[idx];
        iter++;
    }

    t->items[idx] = i;
    t->counter++;
}


char*
ht_search(ht_index_T* t, const char* k)
{
    size_t idx = ht_get_hidx(k, t->size, 0);

    ht_item_T* i = t->items[idx];

    size_t iter = 1;
    while(i != NULL)
    {
        if(i != &HT_DELETED_ITEM && strcmp(i->key, k) == 0)
            return i->val;

        idx = ht_get_hidx(k, t->size, iter);
        i = t->items[idx];
        iter++;
    }

    return NULL;
}


void
ht_delete(ht_index_T* t, const char* k)
{
    size_t idx = ht_get_hidx(k, t->size, 0);

    ht_item_T* i = t->items[idx];

    size_t iter = 1;
    while(i != NULL)
    {
        if(i != &HT_DELETED_ITEM)
        {
            if(strcmp(i->key, k) == 0)
            {
                hti_del(i);
                t->items[idx] = &HT_DELETED_ITEM;
            }
        }

        idx = ht_get_hidx(k, t->size, iter);
        i = t->items[idx];
        i++;
    }

    t->counter--;
}
