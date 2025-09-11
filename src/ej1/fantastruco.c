#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "ej1.h"

void sleep(void* card) {
    fantastruco_t* fantastruco = card;
    fantastruco->face_up = 0;
}

void wakeup(void* card) {
    fantastruco_t* fantastruco = card;
    fantastruco->face_up = 1;
}

directory_entry_t* create_dir_entry(char* ability_name, void* ability_ptr) {
    directory_entry_t* dir_entry = (directory_entry_t*) malloc(sizeof(directory_entry_t));
    memset(dir_entry->ability_name, 0, 10);
    strncpy(dir_entry->ability_name, ability_name, 9);
    dir_entry->ability_ptr = ability_ptr;
    return dir_entry;
}
