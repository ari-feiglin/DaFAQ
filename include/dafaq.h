#include "CColor.h"
#include "standard.h"
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define STRING_LEN (256)
#define NAME_LEN (32)
#define MAX_CALLS (16)
#define NUM_OF_QUERYERS (5)

typedef enum datatypes {CHAR=1, INT=4, STRING=STRING_LEN, BOOLEAN=1}datatype;
typedef enum token {FIELD_TABLE, DaFAQ, DIGEST, CONSTIPATE, POOP, DIARRHEA, FROM, GET, MERGE, AT, WHERE, AVERAGE, SUM, AMOUNT}tokens, calls, queryers, functions;

typedef struct query{
    queryers queryers[NUM_OF_QUERYERS];
    functions functions[MAX_CALLS];
    char tables[MAX_CALLS][STRING_LEN];
    char get_fields[MAX_CALLS][STRING_LEN];
    char at_fields[MAX_CALLS][STRING_LEN];
    char conditions[MAX_CALLS][STRING_LEN];
}query;

typedef struct field{
    char name[NAME_LEN];
    int data_len;
    char input_mask[NAME_LEN];
}field;

char * magic;
char * extension;
int magic_len;
int extension_len;

//Creation
int create_database(char * name);
int create_table(char * table_name);

//Essential operations
bool check_magic(int fd, bool preserve_offset);
int get_num_of_fields(int fd, bool preserve_offset);
int get_fields(int fd, field ** fields, bool preserve_offset);
int get_num_of_records(int fd, int num_of_fields, bool preserve_offset);
bool check_extension(char * table_name);
bool check_input_mask(char * input_mask);
int valid_input(char * input, char * input_mask);
int get_len_of_record(int fd, bool preserve_offset);

//Edit data
int switch_field(char * file_name, char * field_name, int data_size, char * input_mask, int field_num);
int switch_record(int fd, int record_num, int * input_lens, char ** field_input);

//Queries
int poop(char * table_name, char * dump_file, bool truncate);
int diarrhea(char * database_name, char * dump_name);

//Interfaces
int edit_record_interfaces(char * name, int record_num);
