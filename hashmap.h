#pragma once

#include <Uefi.h>
#include <stdint.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>


#define TABLE_SIZE 100

typedef struct entry_t {
	VOID *key; // EFI_HANDLE *
	VOID *value;
	struct entry_t *next;
} entry_t;

typedef struct {
	entry_t **entries;
} ht_t;

ht_t * ht_create(VOID);

VOID ht_set(ht_t * hashtable, VOID * key, VOID * value);

VOID * ht_get(ht_t * hashtable, VOID * key);

VOID ht_dump(ht_t * hashtable);
