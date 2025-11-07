# Recu Nicolas Pedro Bruzza
## crear_pareja()
Para esta syscall primero debemos agregar su entrada a la IDT, la cual debera tener su dpl de nivel 3 ya que queremos que nuestros usuarios accedan a ella
```c
IDTENTRY_3(99);
```
Con esto en nuestro idt_init ya tenemos creada nuestro interrupt gate para poder llamar a esta syscall, ahora deberiamos modificar nuestro isr con el codigo de la nueva interrupcion
```asm
global _isr99
_isr99:
    pushad

    call estoy_en_pareja
    cmp al, 0 ;Nota, asumo que cero corresponde a VACIO del enumerable estados_t

    jne .emparejar: ;Si estoy_en_pareja() != 0, significa que ya estoy en pareja o ya estaba esperando una pareja
    popad
    iret

    .emparejar:
    call buscar_pareja
    call sched_next_task ;No hace falta que checkee si la siguiente task es igual que la actual, ya que voy a hacer que buscar_pareja pausee la tarea actual
    mov WORD [sched_task_selector], ax
    jmp far [sched_task_offset]

    popad
    iret
```

### Funciones y estructuras auxiliares
Para guardar los datos de mis parejas, voy a crear un array de una estructura que contenga los datos de mis parejas actuales
```c
typedef enum{
    VACIO, //Asumo = 0
    PAREJA, //Asumo = 1
    LIDER, //Asumo = 2
    ESPERANDO_PAREJA //Asumo = 3
} estados_t;
typedef struct {
    estados_t estado;
    uint8_t pareja;
} parejas_entry_t; //Pareja es igual al id de la tarea emparejada

static parejas_entry_t parejas[MAX_TASKS] = {0};
//Este array me debera indicar el estado de la tarea cuyo id coincide con el indice
```
```c
uint_8 estoy_en_pareja(void){
    return parejas[current_task].estado;
}

void buscar_pareja(void){
    parejas[current_task].estado = ESPERANDO_PAREJA;
    sched_disable_task(current_task);
}
```

## juntarse_con(id_tarea)
Nuevamente, tengo que agregarla a mi idt con IDTENTRY_3(100); y agregar el codigo a mi isr.
Como no hay convencion para las syscalls, le voy a pedir a mi usuario que me pase por el registro EAX el id de la tarea a emparejar y el resultado tambien va a ser retornado por EAX
```asm
global _isr100
_isr100:
    pushad

    push EAX
    call juntarse_con

    add ESP, 4 ;Borro el push de la pila
    mov [ESP + 28], EAX ;Escribo el resultado de juntarse_con en EAX

    popad
    iret
```

```c
uint32_t juntarse_con(uint32_t id_tarea){
    if (parejas[current_task].estado != 0){
        return 1; //Mi tarea ya estaba en pareja
    }
    if (parejas[id_tarea].estado != ESPERANDO_PAREJA){
        return 1; //Mi tarea a emparejar no estaba esperando una tarea
    }
    //Si llego aca, significa que puedo crear la pareja
    parejas[current_task].estado = PAREJA;
    parejas[current_task].pareja = id_tarea;
    parejas[id_tarea].estado = LIDER;
    parejas[id_tarea].pareja = current_task;
    sched_enable_task(id_tarea);
    return 0; //Emparejados
}
```
Antes de continuar con abandonar_pareja, tengo que modificar mi codigo de la page_fault_handler para poder mapear las direcciones compartidas una vez sean requeridas por las tareas

```c
bool page_fault_handler(vaddr_t virt) {
  print("Atendiendo page fault...", 0, 0, C_FG_WHITE | C_BG_BLACK);
  // Chequeemos si el acceso fue dentro del area on-demand
  if(virt >= ON_DEMAND_MEM_START_VIRTUAL && virt <= ON_DEMAND_MEM_END_VIRTUAL){  // En caso de que si, mapear la pagina
    uint32_t cr3 = rcr3();
    uint32_t attr_ondemand = 1 | 1 << 1 | 1 << 2 | 0 << 3 | 0 << 4 | 0 << 5 | 0 << 6 | 0 << 7 | 0 << 8 | 0 << 9 ;
    mmu_map_page(cr3, ON_DEMAND_MEM_START_VIRTUAL, ON_DEMAND_MEM_START_PHYSICAL, attr_ondemand);
    return 1;
  }
  if(virt >= 0xC0C00000 && virt <= 0xC0FFFFFF && parejas[current_task].estado != VACIO){
    //Si estoy en mi rango de memoria emparejada y estoy en pareja, deberia asignarla cuando sea necesaria
    uint32_t cr3 = rcr3();
    uint32_t P = 0; 
    if(parejas[current_task].estado == LIDER){
        P = 1;
    }
    //P son los privilegios que deberia tener, si soy lider puedo escribir (P = 1), sino soy pareja y no puedo escribir
    uint32_t attr = 1 | P << 1 | 1 << 2 | 0 << 3 | 0 << 4 | 0 << 5 | 0 << 6 | 0 << 7 | 0 << 8 | 0 << 9 ;
    paddr_t dir_fisica = next_free_user_page();
    mmu_map_page(cr3, virt, dir_fisica, attr);
    //Con esto ya esta mapeada mi direccion en mi tarea actual, ahora deberia mapearla en mi pareja
    uint32_t pareja_id = parejas[current_task].pareja;
    uint32_t pareja_cr3 = tss_tasks[pareja_id].cr3;
    //Los attributos de la pagina de la tarea que estoy mapeando deben ser opuestos a la de la tarea actual
    if(P = 0) P = 1; else P = 0;
    uint32_t attr = 1 | P << 1 | 1 << 2 | 0 << 3 | 0 << 4 | 0 << 5 | 0 << 6 | 0 << 7 | 0 << 8 | 0 << 9 ;
    mmu_map_page(pareja_cr3, virt, dir_fisica, attr);
    //Por ultimo la limpio
    zero_page(virt);
    return 1;
  }
  return 0;
}
```
Con esto ya deberia funcionar el mapeo de paginas

## abandonar_pareja()
Otra vez debo agregar un IDTENTRY_3(101) a mi idt_init y cambiar el codigo de mi interrupcion
```asm
global _isr101
_isr101:
    pushad

    call abandonar_pareja
    cmp AL, 0; Si abandonar pareja retorna 0, significa que esta tarea esta pausada
    jne .fin
    ; Aca deberia saltar a otra tarea, ya que si AL = 0 significa que esta tarea esta pausada

    call sched_next_task
    mov WORD [sched_task_selector], ax
    jmp far [sched_task_offset]

    ; Una vez retorne aca, significa que esta tarea ya no esta en pareja y fue reanudada, por lo tanto tengo que liberar la memoria compartida
    call librerar_memoria_pareja

    .fin:
    popad
    iret
```

```c
uint8_t abandonar_pareja(void){
    if(parejas[current_task].estado == PAREJA){
        //Si mi tarea es la pareja, solamente tengo que dejar de poder acceder a esas posiciones de memoria y actualizar los estados de las parejas
        parejas[current_task].estado = VACIO;
        uint32_t cr3 = rcr3();
        pd_entry_t* directorio = (pd_entry_t*) (CR3_TO_PAGE_DIR(cr3));
        pt_entry_t* tabla = (pt_entry_t*) directorio[0x303]->pt //Indice correspondiente a las direcciones a partir de 0xC0C00000
        //Borro la tabla entera
        for(uint32_t i = 0; i < 1024; i++){
            tabla[i]->attrs = 0;
        }
        //Ahora tengo que actualizar el estado de mi lider, si este esta pausado significa que estaba esperandome para reanudar, sino simplemente puedo actualizar mi estado y continuar
        uint32_t lider = parejas[current_task].pareja;
        if(sched_tasks[lider].state == TASK_PAUSED){
            sched_enable_task(lider);
        }
        return 1;
    }
    else if(parejas[current_task].estado == LIDER){
        //Si mi tarea es la lider, tengo que ver si mi pareja ya libero su memoria
        uint32_t pareja = parejas[current_task].pareja;
        if(parejas[pareja].state == VACIO){
            //Si mi pareja ya abandono no hace falta que pause la tarea, solo tengo que borrar la memoria
            parejas[current_task].estado = VACIO;
            uint32_t cr3 = rcr3();
            pd_entry_t* directorio = (pd_entry_t*) (CR3_TO_PAGE_DIR(cr3));
            pt_entry_t* tabla = (pt_entry_t*) directorio[0x303]->pt  //Indice correspondiente a las direcciones a partir de 0xC0C00000
            //Borro la tabla entera
            for(uint32_t i = 0; i < 1024; i++){
                tabla[i]->attrs = 0;
            }
            return 1;
        }
        else{
            //Sino tengo que pausar esta tarea y esperar a que la pareja libere
            sched_disable_task(current_task);
            return 0;
        }
    }
    return 1; //Si caigo aca, significa que llame a abandonar_pareja sin estar en pareja
}

void liberar_memoria_pareja(void){
    parejas[current_task].estado = VACIO;
    uint32_t cr3 = rcr3();
    pd_entry_t* directorio = (pd_entry_t*) (CR3_TO_PAGE_DIR(cr3));
    pt_entry_t* tabla = (pt_entry_t*) directorio[0x303]->pt //Indice correspondiente a las direcciones a partir de 0xC0C00000
    //Borro la tabla entera
    for(uint32_t i = 0; i < 1024; i++){
        tabla[i]->attrs = 0;
    }
}
```

## Segundo ejercicio, uso_de_memoria_de_las_parejas()
Como esta funcion no necesita ser llamada por syscall y solo puede ser accedida por el kernel, no necesito modificar la idt ni mi isr
```c
uint32_t uso_de_memoria_de_las_parejas(void){
    uint32_t cont = 0;
    //Debido a que la memoria siempre va a quedar guardada en la pdir de lider hasta que termine la pareja, solo necesito revisar mi array de estados de parejas y ver cuantas paginas tiene mapeadas cada lider
    for(uint32_t i = 0; i < MAX_TASKS; i++){
        if(parejas[i].estados == LIDER){
            uint32_t cr3 = tss_tasks[i].cr3
            pd_entry_t* directorio = (pd_entry_t*) (CR3_TO_PAGE_DIR(cr3));
            pt_entry_t* tabla = directorio[0x303]->pt;
            //Obtengo la page table del directorio correspondiente a 0xC0C00000
            for(uint32_t j = 0; j < 1024; j++){
                if((tabla[j]->attrs && 1) != 0){
                    //Si el attributo presente es diferente de 0 significa que hay algo presente en esa entrada de la tabla, por lo tanto hay 4 kb asignados 
                    cont++;
                }
            }
        }
    }
    return cont << 12; //Nota, devuelve el resultado en bytes
}
```