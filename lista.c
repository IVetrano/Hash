#include "lista.h"

#include <stdlib.h>
#include <stdio.h>

//Nodos
typedef struct nodo{
	void* dato;
	struct nodo* prox;
} nodo_t;

nodo_t* nodo_crear(void* valor){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (!nodo) return NULL;
	nodo->dato = valor;
	nodo->prox = NULL;
	return nodo;
}

void nodo_destruir(nodo_t* nodo){
	free(nodo);
}

//Lista

struct lista{
	nodo_t* primero;
	nodo_t* ultimo;
	size_t largo;
};

lista_t* lista_crear(void){
	lista_t* lista = malloc(sizeof(lista_t));

	if(!lista) return NULL;

	lista->primero = NULL;
	lista->ultimo = NULL;
	lista->largo = 0;

	return lista;
}

bool lista_esta_vacia(const lista_t* lista){
	return lista->largo == 0 && lista->primero == NULL && lista->ultimo == NULL;
}

bool lista_insertar_primero(lista_t *lista, void *dato){
	nodo_t* nuevo = nodo_crear(dato);
	if (!nuevo) return false;

	if (lista_esta_vacia(lista)) lista->ultimo = nuevo;
	else nuevo->prox = lista->primero;

	lista->primero = nuevo;
	lista->largo++;

	return true;
}

bool lista_insertar_ultimo(lista_t *lista, void *dato){
	nodo_t* nuevo = nodo_crear(dato);
	if (!nuevo) return false;

	if (lista_esta_vacia(lista)) lista->primero = nuevo;
	else lista->ultimo->prox = nuevo;

	lista->ultimo = nuevo;
	lista->largo++;

	return true;
}

void* lista_borrar_primero(lista_t *lista){
	if (lista_esta_vacia(lista)) return NULL;

	void* dato = lista->primero->dato;
	nodo_t* nodo = lista->primero;
	lista->primero = lista->primero->prox;
	nodo_destruir(nodo);

	if (!lista->primero) lista->ultimo = NULL;

	lista->largo--;

	return dato;
}

void* lista_ver_primero(const lista_t *lista){
	if (lista_esta_vacia(lista)) return NULL;
	return lista->primero->dato;
}


void* lista_ver_ultimo(const lista_t *lista){
	if (lista_esta_vacia(lista)) return NULL;
	return lista->ultimo->dato;
}

size_t lista_largo(const lista_t *lista){
	return lista->largo;
}

void lista_destruir(lista_t *lista, void (*destruir_dato)(void *)){
	nodo_t* actual = lista->primero;

	while (actual){
		if (destruir_dato) destruir_dato(actual->dato);
		actual = actual->prox;
		lista_borrar_primero(lista);
	}
	
	free(lista);
}


void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
	nodo_t* actual = lista->primero;
	bool seguir_iterando = true;
	while (actual && seguir_iterando){
		seguir_iterando = visitar(actual->dato, extra);
		actual = actual->prox;
	}
}



//Iterador externo

struct lista_iter{
	nodo_t* anterior;
	nodo_t* actual;
	lista_t* lista;
};

lista_iter_t* lista_iter_crear(lista_t* lista){
	lista_iter_t* iterador = malloc(sizeof(lista_iter_t));
	if(!iterador) return NULL;

	iterador->lista = lista;
	iterador->anterior = NULL;
	iterador->actual = lista->primero;

	return iterador;
}

bool lista_iter_avanzar(lista_iter_t *iter){
	if (lista_iter_al_final(iter)) return false;

	iter->anterior = iter->actual;
	iter->actual = iter->actual->prox;
	return true;
}

void* lista_iter_ver_actual(const lista_iter_t* iter){
	if (!lista_iter_al_final(iter)) return iter->actual->dato;
	return NULL;
}

bool lista_iter_al_final(const lista_iter_t* iter){
	return iter->actual == NULL;
}

void lista_iter_destruir(lista_iter_t* iter){
	free(iter);
}


bool lista_iter_insertar(lista_iter_t* iter, void* dato){
	//Si el iterador esta al principio de la lista
	if (!iter->anterior){
		if (!lista_insertar_primero(iter->lista, dato)) return false;
		iter->actual = iter->lista->primero;
	}

	//Si el iterador esta al final o no hay elementos
	else if (lista_iter_al_final(iter)){
		if (!lista_insertar_ultimo(iter->lista, dato)) return false;
		iter->actual = iter->lista->ultimo;
	}

	//En caso contrario
	else {
		nodo_t* nuevo = nodo_crear(dato);
		if (!nuevo) return false;
		nuevo->prox = iter->actual;
		iter->anterior->prox = nuevo;
		iter->actual = nuevo;
		iter->lista->largo++;
	}

	return true;
}

void* lista_iter_borrar(lista_iter_t* iter){
	//Si estoy al final de la lista o no hay elementos
	if (!iter->actual) return NULL;

	void* dato;

	//Si estoy al principio de la lista
	if (!iter->anterior){
		dato = lista_borrar_primero(iter->lista);
		iter->actual = iter->lista->primero;
		return dato;
	}
	
	//En caso contrario
	else {
		dato = iter->actual->dato;
		nodo_t* nodo = iter->actual;

		if (iter->lista->ultimo == nodo) iter->lista->ultimo = iter->anterior;

		iter->anterior->prox = iter->actual->prox;
		iter->actual = iter->actual->prox;
		nodo_destruir(nodo);
		iter->lista->largo--;
	}

	return dato;
}