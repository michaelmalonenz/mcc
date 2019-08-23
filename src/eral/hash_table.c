/**
 Copyright (c) 2019, Michael Malone
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the original author nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL MICHAEL MALONE BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mcc.h"
#include "liberal.h"
#include "hash_table.h"

#define DEFAULT_HASH_TABLE_LENGTH 4096

struct hash_table_node {
    uintptr_t data;
    void *key;
    uint32_t key_len;
    struct hash_table_node *next;
};

struct eral_hash_table
{
    struct hash_table_node **table;
    uint32_t size;
};

static uint32_t elf_hash(const void *key, uint16_t key_len, uint32_t size)
{
    const unsigned char *array = key;
    uint32_t hash = 0, mixer;
    uint16_t i;

    for (i = 0; i < key_len; i++)
    {
        hash = (hash << 4) + array[i];
        mixer = hash & 0xf0000000L;

        if (mixer != 0)
            hash ^= mixer >> 24;

        hash &= ~mixer;
    }

    return hash % size;
}

eral_HashTable_t *eral_HashTableCreate(void)
{
    return eral_HashTableCreateSize(DEFAULT_HASH_TABLE_LENGTH);
}

eral_HashTable_t *eral_HashTableCreateSize(uint32_t size)
{
    eral_HashTable_t *result = (eral_HashTable_t *) malloc(sizeof(eral_HashTable_t));
    result->table = (struct hash_table_node **) malloc(sizeof(void *) * size);
    result->size = size;

    memset(result->table, 0, sizeof(void *) * size);
    return result;
}

void destroy_node(struct hash_table_node *node)
{
    if (node->key != NULL)
    {
        free(node->key);
    }
    free(node);
}

void eral_HashTableDelete(eral_HashTable_t *table, eral_HashNodeDestructor_fn destructor)
{
    uint32_t i;
    for (i = 0; i < table->size; i++)
    {
        if (table->table[i] != NULL)
        {
            struct hash_table_node *current = table->table[i];
            struct hash_table_node *next;
            while (current != NULL)
            {
                next = current->next;
                if (destructor)
                {
                    destructor(current->data);
                }
                destroy_node(current);
                current = next;
            }
            table->table[i] = NULL;
        }
    }
    free(table->table);
    free(table);
}

void eral_HashTableInsert(eral_HashTable_t *table, const void *key, uint16_t key_len, uintptr_t data)
{
    uint32_t index = elf_hash(key, key_len, table->size);
    struct hash_table_node *node = (struct hash_table_node *) malloc(sizeof(struct hash_table_node));
    node->key = malloc(key_len);
    node->data = data;
    memcpy(node->key, key, key_len);
    node->key_len = key_len;
    node->next = NULL;
    if (table->table[index] == NULL)
    {
        table->table[index] = node;
    }
    else
    {
        struct hash_table_node *temp = table->table[index];
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = node;
    }
}

uintptr_t eral_HashTableFind(eral_HashTable_t *table, const void *key, uint16_t key_len)
{
    uint32_t index = elf_hash(key, key_len, table->size);
    struct hash_table_node *result = table->table[index];
    while (result != NULL)
    {
        if (key_len == result->key_len &&
            memcmp(key, result->key, key_len) == 0)
        {
            return result->data;
        }
        else
        {
            result = result->next;
        }
    }
    return NULL_DATA;
}

uintptr_t eral_HashTableRemove(eral_HashTable_t *table, const void *key, uint16_t key_len)
{
    uint32_t index = elf_hash(key, key_len, table->size);
    struct hash_table_node *current = table->table[index];
    struct hash_table_node *previous = NULL;
    uintptr_t result = NULL_DATA;
    while (current != NULL)
    {
        if (current->key_len == key_len &&
            memcmp(current->key, key, key_len) == 0)
        {
            result = current->data;
            if (previous == NULL)
            {
                table->table[index] = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            destroy_node(current);
            return result;
        }   
        previous = current;
        current = current->next;
    }
    return result;
}