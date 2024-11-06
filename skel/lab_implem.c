// Copyright (c) 2024, Marinescu Andrei-Bogdan <<andy.marinescu04@gmail.com>>

#include "lab_implem.h"

linked_list_t *ll_create()
{
    linked_list_t* ll;

    ll = malloc(sizeof(*ll));
	DIE(!ll, "malloc() failed\n");

    ll->head = NULL;
    ll->size = 0;

    return ll;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Daca n >= nr_noduri, noul nod se adauga la finalul listei. Daca
 * n < 0, eroare.
 */
void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data,
                     unsigned int data_size)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (!list) {
        return;
    }

    /* n >= list->size inseamna adaugarea unui nou nod la finalul listei. */
    if (n > list->size) {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
	DIE(!new_node, "malloc() failed\n");

    new_node->data = malloc(data_size);
	DIE(!new_node->data, "malloc() failed\n");
    memcpy(new_node->data, new_data, data_size);

    new_node->next = curr;
    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se elimina nodul de
 * la finalul listei. Daca n < 0, eroare. Functia intoarce un pointer spre acest
 * nod proaspat eliminat din lista. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
ll_node_t *ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (!list || !list->head) {
        return NULL;
    }

    /* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int ll_get_size(linked_list_t* list)
{
     if (!list) {
        return -1;
    }

    return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista si actualizeaza la
 * NULL valoarea pointerului la care pointeaza argumentul (argumentul este un
 * pointer la un pointer).
 */
void ll_free(linked_list_t** pp_list)
{
    ll_node_t* currNode;

    if (!pp_list || !*pp_list) {
        return;
    }

    while (ll_get_size(*pp_list) > 0) {
        currNode = ll_remove_nth_node(*pp_list, 0);
        free(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
    }

    free(*pp_list);
    *pp_list = NULL;
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista inlantuita separate printr-un spatiu.
 */
void ll_print_int(linked_list_t* list)
{
    ll_node_t* curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%d ", *((int*)curr->data));
        curr = curr->next;
    }

    printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista inlantuita, separate printr-un spatiu.
 */
void ll_print_string(linked_list_t* list)
{
    ll_node_t* curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%s ", (char*)curr->data);
        curr = curr->next;
    }

    printf("\n");
}

/*
 * Functii de comparare a cheilor:
 */
int compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int compare_function_chars(void *a, void *b)
{
	int char_a = *((char *)a);
	int char_b = *((char *)b);

	if (char_a == char_b) {
		return 0;
	} else if (char_a < char_b) {
		return -1;
	} else {
		return 1;
	}
}

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

/*
 * Functie care trebuie apelata pentru alocarea si initializarea unei liste.
 * (Setare valori initiale pentru campurile specifice structurii list).
 */
doubly_linked_list_t*
dll_create(unsigned int data_size)
{
    doubly_linked_list_t* list = malloc(sizeof(doubly_linked_list_t));
    list->data_size = data_size;
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    return list;
}

/*
 * Functia intoarce un pointer la nodul de pe pozitia n din lista.
 * Pozitiile din lista sunt indexate incepand cu 0 (i.e. primul nod din lista se
 * afla pe pozitia n=0). Daca n >= nr_noduri, atunci se intoarce nodul de pe
 * pozitia rezultata daca am "cicla" (posibil de mai multe ori) pe lista si am
 * trece de la ultimul nod, inapoi la primul si am continua de acolo. Cum putem
 * afla pozitia dorita fara sa simulam intreaga parcurgere?
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, int n)
{
    dll_node_t* node;
    node = list->head;

    if (n >= (int)list->size)
        n = n % list->size;

    for (int i = 1; i <= n; i++) {
        node = node->next;
    }

    return node;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Cand indexam pozitiile nu "ciclam" pe lista circulara ca la
 * get, ci consideram nodurile in ordinea de la head la ultimul (adica acel nod
 * care pointeaza la head ca nod urmator in lista). Daca n >= nr_noduri, atunci
 * adaugam nodul nou la finalul listei.
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
void dll_add_nth_node(doubly_linked_list_t* list, int n, const void* data)
{
    if (n < 0) {
        return;
    }

    dll_node_t* node;
    node = list->head;

    dll_node_t* new = (dll_node_t*)malloc(sizeof(dll_node_t));
    DIE(new == NULL, "malloc() failed\n");
    new->data = malloc(list->data_size * sizeof(char));
    DIE(new->data == NULL, "malloc() failed\n");
    memcpy(new->data, data, list->data_size);

    if (n >= (int)list->size && list->size != 0) {
        new->next = NULL;
        new->prev = list->tail;
        list->tail->next = new;
        list->tail = new;
        list->size++;
        return;
    }

	if(!list->size) {
		list->head = new;
        list->tail = new;
		new->next = NULL;
		new->prev = NULL;
		list->size++;
		return;
	}

    if (n == 0) {
        new->next = list->head;
		list->head->prev = new;
        list->head = new;
        new->prev = NULL;
        list->size++;
        return;
    }

    for (int i = 1; i < n; i++) {
        node = node->next;
    }

    new->next = node->next;
    new->next->prev = new;
    node->next = new;
    new->prev = node;
    list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Functia intoarce un pointer spre acest nod
 * proaspat eliminat din lista. Daca n >= nr_noduri - 1, se elimina nodul de la
 * finalul listei. Este responsabilitatea apelantului sa elibereze memoria
 * acestui nod.
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, int n)
{
    if (n < 0) {
        return NULL;
    }

    if (n >= (int)list->size - 1) {
        n = list->size - 1;
    }

    dll_node_t* node = list->head;

    if (n == 0) {
        list->head = list->head->next;
        if (list->head)
            list->head->prev = NULL;
        list->size--;
        return node;
    }

    for (int i = 1; i < n; i++) {
        node = node->next;
    }

    dll_node_t* removed = node->next;
    node->next = node->next->next;
    node->next->prev = removed->prev;

    list->size--;
    return removed;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int
dll_get_size(doubly_linked_list_t* list)
{
    return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista.
 */
void dll_free(doubly_linked_list_t** pp_list)
{
    dll_node_t* node = (*pp_list)->head;
    for (unsigned int i = 0; i < (*pp_list)->size; i++) {
        (*pp_list)->head = (*pp_list)->head->next;
        free(node->data);
        free(node);
        node = (*pp_list)->head;
    }
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista inlantuita separate printr-un spatiu.
 */
void dll_print_int(doubly_linked_list_t* list)
{
    dll_node_t* node = list->head;
    for (unsigned int i = 0; i < list->size; i++) {
        printf("%d ", *(int*)node->data);
        node = node->next;
    }

    printf("\n");
}

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*),
		void (*key_val_free_function)(void*))
{
	hashtable_t *ht = (hashtable_t *) malloc(sizeof(hashtable_t));
	DIE(!ht, "malloc() failed\n");
    ht->size = 0;
    ht->hmax = hmax;
    ht->hash_function = hash_function;
    ht->compare_function = compare_function;
    ht->key_val_free_function = key_val_free_function;
    ht->buckets = (linked_list_t **) malloc(ht->hmax * sizeof(linked_list_t));
	DIE(!ht->buckets, "malloc() failed\n");

    for (unsigned int i = 0; i < ht->hmax; ++i) {
        ht->buckets[i] = ll_create();
    }
    return ht;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable
 * folosind functia put;
 * 0, altfel.
 */
int ht_has_key(hashtable_t *ht, void *key)
{
    if (!ht)
        return -1;
    int index = ht->hash_function(key) % ht->hmax;
    ll_node_t *first = ht->buckets[index]->head;
    while (first) {
        if (ht->compare_function(((info*)first->data)->key, key) == 0)
            return 1;
        first = first->next;
    }
	return 0;
}

void *ht_get(hashtable_t *ht, void *key)
{
    if (!ht)
        return NULL;
    int index = ht->hash_function(key) % ht->hmax;
    ll_node_t *first = ht->buckets[index]->head;
    while (first) {
        if (ht->compare_function(((info*)first->data)->key, key) == 0) {
            return ((info*)first->data)->value;
        }
        first = first->next;
    }
	return NULL;
}

/*
 * Atentie! Desi cheia este trimisa ca un void pointer (deoarece nu se impune
 * tipul ei), in momentul in care se creeaza o noua intrare in hashtable (in
 * cazul in care cheia nu se gaseste deja in ht), trebuie creata o copie a
 * valorii la care pointeaza key si adresa acestei copii trebuie salvata in
 * structura info asociata intrarii din ht. Pentru a sti cati octeti trebuie
 * alocati si copiati, folositi parametrul key_size.
 *
 * Motivatie:
 * Este nevoie sa copiem valoarea la care pointeaza key deoarece dupa un apel
 * put(ht, key_actual, value_actual), valoarea la care pointeaza key_actual
 * poate fi alterata (de ex: *key_actual++). Daca am folosi direct adresa
 * pointerului key_actual, practic s-ar modifica din afara hashtable-ului cheia
 * unei intrari din hashtable. Nu ne dorim acest lucru, fiindca exista riscul sa
 * ajungem in situatia in care nu mai stim la ce cheie este inregistrata o
 * anumita valoare.
 */
void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	if (!ht) {
        return;
    }

    int index = ht->hash_function(key) % ht->hmax;
    ll_node_t *first = ht->buckets[index]->head;
    while (first) {
        if (ht->compare_function(((info*)first->data)->key, key) == 0) {
            free(((info*)first->data)->value);
            ((info*)first->data)->value = malloc(value_size);
			DIE(!((info*)first->data)->value, "malloc() failed\n");

            memcpy(((info*)first->data)->value, value, value_size);
            return;
        }
        first = first->next;
    }
    if (!first) {
        info new_info;
        new_info.value = malloc(value_size);
		DIE(!new_info.value, "malloc() failed\n");
        new_info.key = malloc(key_size);
		DIE(!new_info.key, "malloc() failed\n");
        memcpy(new_info.value, value, value_size);
        memcpy(new_info.key, key, key_size);
        ll_add_nth_node(ht->buckets[index], 0, &new_info, sizeof(info));
        ht->size++;
    }
}

/*
 * Procedura care elimina din hashtable intrarea asociata cheii key.
 * Atentie! Trebuie avuta grija la eliberarea intregii memorii folosite pentru o
 * intrare din hashtable (adica memoria pentru copia lui key --vezi observatia
 * de la procedura put--, pentru structura info si pentru structura Node din
 * lista inlantuita).
 */
void ht_remove_entry(hashtable_t *ht, void *key)
{
    if (!ht) {
        return;
    }

    int index = ht->hash_function(key) % ht->hmax;
    ll_node_t* first = ht->buckets[index]->head;

    int i = 0;

    while (first) {
        if (ht->compare_function(((info*)first->data)->key, key) == 0) {
            ht->key_val_free_function((info*)first->data);
            ll_node_t *tmp = ll_remove_nth_node(ht->buckets[index], i);
            free(tmp);
            ht->size--;
            return;
        }
        i++;
        first = first->next;
    }
}

/*
 * Procedura care elibereaza memoria folosita de toate intrarile din hashtable,
 * dupa care elibereaza si memoria folosita pentru a stoca structura hashtable.
 */
void ht_free(hashtable_t *ht)
{
    if (!ht)
        return;
    for (unsigned int i = 0; i < ht->hmax; ++i) {
        ll_node_t *first = ht->buckets[i]->head;
        while (first) {
            ll_node_t *tmp = first;
            first = first->next;
            ht->key_val_free_function(tmp->data);
            free(tmp);
        }
        free(ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
}

void key_val_free_function(void *data) {
    free(((info *)data)->key);
    free(((info *)data)->value);
    free(data);
}

unsigned int ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}

queue_t *q_create(unsigned int data_size, unsigned int max_size)
{
	queue_t *q = calloc(1, sizeof(*q));
	DIE(!q, "calloc() failed\n");

	q->data_size = data_size;
	q->max_size = max_size;

	q->buff = malloc(max_size * sizeof(*q->buff));
	DIE(!q->buff, "malloc() failed\n");

	return q;
}

/*
 * Functia intoarce numarul de elemente din coada al carei pointer este trimis
 * ca parametru.
 */
unsigned int q_get_size(queue_t* q)
{
	return !q ? 0 : q->size;
}

/*
 * Intoarce 1 daca coada este goala si 0 in caz contrar.
 */
unsigned int q_is_empty(queue_t* q)
{
	return !q ? 1 : !q->size;
}

/* 
 * Functia intoarce primul element din coada, fara sa il elimine.
 */
void *q_front(queue_t* q)
{
	if (!q || !q->size)
		return NULL;
	return q->buff[q->read_idx];
}

/*
 * Functia scoate un element din coada. Se va intoarce 1 daca operatia s-a
 * efectuat cu succes (exista cel putin un element pentru a fi eliminat) si
 * 0 in caz contrar.
 */
bool q_dequeue(queue_t* q)
{
	if (!q || !q->size)
		return false;

	free(q->buff[q->read_idx]);

	q->read_idx = (q->read_idx + 1) % q->max_size;
	--q->size;
	return true;
}

/* 
 * Functia introduce un nou element in coada. Se va intoarce 1 daca
 * operatia s-a efectuat cu succes (nu s-a atins dimensiunea maxima) 
 * si 0 in caz contrar.
 */
bool q_enqueue(queue_t* q, void* new_data)
{
	void* data;
	if (!q || q->size == q->max_size)
		return false;

	data = malloc(q->data_size);
	DIE(!data, "malloc() failed\n");

	memcpy(data, new_data, q->data_size);

	q->buff[q->write_idx] = data;
	q->write_idx = (q->write_idx + 1) % q->max_size;
	++q->size;

	return true;
}
