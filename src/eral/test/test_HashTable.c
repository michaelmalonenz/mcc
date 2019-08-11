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
 DISCLAIMED. IN NO EVENT SHALL Michael Malone BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include <stdlib.h>
#include <string.h>

#define MCC_DEBUG 1
#include "config.h"
#include "mcc.h"
#include "liberal.h"
#include "hash_table.h"

static void test_Insert(void)
{
    printf("Running test_Insert...");
    eral_HashTable_t *table = eral_HashTableCreate();

    eral_HashTableInsert(table, "Hello", 5, (uintptr_t) 42);

    eral_HashTableDelete(table, NULL);
    printf("ok\n");
}

static void test_Find(void)
{
    printf("Running test_Find...");
    const char *key = "Hello";
    size_t key_len = strlen(key);
    eral_HashTable_t *table = eral_HashTableCreate();

    eral_HashTableInsert(table, key, key_len, (uintptr_t) 42);
    int result = (int) eral_HashTableFind(table, key, key_len);
    MCC_ASSERT(result == 42);
    result = (int) eral_HashTableFind(table, key, key_len);
    MCC_ASSERT(result == 42);

    eral_HashTableDelete(table, NULL);
    printf("ok\n");
}

static void test_Remove(void)
{
    printf("Running test_Remove...");
    const char *key = "Hello";
    size_t key_len = strlen(key);
    eral_HashTable_t *table = eral_HashTableCreate();

    eral_HashTableInsert(table, key, key_len, (uintptr_t) 42);
    int result = (int) eral_HashTableRemove(table, key, key_len);
    MCC_ASSERT(result == 42);
    result = (int) eral_HashTableRemove(table, key, key_len);
    MCC_ASSERT(result == NULL_DATA);

    eral_HashTableDelete(table, NULL);
    printf("ok\n");
}

typedef struct test_struct_thing {
    const char *key;
    uint32_t key_len;
} test_StructThing_t;

void deleteStructThing(uintptr_t data)
{
    free((test_StructThing_t *) data);
}

void test_DeleteWithDynamicMemory(void)
{
    printf("Running test_Delete With Dynamic Memory...");
    const char *key = "Hello";
    size_t key_len = strlen(key);
    eral_HashTable_t *table = eral_HashTableCreate();
    test_StructThing_t *data = (test_StructThing_t *) malloc(sizeof(test_StructThing_t));
    data->key = key;
    data->key_len = key_len;

    eral_HashTableInsert(table, key, key_len, (uintptr_t) data);
    test_StructThing_t *result = (test_StructThing_t *) eral_HashTableFind(table, key, key_len);
    MCC_ASSERT(result->key_len == key_len);
    MCC_ASSERT(strncmp(result->key, key, key_len) == 0);

    eral_HashTableDelete(table, deleteStructThing);
    printf("ok\n");
}

int main(void)
{
    test_Insert();
    test_Find();
    test_Remove();
    test_DeleteWithDynamicMemory();
    return EXIT_SUCCESS;
}
