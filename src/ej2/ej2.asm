extern strcmp
global invocar_habilidad

; Completar las definiciones o borrarlas (en este ejercicio NO serán revisadas por el ABI enforcer)
DIRENTRY_NAME_OFFSET EQU ??
DIRENTRY_PTR_OFFSET EQU ??
DIRENTRY_SIZE EQU ??

FANTASTRUCO_DIR_OFFSET EQU ??
FANTASTRUCO_ENTRIES_OFFSET EQU ??
FANTASTRUCO_ARCHETYPE_OFFSET EQU ??
FANTASTRUCO_FACEUP_OFFSET EQU ??
FANTASTRUCO_SIZE EQU ??

section .rodata
; Acá se pueden poner todas las máscaras y datos que necesiten para el ejercicio

section .text

; void invocar_habilidad(void* carta, char* habilidad);
invocar_habilidad:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; r/m64 = void*    card ; Vale asumir que card siempre es al menos un card_t*
	; r/m64 = char*    habilidad

	ret ;No te olvides el ret!
