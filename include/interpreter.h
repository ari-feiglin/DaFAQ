#include "dafaq.h"
#include <sys/mman.h>

#define MAX_CALLS (16)
#define NUM_OF_QUERYERS (5)
#define START_QUERY (-2)
#define END_QUERY (-3)

typedef enum token {FIELD_TABLE, DaFAQ, DIGEST, CONSTIPATE, POOP, DIARRHEA, FROM, GET, MERGE, AT, WHERE, AVERAGE, SUM, AMOUNT}tokens, calls, queryers, functions;

typedef struct query{
    queryers queryers[NUM_OF_QUERYERS];
    functions functions[MAX_CALLS];
    char tables[MAX_CALLS][STRING_LEN];
    char get_fields[MAX_CALLS][STRING_LEN];
    char at_fields[MAX_CALLS][STRING_LEN];
    char conditions[MAX_CALLS][STRING_LEN];
}query;

extern char ** string_tokens;
int num_of_tokens;