/*
 * Copyright (c) 2024, Marinescu Andrei-Bogdan <<andy.marinescu04@gmail.com>>
 */

#ifndef SERVER_H
#define SERVER_H

#include "utils.h"
#include "constants.h"
#include "lru_cache.h"

#define TASK_QUEUE_SIZE         1000
#define MAX_LOG_LENGTH          1000
#define MAX_RESPONSE_LENGTH     4096

typedef struct server {
    lru_cache *cache;
    hashtable_t *database;
    queue_t *task_q;
    int sv_id;
    unsigned int sv_hash;
} server;

typedef struct request {
    request_type type;
    char *doc_name;
    char *doc_content;
} request;

typedef struct response {
    char *server_log;
    char *server_response;
    int server_id;
} response;

void q_clear(queue_t* q);
void q_free(queue_t* q);

server *init_server(unsigned int cache_size);

/**
 * @brief Should deallocate completely the memory used by server,
 *     taking care of deallocating the elements in the queue, if any,
 *     without executing the tasks
 */
void free_server(server **s);

/**
 * server_handle_request() - Receives a request from the load balancer
 *      and processes it according to the request type
 * 
 * @param s: Server which processes the request.
 * @param req: Request to be processed.
 * 
 * @return response*: Response of the requested operation, which will
 *      then be printed in main.
 * 
 * @brief Based on the type of request, should call the appropriate
 *     solver, and should execute the tasks from queue if needed (in
 *     this case, after executing each task, PRINT_RESPONSE should
 *     be called).
 */
response *server_handle_request(server *s, request *req);

#endif  /* SERVER_H */
