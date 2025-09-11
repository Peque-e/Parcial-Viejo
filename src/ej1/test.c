#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../test_utils/test-utils.h"
#include "ej1.h"

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

bool check_directory(directory_t dir) {
	bool failed = false;
	if (!strcmp("sleep",dir[0]->ability_name)) {
		failed = dir[0]->ability_ptr == &sleep ? failed : true;
		failed = (!strcmp("wakeup", dir[1]->ability_name)) ? failed : true;
		failed = dir[1]->ability_ptr == &wakeup ? failed : true;
	} else if (!strcmp("wakeup", dir[0]->ability_name) ) {
		failed = dir[0]->ability_ptr == &wakeup ? failed : true;
		failed = (!strcmp("sleep", dir[1]->ability_name)) ? failed : true;
		failed = dir[1]->ability_ptr == &sleep ? failed : true;
	} else {
		failed = true;
	}
	return failed;
}

void free_dir(directory_t dir, uint16_t dir_entries) {
	for (int i=0; i < dir_entries; i++)
		free(dir[i]);
	free(dir);
}

/**
 * Evalúa los tests del ejercicio 1A. Este ejercicio requiere implementar
 * `init_fantastruco_dir`.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_1A_HECHO`.
 */
void ej1a_init_fantastruco_dir() {
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_1A_HECHO) {
		printf(SKIP "El ejercicio 1A no está hecho aún.\n");
		return;
	}

	fantastruco_t* card = &(fantastruco_t){
		.__dir = NULL,
		.__dir_entries = 9999,
		.__archetype = NULL,
		.face_up = 9999,
	};

	bool failed = false;

	init_fantastruco_dir(card);
	failed = card->__dir_entries == 2 ? failed : true;
	directory_t dir = card->__dir;
	if (!failed) {
		failed = check_directory(dir);
	}

	if (failed) {
		printf(FAIL "ej1a_init_fantastruco_dir{\n");
		for (int i=0; i<card->__dir_entries; i++){
			if (i<2) {
				printf("[%d] Got {.ability_name=%s, .ability_ptr=%p}\n",i, dir[i]->ability_name, dir[i]->ability_ptr);
				printf("Expected {.ability_name=%s, .ability_ptr=", "sleep or wakeup");
				printf("%p or %p}\n", &sleep, &wakeup);
			} else {
				printf("[%d] Got {.ability_name=%s, .ability_ptr=%p}\n",i, dir[i]->ability_name, dir[i]->ability_ptr);
				printf("Expected NULL\n");
			}
		}
		failed_tests++;
	} else {
		printf(DONE "ej1a_init_fantastruco_dir\n");
		successful_tests++;
	}

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 1A tuvo tests que fallaron.\n");
	}

	free_dir(card->__dir, card->__dir_entries);
	return;
}

/**
 * Evalúa los tests del ejercicio 1B. Este ejercicio requiere implementar
 * `summon_fantastruco`.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_1B_HECHO`.
 */
void ej1b_summon_fantastruco() {
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_1B_HECHO) {
		printf(SKIP "El ejercicio 1B no está hecho aún.\n");
		return;
	}

	bool failed = false;
	
	fantastruco_t* card = summon_fantastruco();
	failed = card->__archetype == NULL ? failed : true;
	failed = card->face_up == 1 ? failed : true;

	if (failed) {
		printf(FAIL "ej1b_summon_fantastruco_dir(){\n");
		printf("Got {.__archetype=%p, .face_up=%d}\n", card->__archetype, card->face_up);
		failed_tests++;
	} else {
		printf(DONE "ej1b_summon_fantastruco_dir()\n");
		successful_tests++;
	}

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 1B tuvo tests que fallaron.\n");
	}

	free_dir(card->__dir, card->__dir_entries);
	free(card);
	return;
}

/**
 * Evalúa los tests del ejercicio 1. Este ejercicio requiere implementar
 * `init_fantastruco_dir` y `summon_fantastruco`.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_1A_HECHO` ó a `EJERCICIO_1B_HECHO`.
 */
void ej1ab_init_dir_and_summon_fantastruco() {
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_1A_HECHO || !EJERCICIO_1B_HECHO) {
		printf(SKIP "El ejercicio 1 no tiene ambos incisos hechos aún.\n");
		return;
	}

	bool failed = false;

	fantastruco_t* card = summon_fantastruco();
	failed = card->__archetype == NULL ? failed : true;
	failed = card->face_up == 1 ? failed : true;

	directory_t dir = card->__dir;

	if (!failed) {
		failed = check_directory(dir);
	}

	if (failed) {
		directory_t dir = card->__dir;
		printf(FAIL "ej1ab_init_dir_and_summon_fantastruco(){\n");
		printf("Got {.__archetype=%p, .face_up=%d, .__dir_entries=%d}\n", card->__archetype, card->face_up, card->__dir_entries);
		printf("card.__dir:\n");
		for (int i=0; i<card->__dir_entries; i++){
			if (i<2) {
				printf("[%d] Got {.ability_name=%s, .ability_ptr=%p}\n",i, dir[i]->ability_name, dir[i]->ability_ptr);
				printf("Expected {.ability_name=%s, .ability_ptr=", "sleep or wakeup");
				printf("%p or %p}\n", &sleep, &wakeup);
			} else {
				printf("[%d] Got {.ability_name=%s, .ability_ptr=%p}\n",i, dir[i]->ability_name, dir[i]->ability_ptr);
				printf("Expected NULL\n");
			}
		}		
		failed_tests++;
	} else {
		printf(DONE "ej1ab_init_dir_and_summon_fantastruco()\n");
		successful_tests++;
	}

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 1 tuvo tests que fallaron.\n");
	}

	free_dir(card->__dir, card->__dir_entries);
	free(card);
	return;
}

void ej1ab_different_cards() {
	if (!EJERCICIO_1A_HECHO || !EJERCICIO_1B_HECHO) {
		printf(SKIP "El ejercicio 1 no tiene ambos incisos hechos aún.\n");
		return;
	}
	bool failed = false;

	fantastruco_t *card1 = summon_fantastruco();
	fantastruco_t *card2 = summon_fantastruco();

	card1->face_up = 0;
	failed |= card2->face_up != 1;

	free_dir(card1->__dir, card1->__dir_entries);
	free_dir(card2->__dir, card2->__dir_entries);
	free(card1);
	free(card2);

	if (failed) {
		printf(FAIL "ej1ab_different_cards()\n");
		failed_tests++;
	} else {
		printf(DONE "ej1ab_different_cards()\n");
		successful_tests++;
	}
	return;
}

/**
 * Corre los tests de este ejercicio.
 *
 * Las variables `EJERCICIO_1A_HECHO` y `EJERCICIO_1B_HECHO` controlan qué
 * testsuites se van a correr. Asignarles `false` indica que el ejercicio no
 * está implementado y por lo tanto no se ejecutarán sus tests.
 * a él.
 *
 * Si algún test falla el programa va a terminar con un código de error.
 */
int main(int argc, char* argv[]) {
	ej1a_init_fantastruco_dir();
	ej1b_summon_fantastruco();
	ej1ab_init_dir_and_summon_fantastruco();
	ej1ab_different_cards();

	printf(
		"\nSe corrieron %lu tests. %lu corrieron exitosamente. %lu fallaron.\n",
		failed_tests + successful_tests, successful_tests, failed_tests
	);

	if (failed_tests || !EJERCICIO_1A_HECHO || !EJERCICIO_1B_HECHO) {
		return 1;
	} else {
		return 0;
	}
}
