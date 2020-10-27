#ifndef DAFAQ
#define DAFAQ

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

typedef enum error_code_e{
    ERROR_CODE_UNINTIALIZED = -1,
    ERROR_CODE_SUCCESS = 0,

    ERROR_CODE_COULDNT_CHANGE_DIR,
    ERROR_CODE_COULDNT_CHMOD,
    ERROR_CODE_COULDNT_MKDIR,
    ERROR_CODE_COULDNT_OPEN,
    ERROR_CODE_COULDNT_RENAME,
    ERROR_CODE_COULDNT_WRITE,
    ERROR_CODE_COULDNT_READ,
    ERROR_CODE_COULDNT_TRUNCATE,
    ERROR_CODE_COULDNT_LSEEK,
    ERROR_CODE_COULDNT_GET_NUM_OF_FIELDS,
    ERROR_CODE_COULDNT_GET_FIELDS,
    ERROR_CODE_COULDNT_GET_NUM_OF_RECORDS,
    ERROR_CODE_COULDNT_GET_RECORD,
    ERROR_CODE_COULDNT_ALLOCATE_MEMORY,
    ERROR_CODE_COULDNT_GET_LEN_OF_RECORD,
    ERROR_CODE_INVALID_INPUT,
    ERROR_CODE_COULDNT_GET_INPUT,
    ERROR_CODE_INVALID_DATATYPE,
    ERROR_CODE_INDEX_OUT_OF_BOUNDS,
    ERROR_CODE_INVALID_FILE,
    ERROR_CODE_FIELD_DOESNT_EXIST,
    ERROR_CODE_OUT_OF_DATE
} error_code_t;

typedef enum datatypes {CHAR=1, INT=4, STRING=STRING_LEN, BOOLEAN=1}datatype;
typedef enum token {FIELD_TABLE, DaFAQ, DIGEST, CONSTIPATE, POOP, DIARRHEA, FROM, GET, MERGE, AT, WHERE, AVERAGE, TSUM, AMOUNT}tokens, calls, queryers, functions;
typedef enum operators {EQUALS=0, NOT_EQUALS, GREATER, LESS, GREATER_OR_EQUALS, LESS_OR_EQUALS}operators;

typedef struct query{
    int table_fds[MAX_CALLS];
    int field_indexes[MAX_CALLS][MAX_CALLS];
    int merge_field_indexes[2];
    int target[MAX_CALLS][MAX_CALLS];
    operators operator[MAX_CALLS][MAX_CALLS];
}query;

typedef struct field{
    char name[NAME_LEN];
    int data_len;
    char input_mask[NAME_LEN];
}field;

typedef struct record_field{
    int record_num;
    int record_field_offset;    //Offset of field relative to the start of the record
    int field_index;
    int data_len;
    char * data;
}record_field;

extern char * operations[];
extern char * magic;
extern char * extension;
extern char * sort_extension;
extern int magic_len;
extern int extension_len;
extern int num_of_operations;

//Creation
int create_database(char * name);
int create_table(char * table_name, int num_of_fields, field * fields);

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
int switch_record(int fd, int record_num, int * input_lens, char ** field_input, int sort_file_fd);

//Queries
error_code_t get_record_field(IN int fd, OUT record_field * target_record_field, IN int record_number, IN int field_num, IN bool preserve_offset);
error_code_t get_record(IN int fd, OUT record_field ** record, IN int record_number, IN bool preserve_offset);
error_code_t get_all_records(IN int fd, OUT record_field *** records, IN bool preserve_offset);
error_code_t quicksort_record_fields(int table_fd, int sort_file_fd, int field_index, bool truncate);
error_code_t binary_search_sort_file(int table_fd, int sort_fd, char * target_data, operators operation, int field_index, bool ** valid_record_indexes);
error_code_t get_valid_record_map(int table_fd, int sort_fd, char * target_data, operators operator, int field_index, int num_of_records, bool ** valid_record_map);

//Pooping
int poop(char * table_name, char * dump_file, bool truncate);
int diarrhea(char * database_name, char * dump_name);

//Interfaces
int create_table_interface(char * table_name);
int switch_record_interface(char * name, int record_num);
error_code_t switch_field_interface(IN char * table_name);
error_code_t query_interface(char * table_name, char * sort_file_name);

//Aggregate functions
int COUNT(int fd, int num_of_records, bool * valid_record_map);
int SUM(int fd, int field_index, int num_of_records, bool * valid_record_map);
double AVG(int fd, int field_index, int num_of_records, bool * valid_record_map);

//Menu
int print_menu();
void print_indent_line(int indent_len, int row_length, bool centered, int r, int g, int b);
void print_title(int row_length);

#endif
