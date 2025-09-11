#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
	char ability_name[10]; //asmdef_offset:DIRENTRY_NAME_OFFSET
	void* ability_ptr; //asmdef_offset:DIRENTRY_PTR_OFFSET
} directory_entry_t; //asmdef_size:DIRENTRY_SIZE

typedef directory_entry_t** directory_t;

typedef struct {
	directory_t __dir; //asmdef_offset:FANTASTRUCO_DIR_OFFSET
	uint16_t __dir_entries; //asmdef_offset:FANTASTRUCO_ENTRIES_OFFSET
	void* __archetype; //asmdef_offset:FANTASTRUCO_ARCHETYPE_OFFSET
	bool face_up; //asmdef_offset:FANTASTRUCO_FACEUP_OFFSET
} fantastruco_t; //asmdef_size:FANTASTRUCO_SIZE

/**
 * Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - init_fantastruco_dir
 */
extern bool EJERCICIO_1A_HECHO;

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - summon_fantastruco
 */
extern bool EJERCICIO_1B_HECHO;

/**
 * OPCIONAL: implementar en C
 */
void init_fantastruco_dir(fantastruco_t* card);

/**
 * OPCIONAL: implementar en C
 */
fantastruco_t* summon_fantastruco();

// Funciones ya provistas
void sleep(void* card);
void wakeup(void* card);
directory_entry_t* create_dir_entry(char* ability_name, void* ability_ptr);
