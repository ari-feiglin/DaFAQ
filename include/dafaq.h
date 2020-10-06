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

typedef enum datatypes {CHAR=1, INT=4, STRING=STRING_LEN, BOOLEAN=1}datatype;

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

//Edit data
int switch_field(char * file_name, char * field_name, int data_size, char * input_mask, int field_num);
int add_record(char * name);

//Queries
int poop(char * file_name);
int diarrhea(char * database_name);
