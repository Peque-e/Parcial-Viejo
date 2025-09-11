#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../test_utils/test-utils.h"
#include "ej2.h"

void *ultima_dijo_hola = NULL;
void *ultima_dijo_chau = NULL;

void *ultima_comida = NULL;
size_t saludos_abiertos = 0;

/**
 * Cuenta cuántos tests corrieron exitosamente.
 */
uint64_t successful_tests = 0;
/**
 * Cuenta cuántos tests test fallaron.
 */
uint64_t failed_tests = 0;

/**
 * El mensaje [DONE] escrito en verde.
 */
#define DONE "[\033[32;1mDONE\033[0m] "

/**
 * El mensaje [FAIL] escrito en rojo.
 */
#define FAIL "[\033[31;1mFAIL\033[0m] "

/**
 * El mensaje [INFO] escrito en amarillo.
 */
#define INFO "[\033[33;1mINFO\033[0m] "

/**
 * El mensaje [SKIP] escrito en magenta.
 */
#define SKIP "[\033[95;1mSKIP\033[0m] "

void comer_algo(void* carta) {
	printf("yummy!    desde %p\n", carta);
	ultima_comida = carta;
}

void decir_hola(void* carta) {
	printf("hola, soy carta %p\n", carta);
	ultima_dijo_hola = carta;
	saludos_abiertos += 1;
}

void decir_chau(void* carta) {
	printf("chau, soy carta %p\n", carta);
	ultima_dijo_chau = carta;
	saludos_abiertos -= 1;
}

directory_entry_t *crear_entry(char* nombre, ability_function_t fp) {
	directory_entry_t *entry = malloc(sizeof(directory_entry_t));
	strncpy(entry->ability_name, nombre, 10);
	entry->ability_name[9] = '\0';
	entry->ability_ptr = fp;
	return entry;
}

fantastruco_t* crear_carta_con_hola_y_chau() {
	// sin arquetipo

	directory_entry_t *entry_hola = crear_entry("hola", decir_hola);
	directory_entry_t *entry_chau = crear_entry("chau", decir_chau);
	uint16_t cant_habilidades = 2;

	directory_entry_t **dir = malloc(sizeof(void*) * cant_habilidades);
	dir[0] = entry_hola;
	dir[1] = entry_chau;

	fantastruco_t *carta = malloc(sizeof(fantastruco_t));
	carta->__dir = dir;
	carta->__dir_entries = cant_habilidades;
	carta->__archetype = NULL;
	carta->face_up = 0;

	return carta;
}

fantastruco_t* crear_carta_sin_habilidades(fantastruco_t *arq) {
	// sin arquetipo
	directory_entry_t **dir = (directory_entry_t **) 0xFE0FE0FE0;
	fantastruco_t *carta = malloc(sizeof(fantastruco_t));
	carta->__dir = dir;
	carta->__dir_entries = 0;
	carta->__archetype = arq;
	carta->face_up = 0;
	return carta;
}

fantastruco_t* crear_carta_restaurant_con_arquetipo(fantastruco_t *arq) {
	directory_entry_t *entry_pizza = crear_entry("pizza", comer_algo);
	directory_entry_t *entry_pasta = crear_entry("pasta", comer_algo);
	uint16_t cant_habilidades = 2;

	directory_entry_t **dir = malloc(sizeof(void*) * cant_habilidades);
	dir[0] = entry_pizza;
	dir[1] = entry_pasta;

	fantastruco_t *carta = malloc(sizeof(fantastruco_t));
	carta->__dir = dir;
	carta->__dir_entries = cant_habilidades;
	carta->__archetype = arq;
	carta->face_up = 0;

	return carta;
}

void borrar_carta(fantastruco_t* carta) {
	// sin arquetipo
	for (int i=0; i < carta->__dir_entries; i++) {
		free(carta->__dir[i]);
	}
	if(carta->__dir_entries != 0)
		free(carta->__dir);
	free(carta);
}

void cleanup() {
	ultima_dijo_hola = NULL;
	ultima_dijo_chau = NULL;
	ultima_comida = NULL;
	saludos_abiertos = 0;
}

void check_clean_globals() {
	assert(ultima_dijo_hola == NULL);
	assert(ultima_dijo_chau == NULL);
	assert(ultima_comida == NULL);
	assert(saludos_abiertos == 0);
}

void test_invocar_habilidades_sin_arquetipo() {
	fantastruco_t *cartita = crear_carta_con_hola_y_chau();

	bool failed = false;

	check_clean_globals();

	invocar_habilidad(cartita, "hola");
	
	failed = (ultima_dijo_hola == cartita) ? failed : true;
	failed = (saludos_abiertos == 1) ? failed : true;
	failed = (ultima_dijo_chau == NULL) ? failed : true;

	invocar_habilidad(cartita, "chau");
	
	failed = (ultima_dijo_chau == cartita) ? failed : true;
	failed = (saludos_abiertos == 0) ? failed : true;

	cleanup();
	borrar_carta(cartita);

	if (failed) {
		printf(FAIL "test_invocar_habilidades_sin_arquetipo(){\n");
		failed_tests++;
	} else {
		printf(DONE "test_invocar_habilidades_sin_arquetipo()\n");
		successful_tests++;
	}
}

void test_invocar_habilidad_especial_carta_con_arquetipo() {
	fantastruco_t *cartita = crear_carta_con_hola_y_chau();
	fantastruco_t *carta2 = crear_carta_restaurant_con_arquetipo(cartita);
	
	bool failed = false;
	
	check_clean_globals();

	invocar_habilidad(carta2, "pizza");
	failed = (ultima_comida == carta2) ? failed : true;
	failed = (ultima_dijo_hola == NULL) ? failed : true;
	failed = (ultima_dijo_chau == NULL) ? failed : true;
	failed = (saludos_abiertos == 0) ? failed : true;

	cleanup();
	borrar_carta(carta2);
	borrar_carta(cartita);

	if (failed) {
		printf(FAIL "test_invocar_habilidad_especial_carta_con_arquetipo(){\n");
		failed_tests++;
	} else {
		printf(DONE "test_invocar_habilidad_especial_carta_con_arquetipo()\n");
		successful_tests++;
	}
}

void test_invocar_habilidades_compartidas_carta_con_arquetipo() {
	fantastruco_t *cartita = crear_carta_con_hola_y_chau();
	fantastruco_t *carta2 = crear_carta_restaurant_con_arquetipo(cartita);
	
	bool failed = false;
	
	check_clean_globals();

	invocar_habilidad(carta2, "hola");

	failed = (ultima_dijo_hola == cartita) ? failed : true;
	failed = (saludos_abiertos == 1) ? failed : true;
	failed = (ultima_dijo_chau == NULL) ? failed : true;
	failed = (ultima_comida == NULL) ? failed : true;

	invocar_habilidad(carta2, "chau");

	failed = (ultima_dijo_chau == cartita) ? failed : true;
	failed = (saludos_abiertos == 0) ? failed : true;
	failed = (ultima_comida == NULL) ? failed : true;

	cleanup();
	borrar_carta(carta2);
	borrar_carta(cartita);

	if (failed) {
		printf(FAIL "test_invocar_habilidades_compartidas_carta_con_arquetipo(){\n");
		failed_tests++;
	} else {
		printf(DONE "test_invocar_habilidades_compartidas_carta_con_arquetipo()\n");
		successful_tests++;
	}
}

void test_cadena_arquetipos() {
	fantastruco_t *carta1 = crear_carta_con_hola_y_chau();
	fantastruco_t *carta2 = crear_carta_restaurant_con_arquetipo(carta1);
	fantastruco_t *carta3 = crear_carta_restaurant_con_arquetipo(carta2);
	fantastruco_t *carta4 = crear_carta_restaurant_con_arquetipo(carta3);

	bool failed = false;

	check_clean_globals();

	invocar_habilidad(carta4, "hola");

	failed = (ultima_dijo_hola == carta1) ? failed : true;
	failed = (saludos_abiertos == 1) ? failed : true;
	failed = (ultima_dijo_chau == NULL) ? failed : true;
	failed = (ultima_comida == NULL) ? failed : true;

	cleanup();
	borrar_carta(carta4);
	borrar_carta(carta3);
	borrar_carta(carta2);
	borrar_carta(carta1);

	if (failed) {
		printf(FAIL "test_cadena_arquetipos(){\n");
		failed_tests++;
	} else {
		printf(DONE "test_cadena_arquetipos()\n");
		successful_tests++;
	}
}

void test_invocar_habilidad_inexistente_carta_con_arquetipo() {
	fantastruco_t *cartita = crear_carta_con_hola_y_chau();
	fantastruco_t *carta2 = crear_carta_restaurant_con_arquetipo(cartita);
	
	bool failed = false;
	
	check_clean_globals();

	invocar_habilidad(carta2, "esta_habilidad_no_existe");

	failed = (ultima_dijo_hola == NULL) ? failed : true;
	failed = (ultima_dijo_chau == NULL) ? failed : true;
	failed = (saludos_abiertos == 0) ? failed : true;
	failed = (ultima_comida == NULL) ? failed : true;

	cleanup();
	borrar_carta(carta2);
	borrar_carta(cartita);

	if (failed) {
		printf(FAIL "test_invocar_habilidad_inexistente_carta_con_arquetipo(){\n");
		failed_tests++;
	} else {
		printf(DONE "test_invocar_habilidad_inexistente_carta_con_arquetipo()\n");
		successful_tests++;
	}
}

void test_invocar_carta_sin_habilidades() {
	fantastruco_t *cartita = crear_carta_sin_habilidades(NULL);
	bool failed = false;
	
	check_clean_globals();

	invocar_habilidad(cartita, "esta_habilidad_no_existe");

	failed = (ultima_dijo_hola == NULL) ? failed : true;
	failed = (ultima_dijo_chau == NULL) ? failed : true;
	failed = (saludos_abiertos == 0) ? failed : true;
	failed = (ultima_comida == NULL) ? failed : true;

	cleanup();
	borrar_carta(cartita);

	if (failed) {
		printf(FAIL "test_invocar_carta_sin_habilidades(){\n");
		failed_tests++;
	} else {
		printf(DONE "test_invocar_carta_sin_habilidades()\n");
		successful_tests++;
	}
}

int main() {
	test_invocar_habilidades_sin_arquetipo();
	test_invocar_habilidad_especial_carta_con_arquetipo();
	test_invocar_habilidades_compartidas_carta_con_arquetipo();
	test_invocar_habilidad_inexistente_carta_con_arquetipo();
	test_invocar_carta_sin_habilidades();
	test_cadena_arquetipos();
}

