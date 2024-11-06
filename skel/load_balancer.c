/*
 * Copyright (c) 2024, Marinescu Andrei-Bogdan <<andy.marinescu04@gmail.com>>
 */

#include "load_balancer.h"

load_balancer* init_load_balancer(bool enable_vnodes)
{
    load_balancer* main = malloc(sizeof(load_balancer));
    DIE(!main, "malloc() failed\n");

    main->hash_function_docs = hash_string;
    main->hash_function_servers = hash_uint;
    main->vnodes = enable_vnodes;
    main->servers = malloc(MAX_SERVERS * sizeof(server));
    DIE(!main->servers, "malloc() failed\n");
    main->nr_svs = 0;

    return main;
}

// muta documentele necesare din serverul sursa in serverul destinatie
void move_docs(server* dest, server* src)
{
    // folosim un request auxiliar pentru a goli coada
    request* aux_req = malloc(sizeof(request));
    DIE(!aux_req, "malloc() failed\n");

    aux_req->type = GET_DOCUMENT;
    aux_req->doc_name = NULL;
    server_handle_request(src, aux_req);
    for (unsigned int i = 0; i < src->database->hmax; i++) {
        int ok = 0;  // variabila pentru resetarea cautarii intr-un bucket
        for (ll_node_t* node = src->database->buckets[i]->head; node;
             node = node->next) {
            unsigned int doc_hash = hash_string(((info*)node->data)->key);
            if (((dest->sv_hash < src->sv_hash) &&
                (doc_hash < dest->sv_hash || doc_hash > src->sv_hash)) ||
                ((dest->sv_hash > src->sv_hash) &&
                (doc_hash < dest->sv_hash && doc_hash > src->sv_hash))) {
            ht_put(dest->database,
                   ((info*)node->data)->key, DOC_NAME_LENGTH,
                   ((info*)node->data)->value, DOC_CONTENT_LENGTH);
            lru_cache_remove(src->cache, ((info*)node->data)->key);
            ht_remove_entry(src->database, ((info*)node->data)->key);
            ok = 1;
            break;
            }
        }
        // daca s-a mutat un document reinitializam cautarea in aceeasi lista
        if (ok)
            i--;
    }
    free(aux_req);
}

void loader_add_server(load_balancer* main, int server_id, int cache_size)
{
    server* new_sv = init_server(cache_size);
    new_sv->sv_id = server_id;
    new_sv->sv_hash = main->hash_function_servers(&server_id);
    int poz = 0;
    while (poz != main->nr_svs &&
           (main->servers[poz]->sv_hash < new_sv->sv_hash)) {
        poz++;  // obtinem pozitia pentru serverul nou
    }
    // se muta celelalte servere pentru a face loc celui nou
    for (int i = main->nr_svs; i > poz; i--) {
        main->servers[i] = main->servers[i - 1];
    }
    // se introduce serverul nou in poztita sa
    main->servers[poz] = new_sv;
    main->nr_svs++;
    if (main->nr_svs > 1) {
        move_docs(main->servers[poz], main->servers[(poz + 1) % main->nr_svs]);
    }
}

void loader_remove_server(load_balancer* main, int server_id)
{
    for (int k = 0; k < main->nr_svs; k++) {
        if (main->servers[k]->sv_id == server_id) {
            // folosim un request auxiliar pentru a goli coada
            request* aux_req = malloc(sizeof(request));
            DIE(!aux_req, "malloc() failed\n");
            aux_req->type = GET_DOCUMENT;
            aux_req->doc_name = NULL;
            server_handle_request(main->servers[k], aux_req);
            if (main->servers[(k + 1) % main->nr_svs]) {
                for (unsigned int i = 0;
                     i < main->servers[k]->database->hmax; i++) {
                    ll_node_t *start =
                            main->servers[k]->database->buckets[i]->head;
                    for (ll_node_t* node = start; node; node = node->next) {
                        ht_put(main->servers[(k + 1) % main->nr_svs]->database,
                               ((info*)node->data)->key, DOC_NAME_LENGTH,
                               ((info*)node->data)->value, DOC_CONTENT_LENGTH);
                    }
                }
            }
            free(aux_req);
            free_server(&(main->servers[k]));
            for (int j = k; j < main->nr_svs - 1; j++)
                main->servers[j] = main->servers[j + 1];
            main->servers[main->nr_svs - 1] = NULL;
            main->nr_svs--;
            return;
        }
    }
}

response* loader_forward_request(load_balancer* main, request* req)
{
    unsigned int doc_hash = main->hash_function_docs(req->doc_name);
    for (int i = 0; i < main->nr_svs; i++) {
        if (doc_hash < main->servers[i]->sv_hash)
            return server_handle_request(main->servers[i], req);
    }
    // s-a parcurs tot hashring-ul deci se va aplica request-ul pe primul server
    return server_handle_request(main->servers[0], req);
}

void free_load_balancer(load_balancer** main)
{
    for (int i = 0; i < (*main)->nr_svs; i++)
        free_server(&(*main)->servers[i]);
    free((*main)->servers);
    free(*main);

    *main = NULL;
}
