/*

  _               _
 | |             | |
 | |__   __ _ ___| |__    ___
 | '_ \ / _` / __| '_ \  / __|
 | | | | (_| \__ \ | | || (__
 |_| |_|\__,_|___/_| |_(_)___|

 hash.c: Hash table implementation

 ---------------------------------------------------------------------

 Copyright 2018-2019 Caian R. Ertl <hi@caian.org>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */


#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "prime.h"


void               ht_delete      (ht_index_T* t, const char* k);
void               ht_destroy     (ht_index_T* t);
static size_t      ht_get_hidx    (const char* s, const size_t b_len, unsigned int attempt);
static size_t      ht_hash        (const char* s, const long prime, const size_t b_len);
void               ht_insert      (ht_index_T* t, const char* k, const char* v);
static int         ht_load_ratio  (const ht_index_T* ht);
static ht_item_T*  hti_new        (const char* k, const char* v);
static ht_index_T* ht_new_sized   (const uint32_t base_s);
static void        ht_resize      (ht_index_T* ht, const uint32_t base_s);
static void        ht_resize_down (ht_index_T* ht);
static void        ht_resize_up   (ht_index_T* ht);
char*              ht_search      (ht_index_T* t, const char* k);
static void        hti_del        (ht_item_T* i);
static ht_item_T*  hti_new        (const char* k, const char* v);


enum
{
    HT_INITIAL_BASE_SIZE = 53,
    HT_PRIME_NUMBER_A    = 1741,
    HT_PRIME_NUMBER_B    = 3539
};


struct ht_item_S
{
    char* key;
    char* val;
};


struct ht_index_S
{
    size_t counter;

    uint32_t size;
    uint32_t base_s;

    ht_item_T** items;
};


static ht_item_T HT_DELETED_ITEM = {NULL, NULL};


char* dupstr(const char* s)
{
    size_t len = (strlen(s) + 1);
    char* p = malloc(len);

    return p ? memcpy(p, s, len) : NULL;
}


static int ht_load_ratio(const ht_index_T* ht)
{
    return ((ht->counter * 100) / ht->size);
}


static size_t ht_hash(const char* s, const long prime, const size_t b_len)
{
    long hash = 0;
    const size_t v_len = strlen(s);

    for (size_t i = 0; i < v_len; i++)
    {
        hash += (long)pow(prime, v_len - (i + 1)) * s[i];
        hash %= b_len;
    }

    return (size_t)hash;
}


static size_t ht_get_hidx(const char* s, const size_t b_len, unsigned int attempt)
{
    const size_t ha = ht_hash(s, HT_PRIME_NUMBER_A, b_len);
    const size_t hb = ht_hash(s, HT_PRIME_NUMBER_B, b_len);

    return (ha + (attempt * (hb + 1))) % b_len;
}


static ht_item_T* hti_new(const char* k, const char* v)
{
    ht_item_T* i = malloc(sizeof(struct ht_item_S));
    i->key = dupstr(k);
    i->val = dupstr(v);

    return i;
}


static void hti_del(ht_item_T* i)
{
    free(i->key);
    free(i->val);
    free(i);
}


static void ht_resize(ht_index_T* ht, const uint32_t base_s)
{
    if (base_s < HT_INITIAL_BASE_SIZE)
        return;

    ht_index_T* nht = ht_new_sized(base_s);

    for (size_t i = 0; i < ht->size; i++)
    {
        ht_item_T* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM)
            ht_insert(nht, item->key, item->val);
    }

    ht->base_s  = nht->base_s;
    ht->counter = nht->counter;

    const uint32_t size_tmp = ht->size;
    ht->size  = nht->size;
    nht->size = size_tmp;

    ht_item_T** items_tmp = ht->items;
    ht->items  = nht->items;
    nht->items = items_tmp;

    ht_destroy(nht);
}


static void ht_resize_up(ht_index_T* ht)
{
    ht_resize(ht, (ht->base_s * 2));
}


static void ht_resize_down(ht_index_T* ht)
{
    ht_resize(ht, (ht->base_s / 2));
}


static ht_index_T* ht_new_sized(const uint32_t base_s)
{
    ht_index_T* ht = malloc(sizeof(struct ht_index_S));
    if (ht == NULL)
        return NULL;

    ht->counter = 0;
    ht->size    = next_prime(base_s);
    ht->base_s  = base_s;

    ht->items   = calloc((size_t)ht->size, sizeof(ht_item_T*));
    if (ht->items == NULL)
        return NULL;

    return ht;
}


ht_index_T* ht_new()
{
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}


void ht_destroy(ht_index_T* t)
{
    for (size_t i = 0; i < t->size; i++)
    {
        ht_item_T* item = t->items[i];
        if (item != NULL)
            hti_del(item);
    }

    free(t->items);
    free(t);
}


void ht_insert(ht_index_T* t, const char* k, const char* v)
{
    if (ht_load_ratio(t) > 70)
        ht_resize_up(t);

    ht_item_T* i = hti_new(k, v);

    size_t idx = ht_get_hidx(i->key, t->size, 0);
    ht_item_T* cur_i = t->items[idx];

    size_t iter = 1;
    while (cur_i != NULL)
    {
        if (cur_i != &HT_DELETED_ITEM && strcmp(cur_i->key, k) == 0)
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


char* ht_search(ht_index_T* t, const char* k)
{
    size_t idx = ht_get_hidx(k, t->size, 0);

    ht_item_T* i = t->items[idx];

    size_t iter = 1;
    while (i != NULL)
    {
        if (i != &HT_DELETED_ITEM && strcmp(i->key, k) == 0)
            return i->val;

        idx = ht_get_hidx(k, t->size, iter);
        i = t->items[idx];
        iter++;
    }

    return NULL;
}


void ht_delete(ht_index_T* t, const char* k)
{
    if (ht_load_ratio(t) < 10)
        ht_resize_down(t);

    size_t idx = ht_get_hidx(k, t->size, 0);

    ht_item_T* i = t->items[idx];

    size_t iter = 1;
    while (i != NULL)
    {
        if (i != &HT_DELETED_ITEM && strcmp(i->key, k) == 0)
        {
            hti_del(i);
            t->items[idx] = &HT_DELETED_ITEM;
        }

        idx = ht_get_hidx(k, t->size, iter);
        i = t->items[idx];
        i++;
    }

    t->counter--;
}
