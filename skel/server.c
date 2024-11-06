/*
 * Copyright (c) 2024, Marinescu Andrei-Bogdan <<andy.marinescu04@gmail.com>>
 */

#include <stdio.h>
#include "server.h"
#include "lru_cache.h"

#include "utils.h"

// Functia elimina toate elementele din coada de taskuri
void q_clear(queue_t* q)
{
	unsigned int i;
	if (!q || !q->size)
		return;

	for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size) {
        free(((request *)q->buff[i])->doc_content);
        free(((request *)q->buff[i])->doc_name);
        free(q->buff[i]);
    }

	q->read_idx = 0;
	q->write_idx = 0;
	q->size = 0;
}

// Functia elibereaza toata memoria ocupata de coada de taskuri.
void q_free(queue_t* q)
{
	if (!q)
		return;

	q_clear(q);
	free(q->buff);
	free(q);
}

static response
*server_edit_document(server *s, char *doc_name, char *doc_content) {
    response *res = malloc(sizeof(response));
    DIE(!res, "malloc() failed\n");

    res->server_id = s->sv_id;
    res->server_log = malloc(MAX_LOG_LENGTH);
    DIE(!res->server_log, "malloc() failed\n");

    res->server_response = malloc(MAX_RESPONSE_LENGTH);
    DIE(!res->server_response, "malloc() failed\n");

    void **evicted_key = calloc(1, sizeof(void *));
    DIE(!evicted_key, "calloc() failed\n");

    if (!lru_cache_put(s->cache, doc_name, doc_content, evicted_key)) {
        // daca exista deja in cache
        sprintf(res->server_log, LOG_HIT, doc_name);
        sprintf(res->server_response, MSG_B, doc_name);
        ht_put(s->database, doc_name, DOC_NAME_LENGTH,
               doc_content, DOC_CONTENT_LENGTH);
    } else {
        if (ht_has_key(s->database, doc_name)) {
            // daca nu e in cache dar exista in database
            sprintf(res->server_response, MSG_B, doc_name);
            ht_put(s->database, doc_name, DOC_NAME_LENGTH,
                   doc_content, DOC_CONTENT_LENGTH);
        } else {
            // daca documentul nu exista deloc in database-ul serverului
            sprintf(res->server_response, MSG_C, doc_name);
            ht_put(s->database, doc_name, DOC_NAME_LENGTH,
                   doc_content, DOC_CONTENT_LENGTH);
        }
        // daca a fost scoasa o cheie
        if (lru_cache_is_full(s->cache) && *evicted_key) {
            sprintf(res->server_log, LOG_EVICT, doc_name,
                    (char *)(*evicted_key));
            free(*evicted_key);
            *evicted_key = NULL;
        }
        else
            sprintf(res->server_log, LOG_MISS, doc_name);
    }

    free(evicted_key);
    evicted_key = NULL;
    return res;
}

static response
*server_get_document(server *s, char *doc_name) {
    response *res = malloc(sizeof(response));
    DIE(!res, "malloc() failed\n");

    res->server_id = s->sv_id;
    res->server_log = malloc(MAX_LOG_LENGTH);
    DIE(!res->server_log, "malloc() failed\n");

    res->server_response = malloc(MAX_RESPONSE_LENGTH);
    DIE(!res->server_response, "malloc() failed\n");

    void **evicted_key = calloc(1, sizeof(void *));
    DIE(!evicted_key, "calloc() failed\n");

    if (ht_has_key(s->cache->ht, doc_name)) {
        // daca documentul se gaseste in cache
        strcpy(res->server_response, (char *)lru_cache_get(s->cache, doc_name));
        sprintf(res->server_log, LOG_HIT, doc_name);
    } else {
        if (ht_has_key(s->database, doc_name)) {
            // daca documentul nu e in cache dar e in database-ul serverului
            lru_cache_put(s->cache, doc_name,
                          ht_get(s->database, doc_name), evicted_key);
            strcpy(res->server_response,
                   (char *)lru_cache_get(s->cache, doc_name));
            if (lru_cache_is_full(s->cache) && *evicted_key) {
                // daca s-a eliminat o cheie pentru a face loc celei noi
                sprintf(res->server_log, LOG_EVICT,
                        doc_name, (char *)(*evicted_key));
                free(*evicted_key);
                *evicted_key = NULL;
            } else {
                sprintf(res->server_log, LOG_MISS, doc_name);
            }
        } else {
            strcpy(res->server_response, "(null)");
            sprintf(res->server_log, LOG_FAULT, doc_name);
        }
    }

    free(evicted_key);
    evicted_key = NULL;
    return res;
}

server *init_server(unsigned int cache_size) {
    server *sv = malloc(sizeof(server));
    DIE(!sv, "malloc() failed\n");

    sv->cache = init_lru_cache(cache_size);
    sv->database = ht_create(5000, hash_string,
                             compare_function_strings, key_val_free_function);
    sv->task_q = q_create(sizeof(request), TASK_QUEUE_SIZE);
    return sv;
}

response *server_handle_request(server *s, request *req) {
    response *res;

    if (req->type == EDIT_DOCUMENT) {
        // se baga request-ul in coada de task-uri
        request *aux_req = malloc(sizeof(request));
        DIE(!aux_req, "malloc() failed\n");

        aux_req->doc_name = malloc(DOC_NAME_LENGTH);
        DIE(!aux_req->doc_name, "malloc() failed\n");

        aux_req->type = req->type;
        aux_req->doc_content = malloc(DOC_CONTENT_LENGTH);
        DIE(!aux_req->doc_content, "malloc() failed\n");

        memcpy(aux_req->doc_name, req->doc_name, DOC_NAME_LENGTH);
        memcpy(aux_req->doc_content, req->doc_content, DOC_CONTENT_LENGTH);
        q_enqueue(s->task_q, aux_req);
        free(aux_req);
        res = malloc(sizeof(response));
        DIE(!res, "malloc() failed\n");

        res->server_id = s->sv_id;
        res->server_log = malloc(MAX_LOG_LENGTH);
        DIE(!res->server_log, "malloc() failed\n");

        res->server_response = malloc(MAX_RESPONSE_LENGTH);
        DIE(!res->server_response, "malloc() failed\n");

        sprintf(res->server_response, MSG_A,
                get_request_type_str(req->type), req->doc_name);
        sprintf(res->server_log, LOG_LAZY_EXEC, q_get_size(s->task_q));
    } else if (req->type == GET_DOCUMENT) {
        // se executa toate task-urile din coada apoi
        // se executa request-ul de "GET"
        while (!q_is_empty(s->task_q)) {
            request *aux_req = q_front(s->task_q);
            response *aux_res = server_edit_document(s, aux_req->doc_name,
                                                     aux_req->doc_content);
            free(aux_req->doc_content);
            free(aux_req->doc_name);
            PRINT_RESPONSE(aux_res);
            q_dequeue(s->task_q);
        }
        if (req->doc_name)
            res = server_get_document(s, req->doc_name);
    }
    if (req->doc_name)
        return res;
    return NULL;
}

void free_server(server **s) {
    ht_free((*s)->database);
    q_free((*s)->task_q);
    free_lru_cache(&(*s)->cache);
    free(*s);
}
