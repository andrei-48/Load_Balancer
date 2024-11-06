/*
 * Copyright (c) 2024, Marinescu Andrei-Bogdan <<andy.marinescu04@gmail.com>>
 */

#include <stdio.h>
#include <string.h>
#include "lru_cache.h"
#include "utils.h"

lru_cache *init_lru_cache(unsigned int cache_capacity) {
    lru_cache *cache = malloc(sizeof(lru_cache));
    DIE(!cache, "malloc() failed\n");

    cache->ht = ht_create(cache_capacity, hash_string,
                          compare_function_strings, key_val_free_function);
    cache->list = dll_create(sizeof(doc_t));
    cache->max_capacity = cache_capacity;
    return cache;
}

bool lru_cache_is_full(lru_cache *cache) {
    if (cache->list->size == cache->max_capacity)
        return true;
    return false;
}

// functie care elibereaza memoria listei din cache
void free_cache_list(doubly_linked_list_t** cache_list)
{
    dll_node_t* node = (*cache_list)->head;
    for (unsigned int i = 0; i < (*cache_list)->size; i++) {
        (*cache_list)->head = node->next;
        free(((doc_t *)node->data)->doc_content);
        free(((doc_t *)node->data)->doc_name);
        free(node->data);
        free(node);
        node = (*cache_list)->head;
    }
    free(*cache_list);
}

void free_lru_cache(lru_cache **cache) {
    free_cache_list(&((*cache)->list));
    ht_free((*cache)->ht);
    free(*cache);
}

bool lru_cache_put(lru_cache *cache, void *key, void *value,
                   void **evicted_key) {
    if (ht_has_key(cache->ht, key)) {
        // obtinem nodul din lista folosind hashmap-ul cache-ului
        dll_node_t *node = *((dll_node_t **)(ht_get(cache->ht, key)));
        if (node == cache->list->head && node != cache->list->tail) {
            cache->list->head = node->next;
            if (cache->list->head)
                cache->list->head->prev = NULL;
        } else if (node != cache->list->tail) {
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
        if (node != cache->list->tail) {
            node->prev = cache->list->tail;
            cache->list->tail->next = node;
            cache->list->tail = node;
        }
        ht_put(cache->ht, key, DOC_NAME_LENGTH, &node, sizeof(dll_node_t **));
        memcpy(((doc_t *)node->data)->doc_content, value, DOC_CONTENT_LENGTH);
        return false;
    }

    // se face loc pentru noua intrare in cache
    if (lru_cache_is_full(cache)) {
        *evicted_key = malloc(DOC_NAME_LENGTH);
        DIE(!(*evicted_key), "malloc() failed\n");

        dll_node_t *evicted = dll_remove_nth_node(cache->list, 0);
        char *doc_name = ((doc_t *)evicted->data)->doc_name;
        memcpy(*evicted_key, doc_name, DOC_NAME_LENGTH);
        ht_remove_entry(cache->ht, doc_name);
        free(((doc_t *)evicted->data)->doc_content);
        free(((doc_t *)evicted->data)->doc_name);
        free(evicted->data);
        free(evicted);
    }

    // document auxiliar pentru a pune informatia in noul nod
    doc_t *doc = malloc(sizeof(doc_t));
    DIE(!doc, "malloc() failed\n");
    doc->doc_name = malloc(DOC_NAME_LENGTH);
    DIE(!doc->doc_name, "malloc() failed\n");
    doc->doc_content = malloc(DOC_CONTENT_LENGTH);
    DIE(!doc->doc_content, "malloc() failed\n");
    strcpy(doc->doc_name, (char *)key);
    strcpy(doc->doc_content, (char *)value);

    // adaugam noul nod la finalul listei reprezentand
    // documentul accesat cel mai recent
    dll_add_nth_node(cache->list, cache->list->size, doc);
    free(doc);
    ht_put(cache->ht, key, DOC_NAME_LENGTH,
           &cache->list->tail, sizeof(dll_node_t **));
    return true;
}

void *lru_cache_get(lru_cache *cache, void *key) {
    if (!ht_has_key(cache->ht, key))
        return NULL;

    dll_node_t *node = *((dll_node_t **)(ht_get(cache->ht, key)));
    if (node == cache->list->head && node != cache->list->tail) {
        cache->list->head = node->next;
        if (cache->list->head)
            cache->list->head->prev = NULL;
    } else if (node != cache->list->tail) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    if (node != cache->list->tail) {
        node->prev = cache->list->tail;
        cache->list->tail->next = node;
        cache->list->tail = node;
    }
    ht_put(cache->ht, key, DOC_NAME_LENGTH,
           &cache->list->tail, sizeof(dll_node_t **));
    return ((doc_t *)node->data)->doc_content;
}

void lru_cache_remove(lru_cache *cache, void *key) {
    if (!ht_has_key(cache->ht, key))
        return;

    dll_node_t *node = *((dll_node_t **)(ht_get(cache->ht, key)));
        if (node == cache->list->head) {
            cache->list->head = node->next;
            cache->list->size--;
        } else if (node == cache->list->tail) {
            cache->list->tail = node->prev;
            cache->list->tail->next = NULL;
            cache->list->size--;
        } else {
            node->prev->next = node->next;
            node->next->prev = node->prev;
            cache->list->size--;
        }
    ht_remove_entry(cache->ht, key);
    free(((doc_t *)node->data)->doc_content);
    free(((doc_t *)node->data)->doc_name);
    free(node->data);
    free(node);
}
