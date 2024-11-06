## Copyright (c) 2024, Marinescu Andrei-Bogdan <<andy.marinescu04@gmail.com>>

**Nume: Marinescu Andrei-Bogdan**
**Grupă: 315CA**

## Distributed Database - Tema 2

### Descriere:

* Scurtă descriere a funcționalității temei:

* Programul e alcatuit din mai multe fisiere fiecare continand functii pentru o parte a implementarii.
	1. lab_implem: contine implementarile structurilor de date generice ce vor fi folosite in restul temei
		- lista simplu inalantuita: folosita in bucket-urile hashtable-ului
		- lista dublu inlantuita: folosita in lista ce retine ordinea elementelor in cache
		- hashtable-ul: folosit in cache pentru a avea acces la elementele din lista in O(1) si la
						database-ul fiecarui server pentru a retine toate documentele
		- coada (implementata cu vector circular): folosita pentru coada de task-uri a fiecarui server

	2. lru_cache: contine functiile folosite pentru implementarea cache-ului
		- init_lru_cache: creeaza cache-ul si initiaza campurile structurii
		- lru_cache_is_full: verifica daca cache-ul e plin
		- free_cache_list: elibereaza toata memoria folosita de lista din cache
		- free_lru_cache: elibereaza intreaga memorie a cache-ului
		- lru_cache_put: introduce un document in cache (atat in lista cat si in hashtable)
						 intoarce true daca s-a adaugat un element nou si false daca elementul exista deja
		- lru_cache_get: intoarce continutul unui document din cache sau NULL daca documentul nu exista
		- lru_cache_remove: elimina un element din cache

	3. server: contine functiile folosite pentru implementarea unui server
		- q_clear: elimina toate elemnetele din coada de task-uri
		- q_free: elibereaza memoria folosita de coada de task-uri
		- server_edit_document: functia care efectueaza partea de editare a unui document (partea stanga a organigramei);
								deoarece orice operatie asupra unui document il baga in cache folosim lru_cache_put intr-un if
								pentru a adauga elementul in cache si pentru a verifica daca exista deja in el sau nu (vezi ce intoarce lru_cache_put)
								se realizeaza logica din organigrama de pe ocw pentru a creea un response cu campurile necesare, in functie de cazul in care ne aflam
		- server_get_document: functia intoarce continutul unui document sau (null) daca documentul nu exista (partea dreapta a organigramei);
							   se realizeaza logica din organigrama de pe ocw pentru a creea un response cu campurile necesare, in functie de cazul in care ne aflam
		- init_server: creeaza server-ul si initiaza campurile structurii
		- server_request_handle: functia implementeaza partea de "lazy exec" a server-ului
								 daca server-ul primeste un request de "EDIT" aceste e adaugat in coada de task-uri a server-ului
								 daca server-ul primeste un request de "GET" se executa toate task-urile din coada apoi se executa "GET-ul"
		- free_server: elibereaza toata memoria folosita de server

	4. laod_balancer: contine functiile folosite pentru implementarea load balancer-ului
		- init_load balancer: creeaza load balancer-ul si initiaza campurile structurii;
							  creeaza array-ul de servere (hash ring-ul);
		- move_docs: muta documentele dintr-un server sursa intr-un server destinatie
					 goleste coada de task-uri a server-ului sursa (executa toate task-urile din coada)
		- loader_add_server: adauga un server nou in hash ring;
							 gaseste pozitia in care trebuie introdus noul server si muta celelalte servere pentru a face loc celui nou
							 muta documentele in noul server daca este cazul (daca exista mai mult de un server)
		- loader_remove_server: elimina un server din hash ring;
								intai goleste coada de task-uri (le executa pe toate) apoi muta toate documentele din server-ul eliminat la server-ul urmator in hash ring
								elibereaza memoria folosita de server-ul eliminat si muta servere-le urmatoare cu o pozitie inainte
		- loader_forward_request: gaseste server-ul care trebuie sa execute request-ul primit si il trimite server-ului respectiv
		- free_load_balancer: elibereaza memoria folosita de load_balancer


* Eventuale explicații suplimentare pentru anumite părți din temă ce crezi că nu sunt suficient de clare doar urmărind codul
	- pentru a elibera coada de task-uri a unui server am folosit un request auxiliar de tip "GET" fara nume de document sau content, deoarece stim ca server-ul executa toate task-urile din coada cand primeste un request "GET";
	  nu ii dam acestui request nume de document, deoarece nu vrem sa intoarca un raspuns, ci doar sa execute toata coada de task-uri
	- cand mut documentele de la un server la altul am folosit un "reset" pentru a putea folosi functia ht_remove_entry, fara a avea probleme la valgrind sau incercari de acces in zone eliberate recent; practic dupa ce elimin un document dintr-un bucket caut iar in acelasi bucket de la inceput

### Comentarii asupra temei:

* Crezi că ai fi putut realiza o implementare mai bună?
* Sunt destul de multumit de implementarea mea, dar probabil as fi putut optimiza complexitatea de timp din load balancer.

* Ce ai invățat din realizarea acestei teme?
* Am invatat cum functioneaza un sistem de fisiere si distribuirea documentelor intr-o baza de date.
* Am invatat sa folosesc hashtable-ul eficient (acces rapid la elementele unei liste ca la cache).

* Alte comentarii
* Tema a avut un subiect interesant si util.
* Am apreciat explicatiile detaliate de pe ocw in legatura cu implementarea functiilor.

### (Opțional) Resurse / Bibliografie:

* Implementarile de pe lambda checker pentru structurile de date generice folosite in tema.
