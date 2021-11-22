#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include "lista.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const size_t CAPACIDAD_INICIAL = 20;
const size_t COND_AUMENTAR_CAP = 3;
const size_t COEF_AUMENTO_CAP = 2;
const size_t DENOM_CONDICION_DISMINUCION = 4;
const size_t DENOMINADOR_DISMINUCION_CAP = 2;

struct hash{
	hash_destruir_dato_t destruir_dato;
	lista_t** tabla;
	size_t capacidad;
	size_t cantidad;
};

typedef struct nodo_h{
	char* clave;
	void* valor;
} nodo_h_t;

//Funciones de los nodos

nodo_h_t* nodo_h_crear(const char *clave, void *dato){
	nodo_h_t* nodo = malloc(sizeof(nodo_h_t));
	if (!nodo) return NULL;
	//Copio la clave en una nueva
	char* clave_c = strdup(clave);
	if (!clave_c){
		free(nodo);
		return NULL;
	}
	nodo->clave = clave_c;
	nodo->valor = dato;
	return nodo;
}

void nodo_h_destruir(nodo_h_t* nodo){
	free(nodo->clave);
	free(nodo);
}


//Funciones de la tabla de hash

size_t funcion_hash(const char* str){
		int hash = 5381;
		int c;

		while ((c = *str++))
			hash = ((hash << 5) + hash) + c;

		return (size_t) hash;
	}

size_t pos(const char* clave, size_t capacidad){
	return funcion_hash(clave) % capacidad;
}

hash_t* hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* hash = malloc(sizeof(hash_t));
	if (!hash) return NULL;

	hash->tabla = calloc(CAPACIDAD_INICIAL, sizeof(lista_t*));
	if (!hash->tabla){
		free(hash);
		return NULL;
	}

	hash->destruir_dato = destruir_dato;
	hash->capacidad = CAPACIDAD_INICIAL;
	hash->cantidad = 0;

	return hash;
}

size_t hash_cantidad(const hash_t *hash){
	return hash->cantidad;
}

bool redimensionar(hash_t* hash, size_t nueva_cap){
	//Creo otro hash y le asigno una tabla de tamaño nueva_cap
	hash_t* nuevo = hash_crear(NULL);
	if(!nuevo) return false;
	lista_t** nueva_tabla = calloc(nueva_cap, sizeof(lista_t*));
	if(!nueva_tabla){
		hash_destruir(nuevo);
		return false;
	}

	free(nuevo->tabla);
	nuevo->tabla = nueva_tabla;
	nuevo->capacidad = nueva_cap;

	//Guardo en el nuevo hash todos los conjuntos clave-valor del otro
	for(int i = 0; i < hash->capacidad; i++){
		if (!hash->tabla[i]) continue;
		lista_iter_t* iter = lista_iter_crear(hash->tabla[i]);
		while (!lista_iter_al_final(iter)){
			nodo_h_t* nodo = (nodo_h_t*) lista_iter_ver_actual(iter);
			//Guardo en nuevo el conjunto clave-valor, si no pudo guardarlo devuelvo false
			if (!hash_guardar(nuevo, nodo->clave, nodo->valor)){
				hash_destruir(nuevo);
				return false;
			}
			lista_iter_avanzar(iter);
		}
		lista_iter_destruir(iter);
	}
	//Termino de pasar todos los conjuntos clave-valor a la nueva tabla
	nuevo->tabla = hash->tabla;
	hash->tabla = nueva_tabla;

	size_t aux = hash->capacidad;
	hash->capacidad = nueva_cap;
	nuevo->capacidad = aux;

	aux = nuevo->cantidad;
	nuevo->cantidad = hash->cantidad;
	hash->cantidad = aux;

	hash_destruir(nuevo);
	return true;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	//Redimensiono en caso de que sea necesario
	if (hash->cantidad / hash->capacidad >= COND_AUMENTAR_CAP){
		bool red = redimensionar(hash, hash->capacidad * COEF_AUMENTO_CAP);
		if(!red) return false;
	}

	//Busco su posicion
	size_t posicion = pos(clave, hash->capacidad);
	//Si no existe la lista, la creo
	bool cree_lista = false;
	if (!hash->tabla[posicion]){
		lista_t* lista = lista_crear();
		if (!lista) return false;
		hash->tabla[posicion] = lista;
		cree_lista = true;
	}

	//Busco la clave en la lista de esa posicion
	lista_iter_t* iter = lista_iter_crear(hash->tabla[posicion]);
	while (!lista_iter_al_final(iter)){
		nodo_h_t* nodo = (nodo_h_t*) lista_iter_ver_actual(iter);
		if (strcmp(nodo->clave, clave) == 0){
			if(hash->destruir_dato) hash->destruir_dato(nodo->valor);
			nodo->valor = dato;
			lista_iter_destruir(iter);
			return true;
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);

	//Si no esta esa clave en la lista:

	//Creo el nuevo nodo
	nodo_h_t* nodo = nodo_h_crear(clave, dato);
	if (!nodo){
		//Si habia creado la lista de la posicion => la destruyo
		if (cree_lista){
			lista_destruir(hash->tabla[posicion], NULL);
			hash->tabla[posicion] = NULL;
		}
		return false;
	}

	//Lo agrego al final de la lista
	lista_insertar_ultimo(hash->tabla[posicion], nodo);
	hash->cantidad++;
	return true;
}

void* hash_obtener(const hash_t *hash, const char *clave){
	size_t posicion = pos(clave, hash->capacidad);

	//Si no existe la lista
	if (!hash->tabla[posicion]) return NULL;

	void* dato = NULL;
	lista_iter_t* iter = lista_iter_crear(hash->tabla[posicion]);

	//Itero la lista de esa posicion hasta encontrar la clave
	while (!lista_iter_al_final(iter)){
		nodo_h_t* nodo = (nodo_h_t*) lista_iter_ver_actual(iter);
		if (strcmp(nodo->clave, clave) == 0){
			dato = nodo->valor;
			break;
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);

	if (!dato) return NULL;
	return dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
	size_t posicion = pos(clave, hash->capacidad);

	//Si no existe la lista
	if (!hash->tabla[posicion]) return false;

	lista_iter_t* iter = lista_iter_crear(hash->tabla[posicion]);

	//Itero la lista de esa posicion hasta encontrar la clave
	while (!lista_iter_al_final(iter)){
		nodo_h_t* nodo = (nodo_h_t*) lista_iter_ver_actual(iter);
		if (strcmp(nodo->clave, clave) == 0){
			lista_iter_destruir(iter);
			return true;
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);

	return false;
}



void* hash_borrar(hash_t* hash, const char* clave){
    size_t posicion = pos(clave, hash->capacidad);

    //Si no existe la lista
    if (!hash->tabla[posicion]) return NULL;

    void* dato = NULL;
    bool existia = false;
    lista_iter_t* iter = lista_iter_crear(hash->tabla[posicion]);

    while (!lista_iter_al_final(iter)){
        nodo_h_t* nodo = (nodo_h_t*) lista_iter_ver_actual(iter);
        if (strcmp(nodo->clave, clave) == 0){
        	existia = true;
            dato = nodo->valor;
            nodo_h_destruir(nodo);
            lista_iter_borrar(iter);
            break;
        }
        lista_iter_avanzar(iter);
    }
    lista_iter_destruir(iter);

    if (!existia) return NULL;

    hash->cantidad--;
    if (lista_esta_vacia(hash->tabla[posicion])){
        lista_destruir(hash->tabla[posicion], NULL);
        hash->tabla[posicion] = NULL;
    }

    if(hash->cantidad <= hash->capacidad / DENOM_CONDICION_DISMINUCION && hash->capacidad > CAPACIDAD_INICIAL){
        //Si la cantidad es un cuarto de la capacidad => disminuyo a la mitad la capacidad
        redimensionar(hash, hash->capacidad / DENOMINADOR_DISMINUCION_CAP);
    }

    return dato;
}

void nodo_h_destruir_w(void* nodo){
	nodo_h_destruir((nodo_h_t*) nodo);
}

void hash_destruir(hash_t *hash){
	for (int i = 0; i < hash->capacidad; i++){
		if(hash->tabla[i] == NULL) continue;
		lista_iter_t* iter = lista_iter_crear(hash->tabla[i]);
		while (!lista_iter_al_final(iter)){
			nodo_h_t* nodo = lista_iter_ver_actual(iter);
			if (hash->destruir_dato) hash->destruir_dato(nodo->valor);
			lista_iter_avanzar(iter);
		}
		lista_iter_destruir(iter);
		lista_destruir(hash->tabla[i], nodo_h_destruir_w);
	}	
	free(hash->tabla);
	free(hash);
}


//Iterador
typedef struct hash_iter{
	hash_t* hash_guardado;
	lista_iter_t* lista_iter;
	size_t actual;
	size_t recorridos;
} iter_hash_t;



hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* hash_iter = malloc(sizeof(hash_iter_t));
	if(!hash_iter) return NULL;
	hash_iter->hash_guardado = (hash_t*)hash;
	size_t primero = 0;
	if(hash_iter->hash_guardado->cantidad == 0){
		hash_iter->lista_iter = NULL;
		hash_iter->actual = primero;
		hash_iter->recorridos = 1;
		return hash_iter;
	}
	while(hash_iter->hash_guardado->tabla[primero]==NULL){
		primero++;
	}
	hash_iter->lista_iter = lista_iter_crear(hash_iter->hash_guardado->tabla[primero]);
	hash_iter->actual = primero;
	hash_iter->recorridos = 1;
	return hash_iter;
}


bool hash_iter_al_final(const hash_iter_t *iter){
	if(iter->recorridos > iter->hash_guardado->cantidad || iter->hash_guardado->cantidad==0){
		return true;
	}
	return false;
}


bool hash_iter_avanzar(hash_iter_t *iter){
	if(hash_iter_al_final(iter)){
		return false;
	}
	lista_iter_avanzar(iter->lista_iter);
	if(lista_iter_al_final(iter->lista_iter)){
		if(iter->recorridos >= iter->hash_guardado->cantidad){
			iter->recorridos++;
			return true;
		}
		lista_iter_destruir(iter->lista_iter);
		size_t proximo;
		proximo = iter->actual + 1;
		while(iter->hash_guardado->tabla[proximo]==NULL){
			proximo++;
		}
		iter->lista_iter = lista_iter_crear(iter->hash_guardado->tabla[proximo]);
		iter->actual = proximo;
	}
	iter->recorridos++;
	return true;
}


const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(hash_iter_al_final(iter)){
		return NULL;
	}
	void* nodo_h_actual;
	nodo_h_actual = lista_iter_ver_actual(iter->lista_iter);
	nodo_h_t* nodo = nodo_h_actual;
	return nodo->clave;
}


void hash_iter_destruir(hash_iter_t *iter){
	if(iter->lista_iter!=NULL) lista_iter_destruir(iter->lista_iter);
	free(iter);
}
