#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static size_t test__count = 0;
static size_t test__succeed__count = 0;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

extern void abienf_regsave(void*);
#if WITH_ABI_ENFORCER
static char *REGS[] = {"rbx", "rbp", "r12", "r13", "r14", "r15"};
static uint64_t abienf_regs[2][6];

static void abienf_begin() {
	abienf_regsave(abienf_regs[0]);
}

static void abienf_end(bool *failed) {
	abienf_regsave(abienf_regs[1]);
	for (size_t i=0; i<6; i++) {
		if (abienf_regs[0][i] != abienf_regs[1][i]) {
			// *failed = 1;
			printf(ANSI_COLOR_RED "\nerror: [ABI_enforcer] No se preservó el valor de un registro no-volatil." ANSI_COLOR_RESET "\n");
		}
	}
}

static void* abienf_held_p;
static int   abienf_held_i;

#define TEST_CALL_V(fn, ...) (abienf_begin(), fn(__VA_ARGS__), abienf_end(test__fallo))
#define TEST_CALL_I(fn, ...) (abienf_begin(), abienf_held_i = fn(__VA_ARGS__), abienf_end(test__fallo), abienf_held_i)
#define TEST_CALL_S(fn, ...) (abienf_begin(), abienf_held_p = fn(__VA_ARGS__), abienf_end(test__fallo), abienf_held_p)
#else
#define TEST_CALL_V(fn, ...) fn(__VA_ARGS__)
#define TEST_CALL_I(fn, ...) fn(__VA_ARGS__)
#define TEST_CALL_S(fn, ...) fn(__VA_ARGS__)
#endif

#define TEST(name)                                            \
void name##__impl(const char* test__name, bool* test__fallo, char assert_name[1024]); \
void name() {                                                 \
	test__count++;                                        \
	char assert_name[1024] = {0};                         \
	const char* test__name = #name;                       \
	bool test__fallo = false;                             \
	printf("- %s", test__name);                           \
	name##__impl(test__name, &test__fallo, assert_name);  \
	if (!test__fallo) {                                   \
		test__succeed__count++;                       \
		printf(" OK\n");                              \
	}                                                     \
}                                                             \
void name##__impl(const char* test__name, bool* test__fallo, char assert_name[1024])

#define TEST_ASSERT(cond)                                           \
	if (!(cond)) {                                              \
		printf(" FAILED\n");                                \
		if (assert_name[0] == '\0') {                       \
			strcpy(assert_name, #cond);                 \
		}                                                   \
		printf("    al probar %s\n", assert_name);          \
		printf("    fallo en %s:%d\n", __FILE__, __LINE__); \
		printf("        Condición: %s\n", #cond);           \
		*test__fallo = true;                                \
		return;                                             \
	}                                                           \
	assert_name[0] = '\0'

#define TEST_ASSERT_EQUALS(type, expected, got)                     \
	if ((type)(expected) != (type)(got)) {                      \
		char format[1024];                                  \
		printf(" FAILED\n");                                \
		if (assert_name[0] == '\0') {                       \
			strcpy(assert_name, #expected " == " #got); \
		}                                                   \
		printf("    al probar %s\n", assert_name);          \
		printf("    fallo en %s:%d\n", __FILE__, __LINE__); \
		printf("        Esperado: ");                       \
		PRINT_VALUE(type, (type)(expected));                \
		printf("\n");                                       \
		printf("        Recibido: ");                       \
		PRINT_VALUE(type, (type)(got));                     \
		printf("\n");                                       \
		*test__fallo = true;                                \
		return;                                             \
	}                                                           \
	assert_name[0] = '\0'

/* Dumb generic printing mechanism */
static void print_int32_t(int32_t v)  { printf("%d",   v); }
static void print_uint32_t(uint32_t v) { printf("%u",   v); }
static void print_string(char* v)      { printf("%s",   v); }
static void print_float(float v)       { printf("%.2f", v); }
static void print_double(double v)     { printf("%.2f", v); }

#define PRINT_VALUE(type, value) _Generic(*((type*)NULL), \
	int32_t:  print_int32_t,         \
	uint32_t: print_uint32_t,        \
	char*:    print_string,          \
	float:    print_float,           \
	double:   print_double           \
)(value)

static inline void tests_end(char* test_suite_name) {
	printf(
		"Pasaron %ld de %ld tests\n",
		test__succeed__count,
		test__count
	);
	if (test__count == test__succeed__count) {
		printf("¡Pasaron todos los tests de %s!\n", test_suite_name);
		exit(0);
	} else {
		printf("Fallaron algunos tests de %s.\n", test_suite_name);
		exit(1);
	}
}

void error_unaligned_stack() {
	printf(ANSI_COLOR_RED "\nerror: [ABI_enforcer] Se incumplió ABI. Abortando..." ANSI_COLOR_RESET "\n");
}
