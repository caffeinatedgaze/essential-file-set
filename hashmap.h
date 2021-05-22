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

UINT32 hash(VOID *key) {
	return (uintptr_t)key % TABLE_SIZE;
}

entry_t *ht_pair(VOID *key, VOID *value) {
	// allocate the entry
	entry_t *entry = (entry_t *)AllocatePool(sizeof(entry_t));
	entry->key = key;
	entry->value = value;
	entry->next = NULL;

	if (entry == NULL)
		return NULL;

	return entry;
}

ht_t *ht_create(VOID) {
	ht_t *hashtable = (ht_t *)AllocatePool(sizeof(ht_t));

	if (NULL == hashtable)
		return NULL;

	hashtable->entries = AllocatePool(sizeof(entry_t *) * TABLE_SIZE);

	if (NULL == hashtable->entries)
		return NULL;

	for (int i = 0; i < TABLE_SIZE; i++) {
		hashtable->entries[i] = NULL;
	}
	return hashtable;
}

VOID ht_set(ht_t *hashtable, VOID *key, VOID *value) {
	UINT32 slot = hash(key);
	entry_t *entry = hashtable->entries[slot];

	// no entry means that we insert immediately
	if (NULL == entry) {
		hashtable->entries[slot] = ht_pair(key, value);
		return;
	}

	entry_t *prev;
	while (entry != NULL) {
		if (key == entry->key) {
			// match found, replace value
			entry->value = value;
			return;
		}

		// walk to the next
		prev = entry;
		entry = prev->next;
	}
	prev->next = ht_pair(key, value);
}

VOID *ht_get(ht_t *hashtable, VOID *key) {
	UINT32 slot = hash(key);
	entry_t *entry = hashtable->entries[slot];

	if (NULL == entry) {
		return NULL;
	}

	while (entry != NULL) {
		if (key == entry->key) {
			return entry->value;
		}

		// proceed to the next key if available
		entry = entry->next;
	}

	// there is a slot collision, but no exact much
	return NULL;
}

VOID ht_dump(ht_t *hashtable) {
	for (UINT32 i = 0; i < TABLE_SIZE; i++) {
		entry_t *entry = hashtable->entries[i];

		if (NULL == entry) {
			continue;
		}

		DEBUG((EFI_D_INFO, "slot[%4x]: ", i));

		for (;;) {
			DEBUG((EFI_D_INFO, "%x=%x", entry->key, entry->value));

			if (entry->next == NULL)
				break;

			entry = entry->next;
		}
		DEBUG((EFI_D_INFO, "\r\n"));
	}
}
