#pragma once

#define EJ2
#include "../ej1/ej1.h"

typedef struct {
	directory_t __dir;
	uint16_t __dir_entries;
	void* __archetype;
} card_t;

typedef struct {
	directory_t __dir;
	uint16_t __dir_entries;
	fantastruco_t* __archetype;
	uint32_t materials;
} alucard_t;

typedef void ability_function_t(void* card);

void invocar_habilidad(void* carta, char* habilidad);
