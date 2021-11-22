#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* La lista está planteada como una lista de punteros genéricos. */
struct lista;
typedef struct lista lista_t;

/*Iterador externo de la lista. */
struct lista_iter;
typedef struct lista_iter lista_iter_t;

/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía o NULL si hubo un problema al crearla.
lista_t *lista_crear(void);

// Devuelve verdadero si la lista no tiene elementos, false en caso contrario.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

//Agrega un elemento al principio de la lista. Devuelve falso en caso de error
//pre: La lista fue creada
//post: Se agrego un nuevo elemento al principio de la lista
bool lista_insertar_primero(lista_t *lista, void *dato);

//Agrega un elemento al final de la lista. Devuelve falso en caso de error
//pre: La lista fue creada
//post: Se agrego un nuevo elemento al final de la lista
bool lista_insertar_ultimo(lista_t *lista, void *dato);

//Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista no estaba vacía.
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del ultimo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el ultimo elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

//Obtiene el largo de la lista.
//Pre: la lista fue creada.
//Post: se devolvio la cantidad de elementos que hay en la lista
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void (*destruir_dato)(void *));

//Itera la lista ejecutando la funcion "visitar" con los datos de la lista.
//Esta funcion "visitar" debe recibir como parametro el puntero al dato almacenado en la lista
//y un puntero a una variable extra para almacenar resultados anteriores de su ejecucion, ademas
//debe devolver un booleano indicando si se debe seguir iterando, con true indica que hay que seguir
//y con false que hay que dejar de iterar.
//pre: la lista fue creada, existe la funcion visitar.
//post: Se ejecuto la funcion visitar con los datos de la lista.
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);



/* ******************************************************************
 *                 PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

//Crea el iterador de una lista
//pre: la lista fue creada.
//post: devuelve el iterador de la lista posicionado en el primer elemento de esta
//		o NULL si hubo algun problema al crearlo.
lista_iter_t *lista_iter_crear(lista_t *lista);

//Avanza un elemento, devuelve true si logro avanzar un elemento, y
//false si no logro avanzar (lo que sigunifica que estaba en el final de la lista).
//pre: el iterador y la lista fueron creados.
bool lista_iter_avanzar(lista_iter_t *iter);

// Obtiene el valor del elemento de la lista en el cual esta el iterador, o NULL si el iterador esta al final.
// Pre: la lista y el iterador fueron creados.
void *lista_iter_ver_actual(const lista_iter_t *iter);

//Devuelve true si el iterador llego al final de la lista, false si no.
//pre: la lista y el iterador fueron creados.
bool lista_iter_al_final(const lista_iter_t *iter);

//Destruye el iterador
//pre: el iterador fue creado.
void lista_iter_destruir(lista_iter_t *iter);

//Inserta un dato en la posicion del elemento donde esta posicionado el iterador.
//Devuelve true si se inserto correctamente y false si hubo algun problema
//pre: la lista y el iterador fueron creados
//post: Si se inserto correctamente el elemento devuelve true, en caso contrario devuelve false.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

//Borra el elemento en el cual esta posicionado el iterador y devuelve su valor o NULL si no habia
//ningun elemento donde estaba posicionado el iterador
//pre: la lista y el iterador fueron creados.
void *lista_iter_borrar(lista_iter_t *iter);


/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación realizada.
void pruebas_lista_estudiante(void);

#endif  // LISTA_H