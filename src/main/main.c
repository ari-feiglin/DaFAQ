#include "dafaq.h"

char * magic = "DaFAQ";
char * extension = ".dfq";
char * operations[] = {"==", "!=", ">", "<", ">=", "<="};
int num_of_operations = sizeof(operations) / sizeof(operations[0]);
int magic_len = 0;
int extension_len = 0;

int main(int argc, char ** argv){
    char * database_name = "DaFAQ Database";
    
    int error_check = 0;
    record_field ** record = NULL;
    int fd = 0;
    int sort_fd = 0;
    int record_numbers = 0;
    int num_of_fields = 0;
    int i = 0;
    int j = 1;
    char target[] = "zzzz";
    int itarget = 20;
    bool * map = NULL;

    magic_len = strnlen(magic, STRING_LEN);
    extension_len = strnlen(extension, STRING_LEN);

    if(2 == argc){
        database_name = argv[1];
    }

    print_menu();
}
